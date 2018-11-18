#include <iostream>
#include <cmath>
#include <sstream>
#include "mfpf_grad_corr2d_builder.h"
//:
// \file
// \brief Builder for mfpf_grad_corr2d objects.
// \author Tim Cootes

#include <mfpf/mfpf_grad_corr2d.h>
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>

#include <vil/vil_resample_bilin.h>
#include <vil/vil_math.h>
#include <vil/io/vil_io_image_view.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_grad_corr2d_builder::mfpf_grad_corr2d_builder()
{
  set_defaults();
}

//: Define default values
void mfpf_grad_corr2d_builder::set_defaults()
{
  step_size_=1.0;
  ni_=7; nj_=7;
  ref_x_=0.5*(ni_-1);
  ref_y_=0.5*(nj_-1);
  search_ni_=5;
  search_nj_=5;
  nA_=0;
  dA_=0.0;
  overlap_f_=1.0;
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_grad_corr2d_builder::~mfpf_grad_corr2d_builder() = default;

//: Create new mfpf_grad_corr2d on heap
mfpf_point_finder* mfpf_grad_corr2d_builder::new_finder() const
{
  return new mfpf_grad_corr2d();
}

void mfpf_grad_corr2d_builder::set_kernel_size(unsigned ni, unsigned nj,
                                               double ref_x, double ref_y)
{
  ni_=ni; nj_=nj;
  ref_x_=ref_x;
  ref_y_=ref_y;
}

void mfpf_grad_corr2d_builder::set_kernel_size(unsigned ni, unsigned nj)
{
  ni_=ni; nj_=nj;
  ref_x_=0.5*(ni_-1);
  ref_y_=0.5*(nj_-1);
}

//: Define region size in world co-ordinates
//  Sets up ROI to cover given box (with samples at step_size()),
//  with ref point at centre.
void mfpf_grad_corr2d_builder::set_region_size(double wi, double wj)
{
  wi/=step_size();
  wj/=step_size();
  int ni = std::max(1,int(0.99+wi));
  int nj = std::max(1,int(0.99+wj));
  set_kernel_size(unsigned(ni),unsigned(nj));
}

//: Number of dimensions in the model
unsigned mfpf_grad_corr2d_builder::model_dim()
{
  return 2*ni_*nj_;
}


//: Initialise building
// Must be called before any calls to add_example(...)
void mfpf_grad_corr2d_builder::clear(unsigned  /*n_egs*/)
{
  n_added_=0;
}

static void normalize(vil_image_view<double>& im)
{
  unsigned ni=im.ni(),nj=im.nj();
  double sum=0.0,ss=0.0;
  for (unsigned j=0;j<nj;++j)
    for (unsigned i=0;i<ni;++i)
    {
      sum+=im(i,j); ss+=im(i,j)*im(i,j);
    }

  assert(!vnl_math::isnan(sum));

  if (ss<1e-6)
  {
    std::cerr<<"Warning: Almost flat region in mfpf_grad_corr2d_builder\n"
            <<"         Size: "<<ni<<" x "<<nj<<std::endl;
  }

  // Normalise so that im has zero mean and unit sum of squares.
  double mean=sum/(ni*nj);
  ss-=(mean*mean*ni*nj);
  double s=1.0;
  if (ss>0) s = std::sqrt(1.0/ss);
  vil_math_scale_and_offset_values(im,s,-s*mean);
}

void mfpf_grad_corr2d_builder::diff_image(const vimt_image_2d_of<float>& image,
                                          const vgl_point_2d<double>& p,
                                          const vgl_vector_2d<double>& u,
                                          vil_image_view<double>& grad_x,
                                          vil_image_view<double>& grad_y)
{
  assert(image.image().size()>0);

  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());

  const vimt_transform_2d& s_w2i = image.world2im();
  vgl_point_2d<double> p0, im_p0;
  vgl_vector_2d<double> im_u, im_v;

  vil_image_view<float> sample;

  // get raw image data first
  p0 = p-(ref_x_+0.5)*u1-ref_y_*v1;
  im_p0 = s_w2i(p0);
  im_u = s_w2i.delta(p0, u1);
  im_v = s_w2i.delta(p0, v1);
  vil_resample_bilin(image.image(),sample, im_p0.x(),im_p0.y(),
                     im_u.x(),im_u.y(), im_v.x(),im_v.y(), ni_+1,nj_);

  // take differences across x
  grad_x.set_size(ni_,nj_);

  const float* s1 = sample.top_left_ptr();
  const float* s2 = sample.top_left_ptr()+sample.istep();
  std::ptrdiff_t s_jstep = sample.jstep();

  double* kx = grad_x.top_left_ptr();
  std::ptrdiff_t kx_jstep = grad_x.jstep();

  for (unsigned j=0;j<nj_;++j,kx+=kx_jstep,s1+=s_jstep,s2+=s_jstep)
    for (unsigned i=0;i<ni_;++i)
      *(kx+i) = *(s2+i) - *(s1+i);


  // get raw image data first
  p0 = p-ref_x_*u1-(ref_y_+0.5)*v1;
  im_p0 = s_w2i(p0);
  im_u = s_w2i.delta(p0, u1);
  im_v = s_w2i.delta(p0, v1);
  vil_resample_bilin(image.image(),sample, im_p0.x(),im_p0.y(),
                     im_u.x(),im_u.y(), im_v.x(),im_v.y(), ni_,nj_+1);

  // take differences across y
  grad_y.set_size(ni_,nj_);

  s1 = sample.top_left_ptr();
  s2 = sample.top_left_ptr()+sample.jstep();
  s_jstep = sample.jstep();

  double* ky = grad_y.top_left_ptr();
  std::ptrdiff_t ky_jstep = grad_y.jstep();

  for (unsigned j=0;j<nj_;++j,ky+=ky_jstep,s1+=s_jstep,s2+=s_jstep)
    for (unsigned i=0;i<ni_;++i)
      *(ky+i) = *(s2+i) - *(s1+i);
}

