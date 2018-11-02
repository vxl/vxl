#include <iostream>
#include <cmath>
#include <sstream>
#include "mfpf_lin_clsfy_finder_builder.h"
//:
// \file
// \brief Builds mfpf_region_finder objects which use a linear classifier
// \author Tim Cootes

#include <mfpf/mfpf_region_finder.h>
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>

#include <vil/vil_resample_bilin.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_indent.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <mfpf/mfpf_sample_region.h>
#include <mfpf/mfpf_norm_vec.h>
#include <mfpf/mfpf_log_lin_class_cost.h>

#include <clsfy/clsfy_binary_hyperplane.h>
#include <clsfy/clsfy_binary_hyperplane_logit_builder.h>
#include <mbl/mbl_data_array_wrapper.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_lin_clsfy_finder_builder::mfpf_lin_clsfy_finder_builder()
{
  set_defaults();
}

//: Define default values
void mfpf_lin_clsfy_finder_builder::set_defaults()
{
  r1_=1.8;
  r2_=2.9;
  r3_=7.1;
  step_size_=1.0;
  search_ni_=5;
  search_nj_=5;
  n_pixels_=0;
  roi_.resize(0);
  roi_ni_=0;
  roi_nj_=0;
  ref_x_=0;
  ref_y_=0;
  nA_=0;
  dA_=0.0;
  norm_method_=1;
  overlap_f_=1.0;
  var_min_ = 1.0E-6;
  tvar_min_=1.0E30;
  num_examples_=0;
  estimate_var_min_=true;
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_lin_clsfy_finder_builder::~mfpf_lin_clsfy_finder_builder() = default;

//: Create new mfpf_region_finder on heap
mfpf_point_finder* mfpf_lin_clsfy_finder_builder::new_finder() const
{
  return new mfpf_region_finder();
}

//: Define model region using description in form
//  Assumes form defined in world-coords.
//  Assumes step_size() pixel units (ie dimensions
//  are divided by step_size() to map to reference frame).
void mfpf_lin_clsfy_finder_builder::set_region(const mfpf_region_form& form)
{
  step_size_ = form.pose().scale();

  if (form.form()=="box")
  {
    int ni = std::max(1,int(0.99+form.wi()));
    int nj = std::max(1,int(0.99+form.wj()));
    set_as_box(unsigned(ni),unsigned(nj),0.5*ni,0.5*nj);
  }
  else
  if (form.form()=="ellipse")
  {
    double ri = std::max(1.0,form.wi());
    double rj = std::max(1.0,form.wj());
    set_as_ellipse(ri,rj);
  }
  else
  {
    std::cerr<<"mfpf_lin_clsfy_finder_builder::set_region : Unknown form: "<<form<<std::endl;
    std::abort();
  }
}

//: Define region size in world co-ordinates
//  Sets up ROI to cover given box (with samples at step_size()),
//  with ref point at centre.
void mfpf_lin_clsfy_finder_builder::set_region_size(double wi, double wj)
{
  wi/=step_size();
  wj/=step_size();
  int ni = std::max(1,int(0.99+wi));
  int nj = std::max(1,int(0.99+wj));
  set_as_box(unsigned(ni),unsigned(nj),0.5*(ni-1),0.5*(nj-1));
}


//: Define model region as an ni x nj box
void mfpf_lin_clsfy_finder_builder::set_as_box(unsigned ni, unsigned nj)
{
  set_as_box(ni,nj,0.5*(ni-1),0.5*(nj-1));
}

//: Define model region as an ni x nj box
void mfpf_lin_clsfy_finder_builder::set_as_box(unsigned ni, unsigned nj,
                                               double ref_x, double ref_y)
{
  roi_ni_=ni; roi_nj_=nj;
  n_pixels_ = ni*nj;

  // Set ROI to be a box
  roi_.resize(nj);
  for (unsigned j=0;j<nj;++j) roi_[j]=mbl_chord(0,ni-1,j);

  ref_x_=ref_x;
  ref_y_=ref_y;
}

//: Define model region as an ellipse with radii ri, rj
//  Ref. point in centre.
void mfpf_lin_clsfy_finder_builder::set_as_ellipse(double ri, double rj)
{
  ri+=1e-6; rj+=1e-6;
  int ni=int(ri);
  int nj=int(rj);
  roi_.resize(0);
  n_pixels_=0;
  for (int j = -nj;j<=nj;++j)
  {
    // Find start and end of line of pixels inside disk
    int x = int(ri*std::sqrt(1.0-j*j/(rj*rj)));
    roi_.emplace_back(ni-x,ni+x,nj+j);
    n_pixels_+=2*x+1;
  }

  ref_x_=ni;
  ref_y_=nj;
  roi_ni_=2*ni+1;
  roi_nj_=2*nj+1;
}

//: Which normalisation to use (0=none, 1=linear)
void mfpf_lin_clsfy_finder_builder::set_norm_method(short norm_method)
{
  norm_method_=norm_method;
}

//: Number of dimensions in the model
unsigned mfpf_lin_clsfy_finder_builder::model_dim()
{
  return n_pixels_;
}

//: Initialise building
// Must be called before any calls to add_example(...)
void mfpf_lin_clsfy_finder_builder::clear(unsigned  /*n_egs*/)
{
  num_examples_=0;
}

//: Add one example to the model
//  Sample regions in a grid centred on supplied point.
//  Record those close to the centre as positive examples, and those
//  further away as negative examples.
void mfpf_lin_clsfy_finder_builder::add_one_example(
                 const vimt_image_2d_of<float>& image,
                 const vgl_point_2d<double>& p,
                 const vgl_vector_2d<double>& u)
{
  // Determine axes
  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());

  // Compute size of region to sample
  int r = int(r3_+1.0);
  int nsi = 2*r + roi_ni_;
  int nsj = 2*r + roi_nj_;


  unsigned np=image.image().nplanes();
  // Set up sample area with interleaved planes (ie planestep==1)
  vil_image_view<float> sample(nsi,nsj,1,np);

  const vgl_point_2d<double> p0 = p-(r+ref_x_)*u1-(r+ref_y_)*v1;

  const vimt_transform_2d& s_w2i = image.world2im();
  vgl_point_2d<double> im_p0 = s_w2i(p0);
  vgl_vector_2d<double> im_u = s_w2i.delta(p0, u1);
  vgl_vector_2d<double> im_v = s_w2i.delta(p0, v1);

  vil_resample_bilin(image.image(),sample,
                     im_p0.x(),im_p0.y(),  im_u.x(),im_u.y(),
                     im_v.x(),im_v.y(),nsi,nsj);

  vnl_vector<double> v(n_pixels_*sample.nplanes());

  const float* s = sample.top_left_ptr();
  std::ptrdiff_t s_istep = sample.istep();
  std::ptrdiff_t s_jstep = sample.jstep();

  for (int j=-r;j<=r;++j,s+=s_jstep)
  {
    const float *sp = s;
    for (int i=-r;i<=r;++i,sp+=s_istep)
    {
      double d = std::sqrt(static_cast<double>(i*i+j*j));
      if (d<=r3_)
      {
        mfpf_sample_region(sp,sample.jstep(),np,roi_,v);
        if (norm_method_==1)
        {
            double var=var_min_;
            mfpf_norm_vec(v,var_min_,&var);
            if (var<tvar_min_) tvar_min_ = var;
        }

        if (d<=r1_)
        {
          samples_.push_back(v); class_id_.push_back(1);  // Positive example
        }
        else
        if (d>=r2_)
        {
          samples_.push_back(v); class_id_.push_back(0);  // Negative example
        }
      }
    }
  }

  ++num_examples_;
}

