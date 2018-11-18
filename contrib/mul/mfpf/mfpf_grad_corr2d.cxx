#include <cmath>
#include <iostream>
#include <algorithm>
#include "mfpf_grad_corr2d.h"
//:
// \file
// \brief Searches over a grid using normalised correlation
// \author Tim Cootes

#include <vsl/vsl_binary_loader.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vil/vil_resample_bilin.h>
#include <vil/io/vil_io_image_view.h>
#include <vil/vil_math.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

#include <vil/vil_convert.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_grad_corr2d::mfpf_grad_corr2d()
{
  set_defaults();
}

//: Define default values
void mfpf_grad_corr2d::set_defaults()
{
  step_size_=1.0;
  search_ni_=5;
  search_nj_=5;
  nA_=0; dA_=0.0;
  ns_=0; ds_=1.0;
  ref_x_ = 0.0;
  ref_y_ = 0.0;
  overlap_f_=1.0;
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_grad_corr2d::~mfpf_grad_corr2d() = default;

//: Define filter kernel to search with
void mfpf_grad_corr2d::set(const vil_image_view<double>& kx,
                           const vil_image_view<double>& ky,
                           double ref_x, double ref_y)
{
  // Copy, ensuring istep==1
  unsigned ni,nj;
  ni=kx.ni(); nj=kx.nj();
  kernel_x_.set_size(ni,nj);
  double sum_x=0.0,ss_x=0.0;
  for (unsigned j=0;j<nj;++j)
    for (unsigned i=0;i<ni;++i)
    {
      kernel_x_(i,j)=kx(i,j);
      sum_x+=kx(i,j); ss_x+=kx(i,j)*kx(i,j);
    }

  assert(!vnl_math::isnan(sum_x));

  // Normalise so that kernel_ has zero mean and unit sum of squares.
  double mean_x=sum_x/(ni*nj);
  ss_x-=(mean_x*mean_x*ni*nj);
  assert(ss_x>1e-6);  // If near zero, flat region - can't use correlation
  double s_x=1.0;
  if (ss_x>0) s_x = std::sqrt(1.0/ss_x);
  vil_math_scale_and_offset_values(kernel_x_,s_x,-s_x*mean_x);

  // repeat for y-image
  ni=ky.ni(); nj=ky.nj();
  kernel_y_.set_size(ni,nj);
  double sum_y=0.0,ss_y=0.0;
  for (unsigned j=0;j<nj;++j)
    for (unsigned i=0;i<ni;++i)
    {
      kernel_y_(i,j)=ky(i,j);
      sum_y+=ky(i,j); ss_y+=ky(i,j)*ky(i,j);
    }

  assert(!vnl_math::isnan(sum_y));

  // Normalise so that kernel_ has zero mean and unit sum of squares.
  double mean_y=sum_y/(ni*nj);
  ss_y-=(mean_y*mean_y*ni*nj);
  assert(ss_y>1e-6);  // If near zero, flat region - can't use correlation
  double s_y=1.0;
  if (ss_y>0) s_y = std::sqrt(1.0/ss_y);
  vil_math_scale_and_offset_values(kernel_y_,s_y,-s_y*mean_y);

  ref_x_ = ref_x;
  ref_y_ = ref_y;
}

//: Define filter kernel to search with
void mfpf_grad_corr2d::set(const vil_image_view<double>& kx,
                           const vil_image_view<double>& ky)
{
  set(kx, ky, 0.5*(kx.ni()-1.0), 0.5*(kx.nj()-1.0));
}

//: Define filter kernel to search with, expressed as a vector
bool mfpf_grad_corr2d::set_model(const std::vector<double>& v)
{
  // we assume that nplanes()==1
  assert( v.size() == model_dim() );

  // copy vector values into im_x, column by column
  vil_image_view<double> im_x( kernel_x_.ni(), kernel_x_.nj() );
  unsigned vec_ind=0;
  for (unsigned j=0;j<kernel_x_.nj();++j)
    for (unsigned i=0;i<kernel_x_.ni();++i,++vec_ind)
      im_x(i,j)=v[vec_ind];

  // copy vector values into im_y, column by column
  vil_image_view<double> im_y( kernel_y_.ni(), kernel_y_.nj() );
  for (unsigned j=0;j<kernel_y_.nj();++j)
    for (unsigned i=0;i<kernel_y_.nj();++i,++vec_ind)
      im_y(i,j)=v[vec_ind];

  // set kernel from created image
  set(im_x,im_y);

  return true;
}

//: Number of dimensions in the model
unsigned mfpf_grad_corr2d::model_dim()
{
  return kernel_x_.ni()*kernel_x_.nj() + kernel_y_.ni()*kernel_y_.nj();
}

//: Filter kernel to search with, expressed as a vector
void mfpf_grad_corr2d::get_kernel_vector(std::vector<double>& v) const
{
  // we assume that nplanes()==1
  v.resize( 2*kernel_x_.ni()*kernel_x_.nj() );

  // copy kernel values into v, column by column
  unsigned vec_ind=0;
  for (unsigned j=0;j<kernel_x_.nj();++j)
    for (unsigned i=0;i<kernel_x_.ni();++i,++vec_ind)
      v[vec_ind]=kernel_x_(i,j);

  for (unsigned j=0;j<kernel_y_.nj();++j)
    for (unsigned i=0;i<kernel_y_.ni();++i,++vec_ind)
      v[vec_ind]=kernel_y_(i,j);
}

//: Relative size of region used for estimating overlap
//  If 0.5, then overlap requires pt inside central 50% of region.
void mfpf_grad_corr2d::set_overlap_f(double f)
{
  overlap_f_=f;
}

// Assumes im2[i] has zero mean and unit length as a vector
// Assumes element (i,j) is im1[i+j*jstep1] etc
inline double norm_corr(const double* im1, const double* im2,
                        std::ptrdiff_t jstep1, std::ptrdiff_t jstep2,
                        unsigned ni, unsigned nj)
{
  double sum1=0.0,sum2=0.0,sum_sq=0.0;
  for (unsigned j=0;j<nj;++j,im1+=jstep1,im2+=jstep2)
    for (unsigned i=0;i<ni;++i)
    {
      sum1+=im1[i]*im2[i];
      sum2+=im1[i];
      sum_sq+=im1[i]*im1[i];
    }
  unsigned n=ni*nj;
  double mean = sum2/n;
  double ss = std::max(1e-6,sum_sq-n*mean*mean);
  double s = std::sqrt(ss);

  return sum1/s;
}

#if 0 // static (=local) function not used in this file
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
#endif // 0

void mfpf_grad_corr2d::diff_image(const vimt_image_2d_of<float>& image,
                                  const vgl_point_2d<double>& p,
                                  const vgl_vector_2d<double>& u,
                                  vil_image_view<double>& grad_x,
                                  vil_image_view<double>& grad_y,
                                  int search_ni, int search_nj)
{
  assert(image.image().size()>0);

  unsigned ni=kernel_x_.ni();
  unsigned nj=kernel_x_.nj();
  unsigned nsi = ni + 2*search_ni;
  unsigned nsj = nj + 2*search_nj;

  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());

  const vimt_transform_2d& s_w2i = image.world2im();
  vgl_point_2d<double> p0, im_p0;
  vgl_vector_2d<double> im_u, im_v;

  vil_image_view<float> sample;


  // get raw image data first
  p0 = p - (ref_x_+search_ni+0.5)*u1 - (ref_y_+search_nj)*v1;
  im_p0 = s_w2i(p0);
  im_u = s_w2i.delta(p0, u1);
  im_v = s_w2i.delta(p0, v1);
  vil_resample_bilin(image.image(),sample, im_p0.x(),im_p0.y(),
                     im_u.x(),im_u.y(), im_v.x(),im_v.y(), nsi+1,nsj);

  // take differences across x
  grad_x.set_size( nsi,nsj );

  const float* s1 = sample.top_left_ptr();
  const float* s2 = sample.top_left_ptr()+sample.istep();
  std::ptrdiff_t s_jstep = sample.jstep();

  double* kx = grad_x.top_left_ptr();
  std::ptrdiff_t kx_jstep = grad_x.jstep();

  for (unsigned j=0;j<nsj;++j,kx+=kx_jstep,s1+=s_jstep,s2+=s_jstep)
    for (unsigned i=0;i<nsi;++i)
      *(kx+i) = *(s2+i) - *(s1+i);


  // get raw image data first
  p0 = p - (ref_x_+search_ni)*u1 - (ref_y_+search_nj+0.5)*v1;
  im_p0 = s_w2i(p0);
  im_u = s_w2i.delta(p0, u1);
  im_v = s_w2i.delta(p0, v1);
  vil_resample_bilin(image.image(),sample, im_p0.x(),im_p0.y(),
                     im_u.x(),im_u.y(), im_v.x(),im_v.y(), nsi,nsj+1);

  // take differences across y
  grad_y.set_size(nsi,nsj);

  s1 = sample.top_left_ptr();
  s2 = sample.top_left_ptr()+sample.jstep();
  s_jstep = sample.jstep();

  double* ky = grad_y.top_left_ptr();
  std::ptrdiff_t ky_jstep = grad_y.jstep();

  for (unsigned j=0;j<nsj;++j,ky+=ky_jstep,s1+=s_jstep,s2+=s_jstep)
    for (unsigned i=0;i<nsi;++i)
      *(ky+i) = *(s2+i) - *(s1+i);
}