//: Add one example to the model
void mfpf_grad_corr2d_builder::add_one_example(const vimt_image_2d_of<float>& image,
                                               const vgl_point_2d<double>& p,
                                               const vgl_vector_2d<double>& u)
{
  assert(image.image().size()>0);

  vil_image_view<double> grad_x, grad_y;
  diff_image(image,p,u,grad_x,grad_y);
  normalize(grad_x);
  normalize(grad_y);

  if (n_added_==0)
  {
    sum_x_.deep_copy(grad_x);
    sum_y_.deep_copy(grad_y);
  }
  else
  {
    vil_math_add_image_fraction(sum_x_,1.0,grad_x,1.0);
    vil_math_add_image_fraction(sum_y_,1.0,grad_y,1.0);
  }
  n_added_++;
}

//: Get sample of region around specified point in image
void mfpf_grad_corr2d_builder::get_sample_vector(const vimt_image_2d_of<float>& image,
                                                 const vgl_point_2d<double>& p,
                                                 const vgl_vector_2d<double>& u,
                                                 std::vector<double>& v)
{
  assert(image.image().size()>0);

  vil_image_view<double> grad_x, grad_y;
  diff_image(image,p,u,grad_x,grad_y);

  v.resize( model_dim() );
  unsigned v_ind=0;

  for (unsigned j=0;j<grad_x.nj();j++)
    for (unsigned i=0;i<grad_x.ni();i++,v_ind++)
      v[v_ind] = grad_x(i,j);

  for (unsigned j=0;j<grad_y.nj();j++)
    for (unsigned i=0;i<grad_y.ni();i++,v_ind++)
      v[v_ind] = grad_y(i,j);
}