//: Add one example to the model
void mfpf_lin_clsfy_finder_builder::add_example(const vimt_image_2d_of<float>& image,
                                                const vgl_point_2d<double>& p,
                                                const vgl_vector_2d<double>& u)
{
  if (nA_==0)
  {
    add_one_example(image,p,u);
    return;
  }

  vgl_vector_2d<double> v(-u.y(),u.x());
  for (int iA=-int(nA_);iA<=(int)nA_;++iA)
  {
    double A = iA*dA_;
    vgl_vector_2d<double> uA = u*std::cos(A)+v*std::sin(A);
    add_one_example(image,p,uA);
  }
}

//: Build this object from the data supplied in add_example()
void mfpf_lin_clsfy_finder_builder::build(mfpf_point_finder& pf)
{
  assert(pf.is_a()=="mfpf_region_finder");
  auto& rp = static_cast<mfpf_region_finder&>(pf);

  double min_p = 0.001;  // Smallest prob to be returned by classifier
  mbl_data_array_wrapper<vnl_vector<double> > sample_data(samples_);
  clsfy_binary_hyperplane_logit_builder builder;
  builder.set_alpha(1e-6);
  builder.set_min_p(min_p);
  clsfy_binary_hyperplane classifier;
  double mean_error = builder.build(classifier,sample_data,class_id_);
  std::cout<<"Classifier Mean Error: "<<mean_error<<std::endl;

  mfpf_log_lin_class_cost log_lin_cost;
  log_lin_cost.set(classifier.weights(),classifier.bias(),min_p);
  rp.set(roi_,ref_x_,ref_y_,log_lin_cost,norm_method_);
  set_base_parameters(rp);
  rp.set_overlap_f(overlap_f_);

  if (estimate_var_min_ && norm_method_==1 && num_examples_>0)
  {
    //Assume applied var_min is r* min in training set, where r->1 as n->infinity
    //Set r=0.98 for n around 50
    auto dn=double(num_examples_);
    if (dn>0.0)
    {
      double r=0.925; //so r attains 0.98 around n=50
      double alpha=1.0;
      if (dn>50.0)
        alpha=0.98;
      else
      {
        alpha=1-std::pow(r,dn);
        alpha=std::min(alpha,0.98);
      }
      tvar_min_ *= alpha;
      var_min_ = std::max(var_min_,tvar_min_);
    }
    rp.set_var_min(var_min_);
  }
}