//: Get sample of region around specified point in image
void mfpf_grad_corr2d::get_sample_vector(const vimt_image_2d_of<float>& image,
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

//: Radius of circle containing modelled region
double mfpf_grad_corr2d::radius() const
{
  // Compute distance to each corner
  double wx = kernel_x_.ni()-1;
  double x2 = std::max(ref_x_*ref_x_,(ref_x_-wx)*(ref_x_-wx));
  double wy = kernel_x_.nj()-1;
  double y2 = std::max(ref_y_*ref_y_,(ref_y_-wy)*(ref_y_-wy));
  double r2 = x2+y2;
  if (r2<=1) return 1.0;
  return std::sqrt(r2);
}

//: Evaluate match at p, using u to define scale and orientation
// Returns -1*edge strength at p along direction u
double mfpf_grad_corr2d::evaluate(const vimt_image_2d_of<float>& image,
                                  const vgl_point_2d<double>& p,
                                  const vgl_vector_2d<double>& u)
{
  vil_image_view<double> grad_x, grad_y;
  diff_image(image,p,u,grad_x,grad_y);

  return 2.0-norm_corr(grad_x.top_left_ptr(),kernel_x_.top_left_ptr(),
                       grad_x.jstep(),kernel_x_.jstep(),
                       kernel_x_.ni(),kernel_x_.nj())
            -norm_corr(grad_y.top_left_ptr(),kernel_y_.top_left_ptr(),
                       grad_y.jstep(),kernel_y_.jstep(),
                       kernel_y_.ni(),kernel_y_.nj());
}

//: Evaluate match at in a region around p
// Returns a quality of fit at a set of positions.
// response image (whose size and transform is set inside the
// function), indicates the points at which the function was
// evaluated.  response(i,j) is the fit at the point
// response.world2im().inverse()(i,j).  The world2im() transformation
// may be affine.
void mfpf_grad_corr2d::evaluate_region(
                        const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vimt_image_2d_of<double>& response)
{
  assert(image.image().nplanes()==1);

  vil_image_view<double> grad_x, grad_y;
  diff_image(image,p,u,grad_x,grad_y,
             search_ni_,search_nj_);

  int ni=1+2*search_ni_;
  int nj=1+2*search_nj_;
  response.image().set_size(ni,nj);

  double* r = response.image().top_left_ptr();
  const double* kx = kernel_x_.top_left_ptr();
  const double* ky = kernel_y_.top_left_ptr();
  const double* sx = grad_x.top_left_ptr();
  const double* sy = grad_y.top_left_ptr();
  std::ptrdiff_t r_jstep = response.image().jstep();
  std::ptrdiff_t sx_jstep = grad_x.jstep();
  std::ptrdiff_t sy_jstep = grad_y.jstep();
  std::ptrdiff_t kx_jstep = kernel_x_.jstep();
  std::ptrdiff_t ky_jstep = kernel_y_.jstep();

  for (int j=0;j<nj;++j,r+=r_jstep,sx+=sx_jstep,sy+=sy_jstep)
  {
    for (int i=0;i<ni;++i)
    {
      r[i] = 2.0-norm_corr(sx+i,kx,sx_jstep,kx_jstep,
                           kernel_x_.ni(),kernel_x_.nj())
                -norm_corr(sy+i,ky,sy_jstep,ky_jstep,
                           kernel_y_.ni(),kernel_y_.nj());
    }
  }

  // Set up transformation parameters

  // Point (i,j) in dest corresponds to p1+i.u+j.v,
  // an affine transformation for image to world
  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());
  const vgl_point_2d<double> p1 = p-search_ni_*u1-search_nj_*v1;

  vimt_transform_2d i2w;
  i2w.set_similarity(vgl_point_2d<double>(u1.x(),u1.y()),p1);
  response.set_world2im(i2w.inverse());
}

