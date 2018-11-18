//:
// \file
// \author Tim Cootes
// \brief Calculate and apply 2D rigid transformations (translation + rotation)
// Note: Alignment calculated by estimating optimal similarity transformation,
// then discarding scale.

#include <iostream>
#include <cstddef>
#include "msm_rigid_aligner.h"
#include <vnl/vnl_vector.h>
#include <vsl/vsl_binary_loader.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_cholesky.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//=======================================================================

  //: Compute parameters for inverse transformation
vnl_vector<double> msm_rigid_aligner::inverse(const vnl_vector<double>& t) const
{
  vnl_vector<double> q(3);
  double a=std::cos(t[0]), b=std::sin(t[0]);
  q[0] = -t[0];
  q[1] =  -a * t[1] - b * t[2];
  q[2] = b * t[1] - a * t[2];
  return q;
}

  //: Apply the transformation to the given points
void msm_rigid_aligner::apply_transform(const msm_points& points,
                                             const vnl_vector<double>& trans,
                                             msm_points& new_points) const
{
  new_points.vector().set_size(points.vector().size());
  double a=std::cos(trans[0]), b=std::sin(trans[0]), tx=trans[1], ty=trans[2];

  const double* v1=points.vector().data_block();
  const double* end_v=points.vector().end();
  double* v2=new_points.vector().data_block();
  for (;v1!=end_v;v1+=2,v2+=2)
  {
    double x=v1[0],y=v1[1];
    v2[0]=a*x-b*y+tx;
    v2[1]=b*x+a*y+ty;
  }
}

//: Return scaling applied by the transform with given parameters.
double msm_rigid_aligner::scale(const vnl_vector<double>& trans) const
{
  assert(trans.size()==3);
  return 1.0;
}


//: Estimate parameter which best map ref_points to points2
//  Minimises ||points2-T(ref_points)||^2.
//  Takes advantage of assumed properties of ref_points (eg CoG=origin,
//  unit size etc) to perform efficiently.
//
//  When used with a shape model of form ref_points+Pb, where the modes P
//  have certain orthogonality properties with respect to the ref shape,
//  this can give the optimal transformation into a tangent plane, independent
//  of the current parameters.  In this case a one-shot method can be used
//  to compute the optimal shape and pose parameters, rather than an iterative
//  method which is required where the orthogonality properties do not hold,
//  or where weights are considered.
void msm_rigid_aligner::calc_transform_from_ref(const msm_points& ref_pts,
                                                     const msm_points& pts2,
                                                     vnl_vector<double>& trans) const
{
  vgl_point_2d<double> cog2 = pts2.cog();
  const double* p1 = ref_pts.vector().begin();
  const double* p2 = pts2.vector().begin();
  const double* p1_end = ref_pts.vector().end();

  double xy_sum=0.0;
  double dot_sum=0.0;

  for (;p1!=p1_end;p1+=2,p2+=2)
  {
    double dpx = p1[0];
    double dpy = p1[1];
    double dtx = p2[0]-cog2.x();
    double dty = p2[1]-cog2.y();

    dot_sum += dpx*dtx + dpy*dty;
    xy_sum += dpx*dty - dpy*dtx;
  }

  trans.set_size(3);
  trans[0] = std::atan2(xy_sum,dot_sum);
  trans[1] = cog2.x();
  trans[2] = cog2.y();
}

  //: Estimate parameter which best map points1 to points2
  //  Minimises ||points2-T(points1)||^2
void msm_rigid_aligner::calc_transform(const msm_points& pts1,
                                            const msm_points& pts2,
                                            vnl_vector<double>& trans) const
{
  vgl_point_2d<double> cog1 = pts1.cog();
  vgl_point_2d<double> cog2 = pts2.cog();
  const double* p1 = pts1.vector().begin();
  const double* p2 = pts2.vector().begin();
  const double* p1_end = pts1.vector().end();

  double x2_sum=0.0;
  double xy_sum=0.0;
  double dot_sum=0.0;

  for (;p1!=p1_end;p1+=2,p2+=2)
  {
    double dpx = p1[0]-cog1.x();
    double dpy = p1[1]-cog1.y();
    double dtx = p2[0]-cog2.x();
    double dty = p2[1]-cog2.y();

    x2_sum += dpx*dpx + dpy*dpy;
    dot_sum += dpx*dtx + dpy*dty;
    xy_sum += dpx*dty - dpy*dtx;
  }

  trans.set_size(3);
  trans[0] = std::atan2(xy_sum,dot_sum);
  double a=std::cos(trans[0]), b=std::sin(trans[0]);
  trans[1] = cog2.x() - (a*cog1.x() - b*cog1.y());
  trans[2] = cog2.y() - (b*cog1.x() + a*cog1.y());
}

