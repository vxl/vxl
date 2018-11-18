#include <iostream>
#include <cmath>
#include "mfpf_pose_predictor.h"
//:
// \file
// \brief Uses regression to predict new pose from current sample.
// \author Tim Cootes

#include <mfpf/mfpf_sample_region.h>
#include <mfpf/mfpf_norm_vec.h>
#include <vil/vil_resample_bilin.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_indent.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vnl/vnl_vector.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_pose_predictor::mfpf_pose_predictor()
{
  set_defaults();
}

//: Define default values
void mfpf_pose_predictor::set_defaults()
{
  step_size_=1.0;
  pose_type_=translation;
  n_pixels_=0;
  roi_.resize(0);
  roi_ni_=0;
  roi_nj_=0;
  ref_x_=0;
  ref_y_=0;
  norm_method_=1;
  var_min_ = 1.0E-6;
}

//: Size of step between sample points
void mfpf_pose_predictor::set_step_size(double s)
{
  step_size_=s;
}

//: Initialise as a rectangle with ref. in centre.
void mfpf_pose_predictor::set_as_box(unsigned ni, unsigned nj,
                                     short norm_method)
{
  // Set ROI to be a box
  std::vector<mbl_chord> roi(nj);
  for (unsigned j=0;j<nj;++j) roi[j]=mbl_chord(0,ni-1,j);

  set(roi,0.5*(ni-1),0.5*(nj-1),norm_method);
}

//: Define model region as an ellipse with radii ri, rj
//  Ref. point in centre.
void mfpf_pose_predictor::set_as_ellipse(double ri, double rj,
                                         short norm_method)
{
  std::vector<mbl_chord> roi;

  int ni=int(ri+1e-6);
  int nj=int(rj+1e-6);
  for (int j = -nj;j<=nj;++j)
  {
    // Find start and end of line of pixels inside disk
    int x = int(ri*std::sqrt(1.0-j*j/(rj*rj)));
    roi.emplace_back(ni-x,ni+x,nj+j);
  }

  set(roi,ni,nj,norm_method);
}

void mfpf_pose_predictor::set_pose_type(const mfpf_pose_type& pt)
{
  pose_type_=pt;
}
//=======================================================================
// Destructor
//=======================================================================

mfpf_pose_predictor::~mfpf_pose_predictor() = default;

//: Define region and cost of region
void mfpf_pose_predictor::set(const std::vector<mbl_chord>& roi,
                              double ref_x, double ref_y,
                              short norm_method)
{
  ref_x_ = ref_x;
  ref_y_ = ref_y;

  // Check bounding box
  if (roi.size()==0) { roi_ni_=0; roi_nj_=0; return; }
  int ilo=roi[0].start_x(), ihi=roi[0].end_x();
  int jlo=roi[0].y(), jhi=roi[0].y();

  for (unsigned k=1;k<roi.size();++k)
  {
    if (roi[k].start_x()<ilo) ilo=roi[k].start_x();
    if (roi[k].end_x()>ihi)   ihi=roi[k].end_x();
    if (roi[k].y()<jlo) jlo=roi[k].y();
    if (roi[k].y()>jhi) jhi=roi[k].y();
  }
  roi_ni_=1+ihi-ilo;
  roi_nj_=1+jhi-jlo;

  // Apply offset of (-ilo,-jlo) to ensure bounding box is +ive
  ref_x_-=ilo; ref_y_-=jlo;
  roi_.resize(roi.size());
  n_pixels_=0;
  for (unsigned k=0;k<roi.size();++k)
  {
    roi_[k]= mbl_chord(roi[k].start_x()-ilo,
                       roi[k].end_x()-ilo,   roi[k].y()-jlo);
    n_pixels_+=1+roi[k].end_x()-roi[k].start_x();
  }

  assert(norm_method>=0 && norm_method<=1);
  norm_method_ = norm_method;
}

//: Set regression matrices
void mfpf_pose_predictor::set_predictor(const vnl_matrix<double>& R,
                                        const vnl_vector<double>& dp0)
{
  assert(R.rows()==dp0.size());
  R_=R;
  dp0_=dp0;
}

//: Radius of circle containing modelled region
double mfpf_pose_predictor::radius() const
{
  // Compute distance to each corner
  double wx = roi_ni_-1;
  double x2 = std::max(ref_x_*ref_x_,(ref_x_-wx)*(ref_x_-wx));
  double wy = roi_nj_-1;
  double y2 = std::max(ref_y_*ref_y_,(ref_y_-wy)*(ref_y_-wy));
  double r2 = x2+y2;
  if (r2<=1) return 1.0;
  return std::sqrt(r2);
}


//: Get sample of region around specified point in image
void mfpf_pose_predictor::get_sample_vector(const vimt_image_2d_of<float>& image,
                                            const vgl_point_2d<double>& p,
                                            const vgl_vector_2d<double>& u,
                                            vnl_vector<double>& vec)
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
                     im_p0.x(),im_p0.y(),
                     im_u.x(),im_u.y(),
                     im_v.x(),im_v.y(),
                     roi_ni_,roi_nj_);

  vec.set_size(n_pixels_*sample.nplanes());
  mfpf_sample_region(sample.top_left_ptr(),sample.jstep(),
                     np,roi_,vec);

  if (norm_method_==1) mfpf_norm_vec(vec,var_min_);
}