//: Search given image around p, using u to define scale and angle
//  On exit, new_p defines position of the best nearby match.
//  Returns a quality of fit measure at that
//  point (the smaller the better).
double mfpf_grad_corr2d::search_one_pose(
                        const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p)
{
  assert(image.image().nplanes()==1);

  vil_image_view<double> grad_x, grad_y;
  diff_image(image,p,u,grad_x,grad_y,
             search_ni_,search_nj_);

  const double* kx = kernel_x_.top_left_ptr();
  const double* ky = kernel_y_.top_left_ptr();
  const double* sx = grad_x.top_left_ptr();
  const double* sy = grad_y.top_left_ptr();
  std::ptrdiff_t sx_jstep = grad_x.jstep();
  std::ptrdiff_t sy_jstep = grad_y.jstep();
  std::ptrdiff_t kx_jstep = kernel_x_.jstep();
  std::ptrdiff_t ky_jstep = kernel_y_.jstep();

  int ni=1+2*search_ni_;
  int nj=1+2*search_nj_;

  double best_r=-9e99;
  int best_i=-1,best_j=-1;
  for (int j=0;j<nj;++j,sx+=sx_jstep,sy+=sy_jstep)
  {
    for (int i=0;i<ni;++i)
    {
      double r = norm_corr(sx+i,kx,sx_jstep,kx_jstep,
                           kernel_x_.ni(),kernel_x_.nj())
               + norm_corr(sy+i,ky,sy_jstep,ky_jstep,
                           kernel_y_.ni(),kernel_y_.nj());
      if (r>best_r) { best_r=r; best_i=i; best_j=j; }
    }
  }

  // Compute position of best point
  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());
  new_p = p+(best_i-search_ni_)*u1+(best_j-search_nj_)*v1;
  return 2.0 - best_r;
}