// Compute weighted CoG
inline vgl_point_2d<double> msm_wtd_cog(const msm_points& pts,
                                        const vnl_vector<double>& wts)
{
  const double* v=pts.vector().data_block();
  const double* w=wts.data_block();
  const unsigned int n=pts.size();
  const double* end_v=v+2*n;
  double cx=0.0,cy=0.0,w_sum=0.0;
  for (;v!=end_v;v+=2,++w)
  { cx+=w[0]*v[0]; cy+=w[0]*v[1]; w_sum+=w[0]; }

  if (w_sum>0) { cx/=w_sum; cy/=w_sum; }
  return {cx,cy};
}

  //: Estimate parameters which map points1 to points2 allowing for weights
  //  Minimises sum of weighted squares error in frame of pts2,
  //  ie sum w_i * ||p2_i - T(p1_i)||
void msm_rigid_aligner::calc_transform_wt(const msm_points& pts1,
                                               const msm_points& pts2,
                                               const vnl_vector<double>& wts,
                                               vnl_vector<double>& trans) const
{
  vgl_point_2d<double> cog1 = msm_wtd_cog(pts1,wts);
  vgl_point_2d<double> cog2 = msm_wtd_cog(pts2,wts);
  const double* p1 = pts1.vector().begin();
  const double* p2 = pts2.vector().begin();
  const double* w=wts.data_block();
  assert(wts.size()==pts1.size());
  const double* p1_end = pts1.vector().end();

  double x2_sum=0.0;
  double xy_sum=0.0;
  double dot_sum=0.0;

  for (;p1!=p1_end;p1+=2,p2+=2,++w)
  {
    double dpx = p1[0]-cog1.x();
    double dpy = p1[1]-cog1.y();
    double dtx = p2[0]-cog2.x();
    double dty = p2[1]-cog2.y();

    x2_sum  += w[0]*(dpx*dpx + dpy*dpy);
    dot_sum += w[0]*(dpx*dtx + dpy*dty);
    xy_sum  += w[0]*(dpx*dty - dpy*dtx);
  }

  trans.set_size(3);
  trans[0] = std::atan2(xy_sum,dot_sum);
  double a=std::cos(trans[0]), b=std::sin(trans[0]);
  trans[1] = cog2.x() - (a*cog1.x() - b*cog1.y());
  trans[2] = cog2.y() - (b*cog1.x() + a*cog1.y());

}

//: Estimate parameters which map points allowing for anisotropic wts
//  Errors on point i are weighted by wt_mat[i] in pts2 frame.
//  ie error is sum (p2_i-T(p1_i)'*wt_mat[i]*(p2_i-T(p1_i)
void msm_rigid_aligner::calc_transform_wt_mat(
                              const msm_points& pts1,
                              const msm_points& pts2,
                              const std::vector<msm_wt_mat_2d>& wt_mat,
                              vnl_vector<double>& trans) const
{
  assert(pts2.size()==pts1.size());
  assert(wt_mat.size()==pts1.size());

  // Perform computations relative to CoG,
  // to minimise rounding errors
  vgl_point_2d<double> cog1 = pts1.cog();
  vgl_point_2d<double> cog2 = pts2.cog();

  // Compute weighted CoGs

  msm_wt_mat_2d w_sum(0,0,0);
  msm_wt_mat_2d S(0,0,0); // Sum
  double tx_sum = 0, ty_sum=0;
  double txx_sum = 0, txy_sum = 0;
  double s31=0,s32=0,s41=0,s42=0;

  const double* p1 = pts1.vector().begin();
  const double* p2 = pts2.vector().begin();
  const double* p1_end = pts1.vector().end();
  auto w=wt_mat.begin();

  for (;p1!=p1_end;p1+=2,p2+=2,++w)
  {
    double dpx = p1[0]-cog1.x();
    double dpy = p1[1]-cog1.y();
    double dtx = p2[0]-cog2.x();
    double dty = p2[1]-cog2.y();

    double wa=w->m11(), wb=w->m12(), wc=w->m22();

    w_sum += (*w);

    double wx = wa*dpx + wb*dpy;
    double wy = wb*dpx + wc*dpy;
    double vx = wb*dpx - wa*dpy;
    double vy = wc*dpx - wb*dpy;
    s31+=wx; s32+=vx;
    s41+=wy; s42+=vy;

    S += msm_wt_mat_2d(dpx*wx + dpy*wy,
                       dpx*wy - dpy*wx,
                       dpx*vy - dpy*vx);

    // Right hand side
    txx_sum += dtx*wx + dty*wy;
    txy_sum += dtx*vx + dty*vy;
    tx_sum += wa*dtx+wb*dty;
    ty_sum += wb*dtx+wc*dty;
  }

  vnl_matrix<double> M(4,4);
  M(0,0)=S.m11(); M(0,1)=S.m12(); M(0,2)=s31; M(0,3)=s41;
  M(1,0)=S.m21(); M(1,1)=S.m22(); M(1,2)=s32; M(1,3)=s42;
  M(2,0)=    s31; M(2,1)= s32; M(2,2)=w_sum.m11(); M(2,3)=w_sum.m12();
  M(3,0)=    s41; M(3,1)= s42; M(3,2)=w_sum.m21(); M(3,3)=w_sum.m22();

  vnl_vector<double> rhs(4);
  rhs[0]=txx_sum; rhs[1]=txy_sum; rhs[2]=tx_sum; rhs[3]=ty_sum;

  // M is symmetric, so attempt to use Cholesky
  vnl_cholesky chol(M,vnl_cholesky::estimate_condition);
  vnl_vector<double> q(4);
  if (chol.rcond()>1e-6)
  {
    chol.solve(rhs,&q);
  }
  else
  {
    vnl_svd<double> svd(M);
    q = svd.solve(rhs);
  }

  trans.set_size(3);
  trans[0] = std::atan2(q[1],q[0]);
  double a=std::cos(trans[0]), b=std::sin(trans[0]);

  // Include effects of translations by CoG
  trans[1] = q[2] + cog2.x() - (a*cog1.x() - b*cog1.y());
  trans[2] = q[3] + cog2.y() - (b*cog1.x() + a*cog1.y());
}


  //: Apply transform to weight matrices (ie ignore translation component)
