//:
// \file
// \author Tim Cootes
// \brief Calculate and apply 2D translations

#include <iostream>
#include <cstddef>
#include "msm_translation_aligner.h"
#include <vnl/vnl_vector.h>
#include <vsl/vsl_binary_loader.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================

  //: Compute parameters for inverse transformation
vnl_vector<double> msm_translation_aligner::inverse(const vnl_vector<double>& t) const
{
  return -t;
}

  //: Apply the transformation to the given points
void msm_translation_aligner::apply_transform(const msm_points& points,
                                              const vnl_vector<double>& trans,
                                              msm_points& new_points) const
{
  new_points=points;
  new_points.translate_by(trans[0],trans[1]);
}


//: Return scaling applied by the transform with given parameters.
double msm_translation_aligner::scale(const vnl_vector<double>&  /*trans*/) const
{
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
void msm_translation_aligner::calc_transform_from_ref(const msm_points&  /*ref_pts*/,
                                                      const msm_points& pts2,
                                                      vnl_vector<double>& trans) const
{
  vgl_point_2d<double> cog2 = pts2.cog();
  trans.set_size(2);
  trans[0]=cog2.x();
  trans[1]=cog2.y();
}

  //: Estimate parameter which best map points1 to points2
  //  Minimises ||points2-T(points1)||^2
void msm_translation_aligner::calc_transform(const msm_points& pts1,
                                             const msm_points& pts2,
                                             vnl_vector<double>& trans) const
{
  vgl_point_2d<double> cog1 = pts1.cog();
  vgl_point_2d<double> cog2 = pts2.cog();

  trans.set_size(2);
  trans[0]=cog2.x()-cog1.x();
  trans[1]=cog2.y()-cog1.y();
}

// Compute weighted CoG
inline vgl_point_2d<double> msm_wtd_cog(const msm_points& pts,
                                        const vnl_vector<double>& wts)
{
  const double* v=pts.vector().data_block();
  const double* w=wts.data_block();
  unsigned n=pts.size();
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
void msm_translation_aligner::calc_transform_wt(const msm_points& pts1,
                                                const msm_points& pts2,
                                                const vnl_vector<double>& wts,
                                                vnl_vector<double>& trans) const
{
  vgl_point_2d<double> cog1 = msm_wtd_cog(pts1,wts);
  vgl_point_2d<double> cog2 = msm_wtd_cog(pts2,wts);

  trans.set_size(2);
  trans[0]=cog2.x()-cog1.x();
  trans[1]=cog2.y()-cog1.y();
}

  //: Estimate parameters which map points allowing for anisotropic wts
//  Errors on point i are weighted by wt_mat[i] in pts2 frame.
//  ie error is sum (p2_i-T(p1_i)'*wt_mat[i]*(p2_i-T(p1_i)
void msm_translation_aligner::calc_transform_wt_mat(const msm_points& pts1,
                                                    const msm_points& pts2,
                                                    const std::vector<msm_wt_mat_2d>& wt_mat,
                                                    vnl_vector<double>& trans) const
{
  assert(pts2.size()==pts1.size());
  assert(wt_mat.size()==pts1.size());
  // Compute weighted CoGs
  msm_wt_mat_2d w_sum(0,0,0);
  double x1=0.0,y1=0.0;
  double x2=0.0,y2=0.0;
  const double* p1 = pts1.vector().begin();
  const double* p2 = pts2.vector().begin();
  const double* p1_end = pts1.vector().end();
  auto w=wt_mat.begin();
  for (;p1!=p1_end;p1+=2,p2+=2,++w)
  {
    double wa=w->m11(), wb=w->m12(), wc=w->m22();
    w_sum += (*w);
    x1 += wa*p1[0]+wb*p1[1];
    y1 += wb*p1[0]+wc*p1[1];
    x2 += wa*p2[0]+wb*p2[1];
    y2 += wb*p2[0]+wc*p2[1];
  }
  msm_wt_mat_2d w_inv = w_sum.inverse();
  double v11=w_inv.m11(), v12=w_inv.m12(), v22=w_inv.m22();
  vgl_point_2d<double> cog1(v11*x1+v12*y1,v12*x1+v22*y1);
  vgl_point_2d<double> cog2(v11*x2+v12*y2,v12*x2+v22*y2);

  trans.set_size(2);
  trans[0]=cog2.x()-cog1.x();
  trans[1]=cog2.y()-cog1.y();
}

  //: Apply transform to weight matrices (ie ignore translation component)
void msm_translation_aligner::transform_wt_mat(const std::vector<msm_wt_mat_2d>& wt_mat,
                                               const vnl_vector<double>&  /*trans*/,
                                               std::vector<msm_wt_mat_2d>& new_wt_mat) const
{
  new_wt_mat = wt_mat;
}

//: Returns params of pose such that pose(x) = pose1(pose2(x))
vnl_vector<double> msm_translation_aligner::compose(
                         const vnl_vector<double>& pose1,
                         const vnl_vector<double>& pose2) const
{
  return pose1+pose2;
}

//: Apply transform to generate points in some reference frame
//  For instance, depending on transform, may translate so the
//  centre of gravity is at the origin and scale to a unit size.
void msm_translation_aligner::normalise_shape(msm_points& points) const
{
  vgl_point_2d<double> cog = points.cog();
  points.translate_by(-cog.x(),-cog.y());
}


//: Find poses which align a set of points
//  On exit ref_mean_shape is the mean shape in the reference
//  frame, pose_to_ref[i] maps points[i] into the reference
//  frame (ie pose is the mapping from the reference frame to
//  the target frames).
// \param average_pose Average mapping from ref to target frame
void msm_translation_aligner::align_set(const std::vector<msm_points>& points,
                                        msm_points& ref_mean_shape,
                                        std::vector<vnl_vector<double> >& pose_to_ref,
                                        vnl_vector<double>& average_pose,
                                        ref_pose_source) const
{
  std::size_t n_shapes = points.size();
  assert(n_shapes>0);
  pose_to_ref.resize(n_shapes);

  // Use first shape as initial reference
  ref_mean_shape = points[0];
  normalise_shape(ref_mean_shape);

  vnl_vector<double> pose_from_ref;

  average_pose.set_size(2);
  average_pose.fill(0);

  for (unsigned i=0;i<n_shapes;++i)
  {
    if (points[i].size()!=ref_mean_shape.size())
    {
      std::cerr<<"msm_translation_aligner::align_set() shape "<<i
                <<" has different number of points to first shape"<<std::endl;
      std::abort();
    }
    calc_transform_from_ref(ref_mean_shape,points[i],pose_from_ref);
    pose_to_ref[i]=inverse(pose_from_ref);
    average_pose+=pose_from_ref;
  }

  mean_of_transformed(points,pose_to_ref,ref_mean_shape);
  average_pose/=n_shapes;
}

//=======================================================================

std::string msm_translation_aligner::is_a() const
{
  return std::string("msm_translation_aligner");
}

//: Create a copy on the heap and return base class pointer
msm_aligner* msm_translation_aligner::clone() const
{
  return new msm_translation_aligner(*this);
}