// Returns true if p is inside region at given pose
bool mfpf_grad_corr2d::is_inside(const mfpf_pose& pose,
                                 const vgl_point_2d<double>& p,
                                 double f) const
{
  // Set transform model frame -> World
  vimt_transform_2d t1;
  t1.set_similarity(step_size()*pose.u(),pose.p());
  // Compute position of p in model frame
  vgl_point_2d<double> q=t1.inverse()(p);
  q.x()/=f; q.y()/=f;  // To check that q in the central fraction f
  q.x()+=ref_x_;
  if (q.x()<0 || q.x()>(kernel_x_.ni()-1)) return false;
  q.y()+=ref_y_;
  if (q.y()<0 || q.y()>(kernel_x_.nj()-1)) return false;
  return true;
}

//: Return true if modelled regions at pose1 and pose2 overlap
//  Checks if reference point of one is inside region of other
bool mfpf_grad_corr2d::overlap(const mfpf_pose& pose1,
                               const mfpf_pose& pose2) const
{
  if (is_inside(pose1,pose2.p(),overlap_f_)) return true;
  if (is_inside(pose2,pose1.p(),overlap_f_)) return true;
  return false;
}

//: Generate points in ref frame that represent boundary
//  Points of a contour around the shape.
//  Used for display purposes.
void mfpf_grad_corr2d::get_outline(std::vector<vgl_point_2d<double> >& pts) const
{
  pts.resize(7);
  int roi_ni=kernel_x_.ni();
  int roi_nj=kernel_x_.nj();
  vgl_vector_2d<double> r(ref_x_,ref_y_);
  pts[0]=vgl_point_2d<double>(0,roi_nj)-r;
  pts[1]=vgl_point_2d<double>(0,0);
  pts[2]=vgl_point_2d<double>(roi_ni,roi_nj)-r;
  pts[3]=vgl_point_2d<double>(0,roi_nj)-r;
  pts[4]=vgl_point_2d<double>(0,0)-r;
  pts[5]=vgl_point_2d<double>(roi_ni,0)-r;
  pts[6]=vgl_point_2d<double>(roi_ni,roi_nj)-r;
}