//: Parse stream to set up as a box shape.
// Expects: "{ ni: 3 nj: 5 ref_x: 1.0 ref_y: 2.0 }
void mfpf_lin_clsfy_finder_builder::config_as_box(std::istream &is)
{
  // Cycle through string and produce a map of properties
  std::string s = mbl_parse_block(is);
  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  unsigned ni=5,nj=5;

  if (props.find("ni")!=props.end())
  {
    ni=vul_string_atoi(props["ni"]);
    props.erase("ni");
  }
  if (props.find("nj")!=props.end())
  {
    nj=vul_string_atoi(props["nj"]);
    props.erase("nj");
  }

  if (props.find("ref_x")!=props.end())
  {
    ref_x_=vul_string_atof(props["ref_x"]);
    props.erase("ref_x");
  }
  else ref_x_=0.5*(ni-1.0);

  if (props.find("ref_y")!=props.end())
  {
    ref_y_=vul_string_atof(props["ref_y"]);
    props.erase("ref_y");
  }
  else ref_y_=0.5*(nj-1.0);

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_lin_clsfy_finder_builder::config_as_box",
      props, mbl_read_props_type());

  set_as_box(ni,nj,ref_x_,ref_y_);
}

//: Parse stream to set up as an ellipse shape.
// Expects: "{ ri: 2.1 rj: 5.2 }
void mfpf_lin_clsfy_finder_builder::config_as_ellipse(std::istream &is)
{
  // Cycle through string and produce a map of properties
  std::string s = mbl_parse_block(is);
  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  double ri=3.1,rj=3.1;
  if (props.find("ri")!=props.end())
  {
    ri=vul_string_atof(props["ri"]);
    props.erase("ri");
  }

  if (props.find("rj")!=props.end())
  {
    rj=vul_string_atof(props["rj"]);
    props.erase("rj");
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_lin_clsfy_finder_builder::config_as_ellipse",
      props, mbl_read_props_type());

  set_as_ellipse(ri,rj);
}