//: Add one example to the model
void mfpf_grad_corr2d_builder::add_example(const vimt_image_2d_of<float>& image,
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
void mfpf_grad_corr2d_builder::build(mfpf_point_finder& pf)
{
  assert(n_added_>0);
  assert(pf.is_a()=="mfpf_grad_corr2d");
  auto& nc = static_cast<mfpf_grad_corr2d&>(pf);

  vil_image_view<double> mean_x;
  mean_x.deep_copy(sum_x_);
  vil_math_scale_values(mean_x,1.0/n_added_);
  normalize(mean_x);

  vil_image_view<double> mean_y;
  mean_y.deep_copy(sum_y_);
  vil_math_scale_values(mean_y,1.0/n_added_);
  normalize(mean_y);

  nc.set(mean_x,mean_y,ref_x_,ref_y_);
  set_base_parameters(nc);
  nc.set_overlap_f(overlap_f_);
}

//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mfpf_grad_corr2d_builder::set_from_stream(std::istream &is)
{
  // Cycle through string and produce a map of properties
  std::string s = mbl_parse_block(is);
  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  set_defaults();

  // Extract the properties
  parse_base_props(props);

  if (props.find("ni")!=props.end())
  {
    ni_=vul_string_atoi(props["ni"]);
    props.erase("ni");
  }
  if (props.find("nj")!=props.end())
  {
    nj_=vul_string_atoi(props["nj"]);
    props.erase("nj");
  }

  overlap_f_=vul_string_atof(props.get_optional_property("overlap_f", "1.0"));

  if (props.find("ref_x")!=props.end())
  {
    ref_x_=vul_string_atof(props["ref_x"]);
    props.erase("ref_x");
  }
  else ref_x_=0.5*(ni_-1.0);

  if (props.find("ref_y")!=props.end())
  {
    ref_y_=vul_string_atof(props["ref_y"]);
    props.erase("ref_y");
  }
  else ref_y_=0.5*(nj_-1.0);

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
      "mfpf_grad_corr2d_builder::set_from_stream", props, mbl_read_props_type());
  return true;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_grad_corr2d_builder::is_a() const
{
  return std::string("mfpf_grad_corr2d_builder");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder_builder* mfpf_grad_corr2d_builder::clone() const
{
  return new mfpf_grad_corr2d_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_grad_corr2d_builder::print_summary(std::ostream& os) const
{
  os << "{ size: " << ni_ << 'x' << nj_
     << " nA: " << nA_ << " dA: " << dA_ <<' ';
  mfpf_point_finder_builder::print_summary(os);
  os <<" overlap_f: "<<overlap_f_
     << " }";
}

//: Version number for I/O
short mfpf_grad_corr2d_builder::version_no() const
{
  return 2;
}

void mfpf_grad_corr2d_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  mfpf_point_finder_builder::b_write(bfs);  // Save base class
  vsl_b_write(bfs,ni_);
  vsl_b_write(bfs,nj_);
  vsl_b_write(bfs,ref_x_);
  vsl_b_write(bfs,ref_y_);
  vsl_b_write(bfs,nA_);
  vsl_b_write(bfs,dA_);
  vsl_b_write(bfs,sum_x_);
  vsl_b_write(bfs,sum_y_);
  vsl_b_write(bfs,n_added_);
  vsl_b_write(bfs,overlap_f_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_grad_corr2d_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
    case (2):
      mfpf_point_finder_builder::b_read(bfs);  // Load base class
      vsl_b_read(bfs,ni_);
      vsl_b_read(bfs,nj_);
      vsl_b_read(bfs,ref_x_);
      vsl_b_read(bfs,ref_y_);
      vsl_b_read(bfs,nA_);
      vsl_b_read(bfs,dA_);
      vsl_b_read(bfs,sum_x_);
      vsl_b_read(bfs,sum_y_);
      vsl_b_read(bfs,n_added_);
      if (version==1) overlap_f_=1.0;
      else            vsl_b_read(bfs,overlap_f_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