//: Return an image of the kernel
void mfpf_grad_corr2d::get_image_of_model(vimt_image_2d_of<vxl_byte>& image) const
{
  // compute magnitude of image gradient
  vil_image_view<double> kx_sqr,ky_sqr,kernel;
  vil_math_sum_sqr(kernel_x_,kx_sqr);
  vil_math_sum_sqr(kernel_y_,ky_sqr);
  vil_math_image_sum(kx_sqr,ky_sqr,kernel);
  vil_math_sqrt(kernel);

  vil_convert_stretch_range(kernel,image.image());
  vimt_transform_2d ref2im;
  ref2im.set_zoom_only(1.0/step_size_,ref_x_,ref_y_);
  image.set_world2im(ref2im);
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_grad_corr2d::is_a() const
{
  return std::string("mfpf_grad_corr2d");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder* mfpf_grad_corr2d::clone() const
{
  return new mfpf_grad_corr2d(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_grad_corr2d::print_summary(std::ostream& os) const
{
  os << "{  size: " << kernel_x_.ni() << " x " << kernel_x_.nj();
  mfpf_point_finder::print_summary(os);
  os << " overlap_f: " << overlap_f_ << " }";
}

void mfpf_grad_corr2d::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  mfpf_point_finder::b_write(bfs);  // Save base class
  vsl_b_write(bfs,kernel_x_);
  vsl_b_write(bfs,kernel_y_);
  vsl_b_write(bfs,ref_x_);
  vsl_b_write(bfs,ref_y_);
  vsl_b_write(bfs,overlap_f_);
}

short mfpf_grad_corr2d::version_no() const
{
  return 2;
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_grad_corr2d::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
    case (2):
      mfpf_point_finder::b_read(bfs);  // Load in base class
      vsl_b_read(bfs,kernel_x_);
      vsl_b_read(bfs,kernel_y_);
      vsl_b_read(bfs,ref_x_);
      vsl_b_read(bfs,ref_y_);
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

//: Test equality
bool mfpf_grad_corr2d::operator==(const mfpf_grad_corr2d& nc) const
{
  if (!base_equality(nc)) return false;
  if (kernel_x_.ni()!=nc.kernel_x_.ni()) return false;
  if (kernel_x_.nj()!=nc.kernel_x_.nj()) return false;
  if (kernel_y_.ni()!=nc.kernel_y_.ni()) return false;
  if (kernel_y_.nj()!=nc.kernel_y_.nj()) return false;
  if (std::fabs(ref_x_-nc.ref_x_)>1e-6) return false;
  if (std::fabs(ref_y_-nc.ref_y_)>1e-6) return false;
  if (kernel_x_.size()!=nc.kernel_x_.size()) return false;
  if (kernel_x_.size()==0) return true;  // ssd fails on empty
  if (kernel_y_.size()!=nc.kernel_y_.size()) return false;
  if (kernel_y_.size()==0) return true;  // ssd fails on empty
  if (vil_math_ssd(kernel_x_,nc.kernel_x_,double(0))>1e-4) return false;
  return (vil_math_ssd(kernel_y_,nc.kernel_y_,double(0))<1e-4);
}
