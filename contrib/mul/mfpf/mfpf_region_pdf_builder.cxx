//:
// \file
// \brief Builder for mfpf_region_pdf objects.
// \author Tim Cootes


#include <mfpf/mfpf_region_pdf_builder.h>
#include <mfpf/mfpf_region_pdf.h>
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>

#include <vil/vil_resample_bilin.h>
#include <vsl/vsl_vector_io.h>

#include <mfpf/mfpf_sample_region.h>
#include <mfpf/mfpf_norm_vec.h>
#include <mbl/mbl_data_array_wrapper.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_region_pdf_builder::mfpf_region_pdf_builder()
{
  set_defaults();
}

//: Define default values
void mfpf_region_pdf_builder::set_defaults()
{
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
}

void mfpf_region_pdf_builder::set_step_size(double s)
{
  step_size_=s;
}


//=======================================================================
// Destructor
//=======================================================================

mfpf_region_pdf_builder::~mfpf_region_pdf_builder()
{
}

//: Create new mfpf_region_pdf on heap
mfpf_point_finder* mfpf_region_pdf_builder::new_finder() const
{
  return new mfpf_region_pdf();
}

//: Define model region as an ni x nj box
void mfpf_region_pdf_builder::set_as_box(unsigned ni, unsigned nj,
                                         double ref_x, double ref_y,
                                         const vpdfl_builder_base& builder)
{
  set_as_box(ni,nj,ref_x,ref_y);
  pdf_builder_ = builder.clone();
}

//: Define model region as an ni x nj box
void mfpf_region_pdf_builder::set_as_box(unsigned ni, unsigned nj,
                                         double ref_x, double ref_y)
{
  roi_ni_=ni; roi_nj_=nj;
  n_pixels_ = ni*nj;

  // Set ROI to be a box
  roi_.resize(nj);
  for (unsigned j=0;j<nj;++j) roi_.push_back(mbl_chord(0,ni-1,j));

  ref_x_=ref_x;
  ref_y_=ref_y;
}


//: Define model region as an ni x nj box
void mfpf_region_pdf_builder::set_as_box(unsigned ni, unsigned nj,
                                         const vpdfl_builder_base& builder)
{
  set_as_box(ni,nj, 0.5*(ni-1),0.5*(nj-1), builder);
}

//: Define model region as an ellipse with radii ri, rj
//  Ref. point in centre.
void mfpf_region_pdf_builder::set_as_ellipse(double ri, double rj,
                                             const vpdfl_builder_base& builder)
{
  set_as_ellipse(ri,rj);
  pdf_builder_ = builder.clone();
}

//: Define model region as an ellipse with radii ri, rj
//  Ref. point in centre.
void mfpf_region_pdf_builder::set_as_ellipse(double ri, double rj)
{
  ri+=1e-6; rj+=1e-6;
  int ni=int(ri);
  int nj=int(rj);
  roi_.resize(0);
  n_pixels_=0;
  for (int j = -nj;j<=nj;++j)
  {
    // Find start and end of line of pixels inside disk
    int x = int(ri*vcl_sqrt(1.0-j*j/(rj*rj)));
    roi_.push_back(mbl_chord(ni-x,ni+x,nj+j));
    n_pixels_+=2*x+1;
  }

  ref_x_=ni;
  ref_y_=nj;
  roi_ni_=2*ni+1;
  roi_nj_=2*nj+1;
}

//: Initialise building
// Must be called before any calls to add_example(...)
void mfpf_region_pdf_builder::clear(unsigned n_egs)
{
  data_.resize(0);
}

//: Add one example to the model
void mfpf_region_pdf_builder::add_one_example(const vimt_image_2d_of<float>& image,
                                              const vgl_point_2d<double>& p,
                                              const vgl_vector_2d<double>& u)
{
  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());

  unsigned np=image.image().nplanes();
  // Set up sample area with interleaved planes (ie planestep==1)
  vil_image_view<float> sample(roi_ni_,roi_nj_,1,np);

  const vgl_point_2d<double> p0 = p-ref_x_*u1-ref_y_*v1;

  const vimt_transform_2d& s_w2i = image.world2im();
  vgl_point_2d<double> im_p0 = s_w2i(p0);
  vgl_vector_2d<double> im_u = s_w2i.delta(p0, u1);
  vgl_vector_2d<double> im_v = s_w2i.delta(p0, v1);

  vil_resample_bilin(image.image(),sample,
                     im_p0.x(),im_p0.y(),  im_u.x(),im_u.y(),
                     im_v.x(),im_v.y(),roi_ni_,roi_nj_);

  vnl_vector<double> v(n_pixels_*sample.nplanes());
  mfpf_sample_region(sample.top_left_ptr(),sample.jstep(),
                     np,roi_,v);

  mfpf_norm_vec(v);
  data_.push_back(v);
}

