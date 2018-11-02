#include <iostream>
#include <cstdlib>
#include "msm_shape_instance.h"
//:
// \file
// \brief Representation of an instance of a shape model.
// \author Tim Cootes

#include <msm/msm_shape_model.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/algo/vnl_cholesky.h>
#include <vnl/algo/vnl_svd.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//=======================================================================
// Dflt ctor
//=======================================================================

msm_shape_instance::msm_shape_instance()
  : model_(nullptr)
{
}

//: Set up model (retains pointer to model)
msm_shape_instance::msm_shape_instance(const msm_shape_model& model)
{
  set_shape_model(model);
}

//=======================================================================
// Destructor
//=======================================================================

msm_shape_instance::~msm_shape_instance() = default;

//: Set up model (retains pointer to model)
void msm_shape_instance::set_shape_model(const msm_shape_model& model)
{
  model_=&model;
  ref_shape_.set_shape_model(model);
  pose_ = model.default_pose();

  points_valid_=false;
}

//: Define current pose
void msm_shape_instance::set_pose(const vnl_vector<double>& pose)
{
  assert(pose.size()==pose_.size());
  pose_=pose;
  points_valid_=false;
}

//: Define parameters
void msm_shape_instance::set_params(const vnl_vector<double>& b)
{
  assert(b.size()<=model().n_modes());
  ref_shape_.set_params(b);
  points_valid_=false;
}

//: Set all shape parameters to zero
void msm_shape_instance::set_to_mean()
{
  ref_shape_.set_to_mean();
  points_valid_=false;
}


//: Current shape (uses lazy evaluation)
const msm_points& msm_shape_instance::points()
{
  if (points_valid_) return points_;

  // Apply pose transform
  model().aligner().apply_transform(model_points(),pose_,points_);

  points_valid_=true;
  return points_;
}

//: Returns approximate scale of points
//  Actually returns scale of mean after applying current pose
double msm_shape_instance::approx_points_scale() const
{
  double s = model().aligner().scale(pose_);
  return model().ref_mean_points_scale() * s;
}


//: Finds parameters and pose to best match to points
//  All points equally weighted.
//  If res_var>0, and use_prior(), then effect of
//  Gaussian prior is to scale parameters by
//  mode_var/(mode_var+rv), where rv is the res_var scaled
//  to the reference frame.
void msm_shape_instance::fit_to_points(const msm_points& pts,
                                       double res_var)
{
  // Catch case when fitting to self
  if (&pts == &points_) return;

#if 0 // commented out
  // Estimate pose from mean model points to target points
  // Assuming that the modes are orthogonal to the mean,
  // and to the rotation vector for the mean,
  // this transformation maps into the tangent space
  model().aligner().calc_transform_from_ref(
                    ref_shape_.model().mean_points(),pts,pose_);

  vnl_vector<double> pose_inv = model().aligner().inverse(pose_);
  model().aligner().apply_transform(pts,pose_inv,tmp_points_);

  double s = model().aligner().scale(pose_);
  double rv=res_var/(s*s);  // Scale variance into ref frame.

  ref_shape_.fit_to_points(tmp_points_,rv);
  points_valid_=false;
#endif // 0
  vnl_vector<double> pose0 = pose_;

  // Estimate pose from current model points to target points
  model().aligner().calc_transform(model_points(),pts,pose_);

  double dp=1.0;
  int n_its=0;
  while (dp>1e-6 && n_its<10)
  {
    // Transform pts into model frame with these parameters
    vnl_vector<double> pose_inv = model().aligner().inverse(pose_);
    model().aligner().apply_transform(pts,pose_inv,tmp_points_);

    double s = model().aligner().scale(pose_);
    double rv=res_var/(s*s);  // Scale variance into ref frame.

    ref_shape_.fit_to_points(tmp_points_,rv);
    points_valid_=false;

    // Check that the pose has converged - if not repeat the loop.
    pose0 = pose_;
    model().aligner().calc_transform(model_points(),pts,pose_);
    dp = (pose_-pose0).magnitude();

    n_its++;
  }
}