//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mfpf_lin_clsfy_finder_builder::set_from_stream(std::istream &is)
{
  // Cycle through string and produce a map of properties
  std::string s = mbl_parse_block(is);
  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  set_defaults();

  // Extract the properties
  parse_base_props(props);

  if (props.find("shape")!=props.end())
  {
    std::istringstream shape_s(props["shape"]);
    shape_s>>shape_;
    if (shape_=="box")
    {
      // Parse parameters after box
      config_as_box(shape_s);
    }
    else
    if (shape_=="ellipse")
    {
      // Parse parameters after ellipse
      config_as_ellipse(shape_s);
    }
    else throw mbl_exception_parse_error("Unknown shape: "+shape_);

    props.erase("shape");
  }

  if (props.find("norm")!=props.end())
  {
    if (props["norm"]=="none") norm_method_=0;
    else
    if (props["norm"]=="linear") norm_method_=1;
    else throw mbl_exception_parse_error("Unknown norm: "+props["norm"]);

    props.erase("norm");
  }
  if (props.find("estimate_var_min") !=props.end())
  {
    std::string strEstimate=props["estimate_var_min"];
    if (strEstimate[0]=='f' || strEstimate[0]=='F' || strEstimate[0]=='0')
        estimate_var_min_=false;
    else
        estimate_var_min_=true;

    props.erase("estimate_var_min");
  }

  overlap_f_=vul_string_atof(props.get_optional_property("overlap_f",
                                                         "1.0"));
  r1_=vul_string_atof(props.get_optional_property("r1","1.7"));
  r2_=vul_string_atof(props.get_optional_property("r2","2.9"));
  r3_=vul_string_atof(props.get_optional_property("r3","7.1"));

  if (props.find("nA")!=props.end())
  {
    nA_=vul_string_atoi(props["nA"]);
    props.erase("nA");
  }

  if (props.find("dA")!=props.end())
  {
    dA_=vul_string_atof(props["dA"]);
    props.erase("dA");
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_lin_clsfy_finder_builder::set_from_stream", props, mbl_read_props_type());
  return true;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_lin_clsfy_finder_builder::is_a() const
{
  return std::string("mfpf_lin_clsfy_finder_builder");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder_builder* mfpf_lin_clsfy_finder_builder::clone() const
{
  return new mfpf_lin_clsfy_finder_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_lin_clsfy_finder_builder::print_summary(std::ostream& os) const
{
  os << "{ size: " << roi_ni_ << 'x' << roi_nj_
     << " n_pixels: " << n_pixels_
     << " ref_pt: (" << ref_x_ << ',' << ref_y_ << ')' <<'\n';
  vsl_indent_inc(os);
  if (norm_method_==0) os<<vsl_indent()<<"norm: none"<<'\n';
  else                 os<<vsl_indent()<<"norm: linear"<<'\n';
  os <<vsl_indent()<< "r1: "<<r1_<<" r2: "<<r2_<<" r3: "<<r3_<<'\n'
     <<vsl_indent()<< "nA: " << nA_ << " dA: " << dA_ << ' '<<'\n'
     <<vsl_indent();
  mfpf_point_finder_builder::print_summary(os);
  os <<'\n' <<vsl_indent()<<"overlap_f: "<<overlap_f_<<'\n';
  vsl_indent_dec(os);
  os <<vsl_indent()<< '}';
}

void mfpf_lin_clsfy_finder_builder::print_shape(std::ostream& os) const
{
  vil_image_view<vxl_byte> im(roi_ni_,roi_nj_);
  im.fill(0);
  for (auto k : roi_)
    for (int i=k.start_x();i<=k.end_x();++i)
      im(i,k.y())=1;
  for (unsigned j=0;j<im.nj();++j)
  {
    for (unsigned i=0;i<im.ni();++i)
      if (im(i,j)==0) os<<' ';
      else            os<<'X';
    os<<'\n';
  }
}

//: Version number for I/O
short mfpf_lin_clsfy_finder_builder::version_no() const
{
  return 1;
}

void mfpf_lin_clsfy_finder_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  mfpf_point_finder_builder::b_write(bfs);  // Save base class
  vsl_b_write(bfs,roi_);
  vsl_b_write(bfs,roi_ni_);
  vsl_b_write(bfs,roi_nj_);
  vsl_b_write(bfs,n_pixels_);
  vsl_b_write(bfs,ref_x_);
  vsl_b_write(bfs,ref_y_);
  vsl_b_write(bfs,nA_);
  vsl_b_write(bfs,dA_);
  vsl_b_write(bfs,r1_);
  vsl_b_write(bfs,r2_);
  vsl_b_write(bfs,r3_);
  vsl_b_write(bfs,norm_method_);
  vsl_b_write(bfs,overlap_f_);
  vsl_b_write(bfs,var_min_);
  vsl_b_write(bfs,samples_);
  vsl_b_write(bfs,class_id_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_lin_clsfy_finder_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      mfpf_point_finder_builder::b_read(bfs);  // Load base class
      vsl_b_read(bfs,roi_);
      vsl_b_read(bfs,roi_ni_);
      vsl_b_read(bfs,roi_nj_);
      vsl_b_read(bfs,n_pixels_);
      vsl_b_read(bfs,ref_x_);
      vsl_b_read(bfs,ref_y_);
      vsl_b_read(bfs,nA_);
      vsl_b_read(bfs,dA_);
      vsl_b_read(bfs,r1_);
      vsl_b_read(bfs,r2_);
      vsl_b_read(bfs,r3_);
      vsl_b_read(bfs,norm_method_);
      vsl_b_read(bfs,overlap_f_);
      vsl_b_read(bfs,var_min_);
      vsl_b_read(bfs,samples_);
      vsl_b_read(bfs,class_id_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