//: Add one example to the model
void mfpf_region_pdf_builder::add_example(const vimt_image_2d_of<float>& image,
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
void mfpf_region_pdf_builder::build(mfpf_point_finder& pf)
{
  assert(pf.is_a()=="mfpf_region_pdf");
  mfpf_region_pdf& rp = static_cast<mfpf_region_pdf&>(pf);
  rp.set_search_area(search_ni_,search_nj_);

  vpdfl_pdf_base *pdf = pdf_builder().new_model();
  mbl_data_array_wrapper<vnl_vector<double> > data(&data_[0],data_.size());

  pdf_builder().build(*pdf,data);

  rp.set(roi_,ref_x_,ref_y_,*pdf);
  rp.set_step_size(step_size_);

  // Tidy up
  delete pdf;
  data_.resize(0);
}

//: Parse stream to set up as a box shape.
// Expects: "{ ni: 3 nj: 5 ref_x: 1.0 ref_y: 2.0 }
void mfpf_region_pdf_builder::config_as_box(vcl_istream &is)
{
  // Cycle through string and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
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
      "mfpf_region_pdf_builder::config_as_box",
      props, mbl_read_props_type());

  set_as_box(ni,nj,ref_x_,ref_y_);
}

//: Parse stream to set up as an ellipse shape.
// Expects: "{ ri: 2.1 rj: 5.2 }
void mfpf_region_pdf_builder::config_as_ellipse(vcl_istream &is)
{
  // Cycle through string and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
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
      "mfpf_region_pdf_builder::config_as_ellipse",
      props, mbl_read_props_type());

  set_as_ellipse(ri,rj);
}


//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mfpf_region_pdf_builder::set_from_stream(vcl_istream &is)
{
  // Cycle through string and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  set_defaults();

  // Extract the properties
  if (props.find("shape")!=props.end())
  {
    vcl_istringstream shape_s(props["shape"]);
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

  if (props.find("search_ni")!=props.end())
  {
    search_ni_=vul_string_atoi(props["search_ni"]);
    props.erase("search_ni");
  }

  if (props.find("search_nj")!=props.end())
  {
    search_nj_=vul_string_atoi(props["search_nj"]);
    props.erase("search_nj");
  }

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

  if (props.find("pdf_builder")!=props.end())
  {
    vcl_istringstream b_ss(props["pdf_builder"]);
    vcl_auto_ptr<vpdfl_builder_base> bb =
         vpdfl_builder_base::new_pdf_builder_from_stream(b_ss);
    pdf_builder_ = bb->clone();
    props.erase("pdf_builder");
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_region_pdf_builder::set_from_stream", props, mbl_read_props_type());
  return true;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_region_pdf_builder::is_a() const
{
  return vcl_string("mfpf_region_pdf_builder");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder_builder* mfpf_region_pdf_builder::clone() const
{
  return new mfpf_region_pdf_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_region_pdf_builder::print_summary(vcl_ostream& os) const
{
  os << "{ step_size: " << step_size_
     << " size: " << roi_ni_ << 'x' << roi_nj_
     << " n_pixels: " << n_pixels_
     << " ref_pt: (" << ref_x_ << ',' << ref_y_ << ')'
     << " pdf_builder: ";
  if (pdf_builder_.ptr()==0) os << '-';
  else                       os << pdf_builder_;
  os << " search_ni: " << search_ni_
     << " search_nj: " << search_nj_
     << " nA: " << nA_ << " dA: " << dA_
     << '}';
}

void mfpf_region_pdf_builder::print_shape(vcl_ostream& os) const
{
  vil_image_view<vxl_byte> im(roi_ni_,roi_nj_);
  im.fill(0);
  for (unsigned k=0;k<roi_.size();++k)
    for (int i=roi_[k].start_x();i<=roi_[k].end_x();++i)
      im(i,roi_[k].y())=1;
  for (unsigned j=0;j<im.nj();++j)
  {
    for (unsigned i=0;i<im.ni();++i)
      if (im(i,j)==0) os<<' ';
      else            os<<'X';
    os<<vcl_endl;
  }
}


void mfpf_region_pdf_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,step_size_);
  vsl_b_write(bfs,roi_);
  vsl_b_write(bfs,roi_ni_);
  vsl_b_write(bfs,roi_nj_);
  vsl_b_write(bfs,n_pixels_);
  vsl_b_write(bfs,ref_x_);
  vsl_b_write(bfs,ref_y_);
  vsl_b_write(bfs,search_ni_);
  vsl_b_write(bfs,search_nj_);
  vsl_b_write(bfs,nA_);
  vsl_b_write(bfs,dA_);
  vsl_b_write(bfs,pdf_builder_);
  vsl_b_write(bfs,data_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_region_pdf_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,step_size_);
      vsl_b_read(bfs,roi_);
      vsl_b_read(bfs,roi_ni_);
      vsl_b_read(bfs,roi_nj_);
      vsl_b_read(bfs,n_pixels_);
      vsl_b_read(bfs,ref_x_);
      vsl_b_read(bfs,ref_y_);
      vsl_b_read(bfs,search_ni_);
      vsl_b_read(bfs,search_nj_);
      vsl_b_read(bfs,nA_);
      vsl_b_read(bfs,dA_);
      vsl_b_read(bfs,pdf_builder_);
      vsl_b_read(bfs,data_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