void msm_rigid_aligner::transform_wt_mat(
                                const std::vector<msm_wt_mat_2d>& wt_mat,
                                const vnl_vector<double>& trans,
                                std::vector<msm_wt_mat_2d>& new_wt_mat) const
{
  double a=std::cos(trans[0]), b=std::sin(trans[0]);
  new_wt_mat.resize(wt_mat.size());
  for (unsigned i=0;i<wt_mat.size();++i)
    new_wt_mat[i]=wt_mat[i].transform_by(a,b);
}

//: Returns params of pose such that pose(x) = pose1(pose2(x))
vnl_vector<double> msm_rigid_aligner::compose(
                         const vnl_vector<double>& pose1,
                         const vnl_vector<double>& pose2) const
{
  double a=std::cos(pose1[0]), b=std::sin(pose1[0]);

  vnl_vector<double> p(3);
  p[0]= pose1[0]+pose2[0];
  p[1]= a*pose2[1]-b*pose2[2]+pose1[1];
  p[2]= b*pose2[1]+a*pose2[2]+pose1[2];
  return p;
}

//: Apply transform to generate points in some reference frame
//  For instance, depending on transform, may translate so the
//  centre of gravity is at the origin and scale to a unit size.
void msm_rigid_aligner::normalise_shape(msm_points& points) const
{
  vgl_point_2d<double> cog = points.cog();
  points.translate_by(-cog.x(),-cog.y());
}


//: Find poses which align a set of points
//  On exit ref_mean_shape is the mean shape in the reference
//  frame, pose_to_ref[i] maps points[i] into the reference
//  frame (ie pose is the mapping from the reference frame to
//  the target frames).
// \param pose_source defines how orientation of ref_mean_shape is calculated
// \param average_pose Average mapping from ref to target frame
void msm_rigid_aligner::align_set(const std::vector<msm_points>& points,
                                       msm_points& ref_mean_shape,
                                       std::vector<vnl_vector<double> >& pose_to_ref,
                                       vnl_vector<double>& average_pose,
                                       ref_pose_source pose_source) const
{
  std::size_t n_shapes = points.size();
  assert(n_shapes>0);
  pose_to_ref.resize(n_shapes);

  // Use first shape as initial reference
  ref_mean_shape = points[0];
  normalise_shape(ref_mean_shape);

  // Record normalised first shape to define initial orientation
  msm_points base_shape = ref_mean_shape;

  vnl_vector<double> pose_from_ref;
  msm_points new_mean=ref_mean_shape;
  average_pose.set_size(3);

  double change=1.0;

  unsigned n_its=0;

  while (change>1e-6 && n_its<10)
  {
    ref_mean_shape = new_mean;
    normalise_shape(ref_mean_shape);

    average_pose.fill(0);

    for (unsigned i=0;i<n_shapes;++i)
    {
      if (points[i].size()!=ref_mean_shape.size())
      {
        std::cerr<<"msm_rigid_aligner::align_set() shape "<<i
                 <<" has different number of points to first shape"<<std::endl;
        std::abort();
      }
      calc_transform_from_ref(ref_mean_shape,points[i],pose_from_ref);
      pose_to_ref[i]=inverse(pose_from_ref);
      average_pose+=pose_from_ref;
    }

    mean_of_transformed(points,pose_to_ref,new_mean);

    // new_mean should be in the subspace orthogonal to
    // the mean and to rotation defined by the mean.

    change = (new_mean.vector()-ref_mean_shape.vector()).rms();

    if (pose_source==mean_pose && n_its==0)
    {
      // Use the average pose to define the orientation of the mean in the ref frame
      apply_transform(new_mean,average_pose/n_shapes,new_mean);
      change=1.0;  // Force at least one more iteration
    }

    n_its++;
  }

  average_pose/=n_shapes;
}

//=======================================================================

std::string msm_rigid_aligner::is_a() const
{
  return std::string("msm_rigid_aligner");
}

//: Create a copy on the heap and return base class pointer
msm_aligner* msm_rigid_aligner::clone() const
{
  return new msm_rigid_aligner(*this);
}
