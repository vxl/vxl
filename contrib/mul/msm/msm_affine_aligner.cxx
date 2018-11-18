//:
// \file
// \author Tim Cootes
// \brief Calculate and apply 2D affine transformations

#include <iostream>
#include <cstddef>
#include "msm_affine_aligner.h"
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
vnl_vector<double> msm_affine_aligner::inverse(const vnl_vector<double>& t) const
{
  double a = t[0]+1, b = t[1],   tx = t[2];
  double c = t[3],   d = t[4]+1, ty = t[5];
  vnl_vector<double> q(6);

  double det = a*d-b*c;
  if (det==0) det=1;
  q[0] = d/det -1;
  q[1] = -b/det;
  q[2] = (b*ty-d*tx)/det;
  q[3] = -c/det;
  q[4] = a/det -1;
  q[5] = (c*tx-a*ty)/det;

  return q;
}

  //: Apply the transformation to the given points
void msm_affine_aligner::apply_transform(const msm_points& points,
                                             const vnl_vector<double>& t,
                                             msm_points& new_points) const
{
  new_points.vector().set_size(points.vector().size());
  double a = t[0]+1, b = t[1],   tx = t[2];
  double c = t[3],   d = t[4]+1, ty = t[5];

  const double* v1=points.vector().data_block();
  const double* end_v=points.vector().end();
  double* v2=new_points.vector().data_block();
  for (;v1!=end_v;v1+=2,v2+=2)
  {
    double x=v1[0],y=v1[1];
    v2[0]=a*x+b*y+tx;
    v2[1]=c*x+d*y+ty;
  }
}