//: Sample at pose0 and predict a better pose (new_pose)
void mfpf_pose_predictor::new_pose(const vimt_image_2d_of<float>& image,
                                   const mfpf_pose& pose0,
                                   mfpf_pose& new_pose)
{
  vnl_vector<double> v;
  get_sample_vector(image,pose0.p(),pose0.u(),v);
  vnl_vector<double> dp = R_*v+dp0_;

  // Compute pose to update pose0
  mfpf_pose dpose;
  double s;

  switch (pose_type_)
  {
    case translation:
      dpose.p().set(dp[0],dp[1]);
      break;
    case rigid:
      dpose.p().set(dp[0],dp[1]);
      dpose.u().set(std::cos(dp[2]),std::sin(dp[2]));
      break;
    case zoom:
      dpose.p().set(dp[0],dp[1]);
      dpose.u().set(std::exp(dp[2]),0);
      break;
    case similarity:
      dpose.p().set(dp[0],dp[1]);
      s=std::exp(dp[2]);
      dpose.u().set(s*std::cos(dp[3]),s*std::sin(dp[3]));
      break;
    default: assert(!"Invalid pose_type_"); break;
  }

  new_pose=pose0*dpose.inverse(); // i.e., apply inverse of dpose, then pose0
}


//: Generate points in ref frame that represent boundary
//  Points of a contour around the shape.
//  Used for display purposes.
void mfpf_pose_predictor::get_outline(std::vector<vgl_point_2d<double> >& pts) const
{
  pts.resize(7);
  vgl_vector_2d<double> r(ref_x_,ref_y_);
  pts[0]=vgl_point_2d<double>(0,roi_nj_)-r;
  pts[1]=vgl_point_2d<double>(0,0);
  pts[2]=vgl_point_2d<double>(roi_ni_,roi_nj_)-r;
  pts[3]=vgl_point_2d<double>(0,roi_nj_)-r;
  pts[4]=vgl_point_2d<double>(0,0)-r;
  pts[5]=vgl_point_2d<double>(roi_ni_,0)-r;
  pts[6]=vgl_point_2d<double>(roi_ni_,roi_nj_)-r;
}


//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_pose_predictor::is_a() const
{
  return std::string("mfpf_pose_predictor");
}

//: Create a copy on the heap and return base class pointer
mfpf_pose_predictor* mfpf_pose_predictor::clone() const
{
  return new mfpf_pose_predictor(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_pose_predictor::print_summary(std::ostream& os) const
{
  os << "{  step_size: "<<step_size_
     <<" pose_type: "<<pose_type_
     <<" size: "<<roi_ni_<<" x "<<roi_nj_
     << " n_pixels: "<<n_pixels_
     << " ref_pt: ("<<ref_x_<<','<<ref_y_<<')'<<'\n';
  vsl_indent_inc(os);
  if (norm_method_==0) os<<vsl_indent()<<"norm: none ";
  else                 os<<vsl_indent()<<"norm: linear ";
  os << "n_params: "<<R_.rows()<<'\n';
  vsl_indent_dec(os);
  os<<vsl_indent()<<'}';
}

void mfpf_pose_predictor::print_shape(std::ostream& os) const
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

short mfpf_pose_predictor::version_no() const
{
  return 1;
}


void mfpf_pose_predictor::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,step_size_);
  vsl_b_write(bfs,unsigned(pose_type_));
  vsl_b_write(bfs,roi_);
  vsl_b_write(bfs,roi_ni_);
  vsl_b_write(bfs,roi_nj_);
  vsl_b_write(bfs,n_pixels_);
  vsl_b_write(bfs,ref_x_);
  vsl_b_write(bfs,ref_y_);
  vsl_b_write(bfs,norm_method_);
  vsl_b_write(bfs,R_);
  vsl_b_write(bfs,dp0_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_pose_predictor::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  unsigned i;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case 1:
      vsl_b_read(bfs,step_size_);
      vsl_b_read(bfs,i); pose_type_=mfpf_pose_type(i);
      vsl_b_read(bfs,roi_);
      vsl_b_read(bfs,roi_ni_);
      vsl_b_read(bfs,roi_nj_);
      vsl_b_read(bfs,n_pixels_);
      vsl_b_read(bfs,ref_x_);
      vsl_b_read(bfs,ref_y_);
      vsl_b_read(bfs,norm_method_);
      vsl_b_read(bfs,R_);
      vsl_b_read(bfs,dp0_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Test equality
bool mfpf_pose_predictor::operator==(const mfpf_pose_predictor& nc) const
{
  if (std::fabs(step_size_-nc.step_size_)>1e-6) return false;
  if (pose_type_!=nc.pose_type_) return false;
  if (roi_ni_!=nc.roi_ni_) return false;
  if (roi_nj_!=nc.roi_nj_) return false;
  if (norm_method_!=nc.norm_method_) return false;
  if (n_pixels_!=nc.n_pixels_) return false;
  if (std::fabs(ref_x_-nc.ref_x_)>1e-6) return false;
  if (std::fabs(ref_y_-nc.ref_y_)>1e-6) return false;
  return true;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mfpf_pose_predictor& b)
{
    b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mfpf_pose_predictor& b)
{
    b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mfpf_pose_predictor& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

std::ostream& operator<<(std::ostream& os ,const mfpf_pose_type& pt)
{
  switch (pt)
  {
    case translation: os<<"translation"; break;
    case rigid: os<<"rigid"; break;
    case zoom: os<<"zoom"; break;
    case similarity: os<<"similarity"; break;
    default: os<<"Invalid"; break;
  }
  return os;
}
