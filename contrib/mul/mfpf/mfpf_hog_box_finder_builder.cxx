//:
// \file
// \brief Builder for mfpf_hog_box_finder objects.
// \author Tim Cootes


#include <mfpf/mfpf_hog_box_finder_builder.h>
#include <mfpf/mfpf_hog_box_finder.h>
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>

#include <vil/vil_resample_bilin.h>
#include <vil/vil_print.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_indent.h>
#include <vcl_algorithm.h>

#include <mipa/mipa_orientation_histogram.h>
#include <mipa/mipa_sample_histo_boxes.h>

//: Divide elements of v by sum of last nA elements
//  For histogram vectors these are the total sums
inline void mfpf_norm_histo_vec(vnl_vector<double>& v, unsigned nA)
{
  unsigned n=v.size();
  double sum = 0.0;
  for (unsigned i=1;i<=nA;++i) sum+=v[n-i];
  v/=sum;
}

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_hog_box_finder_builder::mfpf_hog_box_finder_builder()
{
  set_defaults();
}

//: Define default values
void mfpf_hog_box_finder_builder::set_defaults()
{
  step_size_=1.0;
  search_ni_=5;
  search_nj_=5;

  nA_bins_=8;
  full360_=true;
  nc_=2;
  ni_=0;
  nj_=0;
  ref_x_=0;
  ref_y_=0;
  norm_method_=0;
  overlap_f_=1.0;
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_hog_box_finder_builder::~mfpf_hog_box_finder_builder()
{
}

//: Create new mfpf_hog_box_finder on heap
mfpf_point_finder* mfpf_hog_box_finder_builder::new_finder() const
{
  return new mfpf_hog_box_finder();
}

void mfpf_hog_box_finder_builder::set_angle_bins(unsigned nA_bins, 
          bool full360, unsigned cell_size)
{
  nA_bins_ = nA_bins;
  full360_  = full360;
  nc_      = cell_size;
}

//: Define region size in world co-ordinates
//  Sets up ROI to cover given box (with samples at step_size()), 
//  with ref point at centre.
void mfpf_hog_box_finder_builder::set_region_size(double wi, double wj)
{
  wi/=(2*nc_*step_size());
  wj/=(2*nc_*step_size());
  int ni = vcl_max(2,int(0.99+wi));
  int nj = vcl_max(2,int(0.99+wj));
  set_as_box(unsigned(ni),unsigned(nj),0.5*(ni-1),0.5*(nj-1));
}


//: Define model region as an ni x nj box
void mfpf_hog_box_finder_builder::set_as_box(unsigned ni, unsigned nj,
                                         double ref_x, double ref_y,
                                         const mfpf_vec_cost_builder& builder)
{
  set_as_box(ni,nj,ref_x,ref_y);
  cost_builder_ = builder.clone();
}

//: Define model region as an ni x nj box
void mfpf_hog_box_finder_builder::set_as_box(unsigned ni, unsigned nj,
                                         double ref_x, double ref_y)
{
  ni_=ni; nj_=nj;

  ref_x_=ref_x;
  ref_y_=ref_y;
}


//: Define model region as an ni x nj box
void mfpf_hog_box_finder_builder::set_as_box(unsigned ni, unsigned nj,
                                  const mfpf_vec_cost_builder& builder)
{
  set_as_box(ni,nj, 0.5*(ni-1),0.5*(nj-1), builder);
}


//: Initialise building
// Must be called before any calls to add_example(...)
void mfpf_hog_box_finder_builder::clear(unsigned n_egs)
{
  unsigned n_per_eg = (1+2*nA_);
  cost_builder().clear(n_egs*n_per_eg);
}

//: Add one example to the model
void mfpf_hog_box_finder_builder::add_one_example(
                 const vimt_image_2d_of<float>& image,
                 const vgl_point_2d<double>& p,
                 const vgl_vector_2d<double>& u)
{
  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());

  assert(image.image().nplanes()==1);

  // Set up sample area with 1 unit border
  unsigned sni = 2+2*nc_*ni_;
  unsigned snj = 2+2*nc_*nj_;
  vil_image_view<float> sample(sni,snj);

  const vgl_point_2d<double> p0 = p-(1+nc_*ref_x_)*u1-(1+nc_*ref_y_)*v1;

  const vimt_transform_2d& s_w2i = image.world2im();
  vgl_point_2d<double> im_p0 = s_w2i(p0);
  vgl_vector_2d<double> im_u = s_w2i.delta(p0, u1);
  vgl_vector_2d<double> im_v = s_w2i.delta(p0, v1);

  vil_resample_bilin(image.image(),sample,
                      im_p0.x(),im_p0.y(),  im_u.x(),im_u.y(),
                      im_v.x(),im_v.y(),
                      sni,snj);

  vil_image_view<float> histo_im;
  mipa_orientation_histogram(sample,histo_im,nA_bins_,nc_,full360_);

  vnl_vector<double> v;
  mipa_sample_histo_boxes_3L(histo_im,0,0,v,ni_,nj_);

  if (norm_method_==1) mfpf_norm_histo_vec(v,nA_bins_);

  cost_builder().add_example(v);
}

//: Add one example to the model
void mfpf_hog_box_finder_builder::add_example(const vimt_image_2d_of<float>& image,
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
    vgl_vector_2d<double> uA = u*vcl_cos(A)+v*vcl_sin(A);
    add_one_example(image,p,uA);
  }
}