//: Return scaling applied by the transform with given parameters.
double msm_affine_aligner::scale(const vnl_vector<double>& t) const
{
  assert(t.size()==6);
  double a = t[0]+1, b = t[1];
  double c = t[3],   d = t[4]+1;
  double s2 = std::fabs(a*d - b*c);
  if (s2>0) return std::sqrt(s2);

  return 0.0;
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
void msm_affine_aligner::calc_transform_from_ref(const msm_points& ref_pts,
                                                     const msm_points& pts2,
                                                     vnl_vector<double>& trans) const
{
  // Could be made more efficient.
  calc_transform(ref_pts,pts2,trans);
}

// Calculate affine parameter vector (a-1,b,tx,c,d-1,ty)
void msm_calc_affine_trans(vnl_vector<double>& trans,
                                        const vnl_matrix<double>& S,
                                        const vnl_vector<double>& rhs_x,
                                        const vnl_vector<double>& rhs_y)
{
  vnl_vector<double> tx(3),ty(3);
  vnl_cholesky C(S,vnl_cholesky::estimate_condition);
  if (C.rcond()>1e-6)
  {
    C.solve(rhs_x,&tx);
    C.solve(rhs_y,&ty);
  }
  else
  {
    vnl_svd<double> svd(S);
    tx = svd.solve(rhs_x);
    ty = svd.solve(rhs_y);
  }

  trans.set_size(6);
  trans[0]=tx[0]-1; trans[1]=tx[1]  ; trans[2]=tx[2];
  trans[3]=ty[0];   trans[4]=ty[1]-1; trans[5]=ty[2];
}


  //: Estimate parameter which best map points1 to points2
  //  Minimises ||points2-T(points1)||^2
void msm_affine_aligner::calc_transform(const msm_points& pts1,
                                            const msm_points& pts2,
                                            vnl_vector<double>& trans) const
{
  unsigned n=pts1.size();
  const double* p1 = pts1.vector().begin();
  const double* p2 = pts2.vector().begin();
  const double* p1_end = pts1.vector().end();

  vnl_matrix<double> S(3,3);
  vnl_vector<double> rhs_x(3),rhs_y(3);
  S.fill(0.0); rhs_x.fill(0); rhs_y.fill(0);

  for (;p1!=p1_end;p1+=2,p2+=2)
  {
    double px = p1[0];
    double py = p1[1];
    double tx = p2[0];
    double ty = p2[1];

    S(0,0) += px*px;
    S(0,1) += px*py;
    S(0,2) += px;
    S(1,1) += py*py;
    S(1,2) += py;

    rhs_x[0] += px*tx;
    rhs_x[1] += py*tx;
    rhs_x[2] += tx;

    rhs_y[0] += px*ty;
    rhs_y[1] += py*ty;
    rhs_y[2] += ty;
  }
  S(1,0) = S(0,1);
  S(2,0) = S(0,2);
  S(2,1) = S(1,2);
  S(2,2) = n;

  msm_calc_affine_trans(trans,S,rhs_x,rhs_y);
}

  //: Estimate parameters which map points1 to points2 allowing for weights
  //  Minimises sum of weighted squares error in frame of pts2,
  //  ie sum w_i * ||p2_i - T(p1_i)||
void msm_affine_aligner::calc_transform_wt(const msm_points& pts1,
                                               const msm_points& pts2,
                                               const vnl_vector<double>& wts,
                                               vnl_vector<double>& trans) const
{
  const double* p1 = pts1.vector().begin();
  const double* p2 = pts2.vector().begin();
  const double* w=wts.data_block();
  assert(wts.size()==pts1.size());
  const double* p1_end = pts1.vector().end();

  vnl_matrix<double> S(3,3);
  vnl_vector<double> rhs_x(3),rhs_y(3);
  S.fill(0.0); rhs_x.fill(0); rhs_y.fill(0);

  for (;p1!=p1_end;p1+=2,p2+=2,++w)
  {
    double px = p1[0];
    double py = p1[1];
    double tx = p2[0];
    double ty = p2[1];

    S(0,0) += (*w)*px*px;
    S(0,1) += (*w)*px*py;
    S(0,2) += (*w)*px;
    S(1,1) += (*w)*py*py;
    S(1,2) += (*w)*py;
    S(2,2) += (*w);

    rhs_x[0] += (*w)*px*tx;
    rhs_x[1] += (*w)*py*tx;
    rhs_x[2] += (*w)*tx;

    rhs_y[0] += (*w)*px*ty;
    rhs_y[1] += (*w)*py*ty;
    rhs_y[2] += (*w)*ty;
  }
  S(1,0) = S(0,1);
  S(2,0) = S(0,2);
  S(2,1) = S(1,2);

  msm_calc_affine_trans(trans,S,rhs_x,rhs_y);
}

//: Estimate parameters which map points allowing for anisotropic wts
//  Errors on point i are weighted by wt_mat[i] in pts2 frame.
//  ie error is sum (p2_i-T(p1_i)'*wt_mat[i]*(p2_i-T(p1_i)
void msm_affine_aligner::calc_transform_wt_mat(
                              const msm_points& pts1,
                              const msm_points& pts2,
                              const std::vector<msm_wt_mat_2d>& wt_mat,
                              vnl_vector<double>& trans) const
{
  assert(pts2.size()==pts1.size());
  assert(wt_mat.size()==pts1.size());

  const double* p1 = pts1.vector().begin();
  const double* p2 = pts2.vector().begin();
  const msm_wt_mat_2d* w = &wt_mat[0];
  const double* p1_end = pts1.vector().end();

  vnl_matrix<double> S(6,6);
  vnl_vector<double> rhs(6);
  S.fill(0.0); rhs.fill(0);

  for (;p1!=p1_end;p1+=2,p2+=2,++w)
  {
    double px = p1[0];
    double py = p1[1];
    double qx = p2[0];
    double qy = p2[1];

    S(0,0) += px*px*w->m11();
    S(0,1) += px*py*w->m11();
    S(0,2) += px*px*w->m12();
    S(0,3) += px*py*w->m12();
    S(0,4) += px*w->m11();
    S(0,5) += px*w->m12();

//  S(1,0) += py*px*w->m11();   Symmetric
    S(1,1) += py*py*w->m11();
    S(1,2) += py*px*w->m12();
    S(1,3) += py*py*w->m12();
    S(1,4) += py*w->m11();
    S(1,5) += py*w->m12();

//  S(2,0) += px*px*w->m12();   Symmetric
//  S(2,1) += px*py*w->m12();   Symmetric
    S(2,2) += px*px*w->m22();
    S(2,3) += px*py*w->m22();
    S(2,4) += px*w->m12();
    S(2,5) += px*w->m22();

//  S(3,0) += py*px*w->m12();   Symmetric
//  S(3,1) += py*py*w->m12();   Symmetric
//  S(3,2) += py*px*w->m22();   Symmetric
    S(3,3) += py*py*w->m22();
    S(3,4) += py*w->m12();
    S(3,5) += py*w->m22();

    S(4,4) += w->m11();
    S(4,5) += w->m12();
    S(5,5) += w->m22();

    rhs[0] += px*qx*w->m11() + px*qy*w->m12();
    rhs[1] += py*qx*w->m11() + py*qy*w->m12();
    rhs[2] += px*qx*w->m12() + px*qy*w->m22();
    rhs[3] += py*qx*w->m12() + py*qy*w->m22();
    rhs[4] += w->m11()*qx + w->m12()*qy;
    rhs[5] += w->m12()*qx + w->m22()*qy;
  }
  // Fill in symmetric elements
  S(1,0)=S(0,1);
  S(2,0)=S(0,2); S(2,1)=S(1,2);
  S(3,0)=S(0,3); S(3,1)=S(1,3); S(3,2)=S(2,3);
  S(4,0)=S(0,4); S(4,1)=S(1,4); S(4,2)=S(2,4); S(4,3)=S(3,4);
  S(5,0)=S(0,5); S(5,1)=S(1,5); S(5,2)=S(2,5); S(5,3)=S(3,5); S(5,4)=S(4,5);

  vnl_vector<double> t(6);
  vnl_cholesky C(S,vnl_cholesky::estimate_condition);
  if (C.rcond()>1e-6)
  {
    C.solve(rhs,&t);
  }
  else
  {
    vnl_svd<double> svd(S);
    t = svd.solve(rhs);
  }

  trans.set_size(6);
  trans[0]=t[0]-1; trans[1]=t[1]  ; trans[2]=t[4];
  trans[3]=t[2];   trans[4]=t[3]-1; trans[5]=t[5];

}


  //: Apply transform to weight matrices (ie ignore translation component)
  //  W_new = A'WA
void msm_affine_aligner::transform_wt_mat(
                                const std::vector<msm_wt_mat_2d>& wt_mat,
                                const vnl_vector<double>& t,
                                std::vector<msm_wt_mat_2d>& new_wt_mat) const
{
  double a = 1+t[0], b=t[1], c=t[3], d=t[4]+1;
  new_wt_mat.resize(wt_mat.size());
  for (unsigned i=0;i<wt_mat.size();++i)
  {
    double w1=wt_mat[i].m11(), w2=wt_mat[i].m12(), w3=wt_mat[i].m22();
    new_wt_mat[i]=msm_wt_mat_2d(a*a*w1+    2*a*c*w2+c*c*w3,
                                a*b*w1+(a*d+b*c)*w2+c*d*w3,
                                b*b*w1+    2*b*d*w2+d*d*w3);
  }

}

//: Returns params of pose such that pose(x) = pose1(pose2(x))
vnl_vector<double> msm_affine_aligner::compose(
                         const vnl_vector<double>& pose1,
                         const vnl_vector<double>& pose2) const
{
  double a = pose1[0]+1, b = pose1[1],   c = pose1[2];
  double d = pose1[3],   e = pose1[4]+1, f = pose1[5];
  double da = pose2[0]+1, db = pose2[1],   dc = pose2[2];
  double dd = pose2[3],   de = pose2[4]+1, df = pose2[5];

  vnl_vector<double> p(6);
  p[0] = a*da + b*dd -1.0;
  p[1] = a*db + b*de;
  p[2] = a*dc + b*df + c;
  p[3] = d*da + e*dd;
  p[4] = d*db + e*de -1.0;
  p[5] = d*dc + e*df + f;
  return p;
}

//: Apply transform to generate points in some reference frame
//  For instance, depending on transform, may translate so the
//  centre of gravity is at the origin and scale to a unit size.
void msm_affine_aligner::normalise_shape(msm_points& points) const
{
  vgl_point_2d<double> cog = points.cog();
  points.translate_by(-cog.x(),-cog.y());
  double L = points.vector().magnitude();
  if (L>1e-6) points.scale_by(1.0/L);
}


//: Find poses which align a set of points
//  On exit ref_mean_shape is the mean shape in the reference
//  frame, pose_to_ref[i] maps points[i] into the reference
//  frame (ie pose is the mapping from the reference frame to
//  the target frames).
// \param pose_source defines how alignment of ref_mean_shape is calculated
// \param average_pose Average mapping from ref to target frame
void msm_affine_aligner::align_set(const std::vector<msm_points>& points,
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
  average_pose.set_size(4);

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
        std::cerr<<"msm_affine_aligner::align_set() shape "<<i
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

  // This will get translation right, but will not cope well
  // with large rotation variations.
  average_pose/=n_shapes;
}

//=======================================================================

std::string msm_affine_aligner::is_a() const
{
  return std::string("msm_affine_aligner");
}

//: Create a copy on the heap and return base class pointer
msm_aligner* msm_affine_aligner::clone() const
{
  return new msm_affine_aligner(*this);
}