//: Finds parameters and pose to best match to points
//  Errors on point i are weighted by wts[i]
void msm_shape_instance::fit_to_points_wt(const msm_points& pts,
                                          const vnl_vector<double>& wts)
{
  // Catch case when fitting to self
  if (&pts == &points_) return;

  vnl_vector<double> pose0 = pose_;
  vnl_vector<double> ref_wts;

  // Estimate pose from current model points to target points
  model().aligner().calc_transform_wt(model_points(),pts,wts,pose_);

  double dp=1.0;
  int n_its=0;
  while (dp>1e-6 && n_its<10)
  {
    // Transform pts into model frame with these parameters
    vnl_vector<double> pose_inv = model().aligner().inverse(pose_);
    model().aligner().apply_transform(pts,pose_inv,tmp_points_);

    if (ref_shape_.use_prior())
    {
      // Need to scale the weights into the reference frame
      double s = model().aligner().scale(pose_);
      ref_wts=wts/(s*s);
      ref_shape_.fit_to_points_wt(tmp_points_,ref_wts);
    }
    else
    {
      // Absolute value of weights not important, only relative values
      // So no need to scale them.
      ref_shape_.fit_to_points_wt(tmp_points_,wts);
    }

    points_valid_=false;

    // Check that the pose has converged - if not repeat the loop.
    pose0 = pose_;
    model().aligner().calc_transform_wt(model_points(),pts,wts,pose_);
    dp = (pose_-pose0).magnitude();

    n_its++;
  }
}

//: Finds parameters and pose to best match to points
//  Errors on point i are weighted by wt_mat[i] in target frame
void msm_shape_instance::fit_to_points_wt_mat(const msm_points& pts,
                                              const std::vector<msm_wt_mat_2d>& wt_mat)
{
  // Catch case when fitting to self
  if (&pts == &points_) return;

  unsigned n=model().size();
  assert(wt_mat.size()==n);

  vnl_vector<double> pose0 = pose_;

  // Estimate pose from current model points to target points
  model().aligner().calc_transform_wt_mat(model_points(),pts,
                                          wt_mat,pose_);

  double dp=1.0;
  int n_its=0;
  while (dp>1e-6 && n_its<10)
  {
    // Transform pts into model frame with these parameters
    vnl_vector<double> pose_inv = model().aligner().inverse(pose_);
    model().aligner().apply_transform(pts,pose_inv,tmp_points_);

    // Transform the weight matrices to the model frame
    // If A is 2x2 scale/rot component, then in model frame
    // error is (A*dx)'*W*(A*dx), thus:
    // wt_mat2[i] = A'*wt_mat[i]*A
    std::vector<msm_wt_mat_2d> wt_mat2(n);
    model().aligner().transform_wt_mat(wt_mat,pose_inv,wt_mat2);

    ref_shape_.fit_to_points_wt_mat(tmp_points_,wt_mat2);
    points_valid_=false;

    // Check that the pose has converged - if not repeat the loop.
    pose0 = pose_;
    model().aligner().calc_transform_wt_mat(model_points(),pts,
                                            wt_mat,pose_);
    dp = (pose_-pose0).magnitude();
    n_its++;
  }
}


//=======================================================================
// Method: version_no
//=======================================================================

short msm_shape_instance::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string msm_shape_instance::is_a() const
{
  return std::string("msm_shape_instance");
}

//=======================================================================
// Method: print
//=======================================================================

  // required if data is present in this class
void msm_shape_instance::print_summary(std::ostream& /*os*/) const
{
}

//=======================================================================
// Method: save
//=======================================================================

  // required if data is present in this class
void msm_shape_instance::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,ref_shape_);
  vsl_b_write(bfs,pose_);
}

//=======================================================================
// Method: load
//=======================================================================

  // required if data is present in this class
void msm_shape_instance::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,ref_shape_);
      vsl_b_read(bfs,pose_);
      break;
    default:
      std::cerr << "msm_shape_instance::b_read() :\n"
               << "Unexpected version number " << version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }

  points_valid_=false;
}


//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const msm_shape_instance& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, msm_shape_instance& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const msm_shape_instance& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_shape_instance& b)
{
 os << b;
}