//: Build this object from the data supplied in add_example()
void mfpf_hog_box_finder_builder::build(mfpf_point_finder& pf)
{
  assert(pf.is_a()=="mfpf_hog_box_finder");
  mfpf_hog_box_finder& rp = static_cast<mfpf_hog_box_finder&>(pf);

  mfpf_vec_cost *cost = cost_builder().new_cost();

  cost_builder().build(*cost);

  rp.set(nA_bins_,full360_,ni_,nj_,nc_,
         ref_x_,ref_y_,*cost,norm_method_);
  set_base_parameters(rp);
  rp.set_overlap_f(overlap_f_);

vcl_cout<<"Model: "<<rp<<vcl_endl;

  // Tidy up
  delete cost;
}



//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mfpf_hog_box_finder_builder::set_from_stream(vcl_istream &is)
{
  // Cycle through string and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  set_defaults();

  // Extract the properties
  parse_base_props(props);

  nc_=vul_string_atoi(props.get_optional_property("nc","2"));
  nA_bins_=vul_string_atoi(props.get_optional_property("nA_bins","8"));
  full360_=vul_string_to_bool(props.get_optional_property("full360","true"));
  ni_=vul_string_atoi(props.get_optional_property("ni","4"));
  nj_=vul_string_atoi(props.get_optional_property("nj","4"));

  if (props.find("norm")!=props.end())
  {
    if (props["norm"]=="none") norm_method_=0;
    else
    if (props["norm"]=="linear") norm_method_=1;
    else throw mbl_exception_parse_error("Unknown norm: "+props["norm"]);

    props.erase("norm");
  }

  overlap_f_=vul_string_atof(props.get_optional_property("overlap_f","1.0"));

  if (props.find("ref_x")!=props.end())
  {
    ref_x_=vul_string_atof(props["ref_x"]);
    props.erase("ref_x");
  }
  else ref_x_=0.5*(ni_-1);

  if (props.find("ref_y")!=props.end())
  {
    ref_y_=vul_string_atof(props["ref_y"]);
    props.erase("ref_y");
  }
  else ref_y_=0.5*(nj_-1);


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

  if (props.find("cost_builder")!=props.end())
  {
    vcl_istringstream b_ss(props["cost_builder"]);
    vcl_auto_ptr<mfpf_vec_cost_builder> bb =
         mfpf_vec_cost_builder::create_from_stream(b_ss);
    cost_builder_ = bb->clone();
    props.erase("cost_builder");
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_hog_box_finder_builder::set_from_stream", props, mbl_read_props_type());
  return true;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_hog_box_finder_builder::is_a() const
{
  return vcl_string("mfpf_hog_box_finder_builder");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder_builder* mfpf_hog_box_finder_builder::clone() const
{
  return new mfpf_hog_box_finder_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_hog_box_finder_builder::print_summary(vcl_ostream& os) const
{
  os << "{ "<<vcl_endl;
  vsl_indent_inc(os);
  os<<vsl_indent()<<"size: " << ni_ << 'x' << nj_
     << " nc: " << nc_ <<" nA_bins: "<<nA_bins_
     << " ref_pt: (" << ref_x_ << ',' << ref_y_ << ')' <<vcl_endl;
  if (full360_) os<<vsl_indent()<<"Angle range: 0-360"<<vcl_endl;
  else          os<<vsl_indent()<<"Angle range: 0-180"<<vcl_endl;
  if (norm_method_==0) os<<vsl_indent()<<"norm: none"<<vcl_endl;
  else                 os<<vsl_indent()<<"norm: linear"<<vcl_endl;
  os <<vsl_indent()<< "cost_builder: ";
  if (cost_builder_.ptr()==0) os << '-'<<vcl_endl;
  else                       os << cost_builder_<<vcl_endl;
  os <<vsl_indent()<< "nA: " << nA_ << " dA: " << dA_ << ' '<<vcl_endl;
  os <<vsl_indent();
  mfpf_point_finder_builder::print_summary(os);
  os <<vcl_endl;
  os <<vsl_indent()<<"overlap_f: "<<overlap_f_<<vcl_endl;
  vsl_indent_dec(os);
  os <<vsl_indent()<< "}";
}

//: Version number for I/O
short mfpf_hog_box_finder_builder::version_no() const
{
  return 2;
}

void mfpf_hog_box_finder_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  mfpf_point_finder_builder::b_write(bfs);  // Save base class
  vsl_b_write(bfs,nc_);
  vsl_b_write(bfs,ni_);
  vsl_b_write(bfs,nj_);
  vsl_b_write(bfs,nA_bins_);
  vsl_b_write(bfs,full360_);
  vsl_b_write(bfs,ref_x_);
  vsl_b_write(bfs,ref_y_);
  vsl_b_write(bfs,nA_);
  vsl_b_write(bfs,dA_);
  vsl_b_write(bfs,cost_builder_);
  vsl_b_write(bfs,norm_method_);
  vsl_b_write(bfs,overlap_f_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_hog_box_finder_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
    case (2):
      mfpf_point_finder_builder::b_read(bfs);  // Load base class
      vsl_b_read(bfs,nc_);
      vsl_b_read(bfs,ni_);
      vsl_b_read(bfs,nj_);
      vsl_b_read(bfs,nA_bins_);
      vsl_b_read(bfs,full360_);
      vsl_b_read(bfs,ref_x_);
      vsl_b_read(bfs,ref_y_);
      vsl_b_read(bfs,nA_);
      vsl_b_read(bfs,dA_);
      vsl_b_read(bfs,cost_builder_);
      vsl_b_read(bfs,norm_method_);
      if (version==1) overlap_f_=1.0;
      else            vsl_b_read(bfs,overlap_f_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
