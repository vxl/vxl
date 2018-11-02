#ifndef msm_ref_shape_instance_h_
#define msm_ref_shape_instance_h_
//:
// \file
// \brief Representation of an instance of a shape model in ref frame.
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <msm/msm_points.h>
#include <msm/msm_wt_mat_2d.h>
#include <msm/msm_param_limiter.h>
#include <mbl/mbl_cloneable_ptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class msm_ref_shape_model;

//: Representation of an instance of a shape model in ref frame.
//  Includes functions to fit instances to sets of points
//  and to generate sets of points.
//
//  By default, all shape parameters are used and params() returns
//  a vector of length equal to the full number of shape modes.
//  To use fewer modes, create a parameter vector with the desired
//  number, and call set_params(b).
//
//  This uses no global pose - use msm_shape_instance for those.
class msm_ref_shape_instance
{
 private:
  //: Shape model of which this is an instance
  const msm_ref_shape_model *model_;

  //: Model parameters
  vnl_vector<double> b_;

  //: Current model points
  msm_points points_;

  //: When true, use Gaussian prior on params in fit_to_points*
  bool use_prior_;

  //: True if model points up to date with b_
  bool points_valid_;

  //: Object which applies limits to parameters in fit_to_points*
  mbl_cloneable_ptr<msm_param_limiter> param_limiter_;

  //: Workspace for points in fit_to_points
  msm_points tmp_points_;

 public:

  // Dflt ctor
  msm_ref_shape_instance();

  // Destructor
  ~msm_ref_shape_instance();

  //: Set up model (retains pointer to model)
  void set_shape_model(const msm_ref_shape_model& model);

  //: Define limits on parameters (clone taken)
  void set_param_limiter(const msm_param_limiter& limiter);

  //: Current object which limits parameters
  const msm_param_limiter& param_limiter() const
  { return param_limiter_; }

  //: Current object which limits parameters (non-const)
  msm_param_limiter& param_limiter()
  { return param_limiter_; }

  //: When true, use Gaussian prior on params in fit_to_points*
  bool use_prior() const { return use_prior_; }

  //: When true, use Gaussian prior on params in fit_to_points*
  void set_use_prior(bool);

  //: Define parameters
  void set_params(const vnl_vector<double>& b);

  //: Set all shape parameters to zero
  void set_to_mean();

  //: Pointer to current model
  const msm_ref_shape_model* model_ptr() const { return model_; }

  //: Reference to current model
  const msm_ref_shape_model& model() const
  { assert(model_!=nullptr); return *model_; }

  //: Current shape parameters
  const vnl_vector<double>& params() const { return b_; }

  //: Current shape in model frame (uses lazy evaluation)
  const msm_points& points();

  //: Finds parameters to best match to points
  //  All points equally weighted.
  //  If pt_var>0, and use_prior(), then effect of
  //  Gaussian prior is to scale parameters by
  //  mode_var/(mode_var+pt_var).
  void fit_to_points(const msm_points& points, double pt_var=0);

  //: Finds parameters to best match to points
  //  Errors on point i are weighted by wts[i]
  //  If use_prior(), then include Gaussian prior on
  //  the shape parameters, and assume that wts are
  //  inverse variances.
  void fit_to_points_wt(const msm_points& points,
                        const vnl_vector<double>& wts);

  //: Finds parameters to best match to points
  //  Errors on point i are weighted by wt_mat[i] in target frame
  //  If use_prior(), then include Gaussian prior on
  //  the shape parameters, and assume that wt_mat are
  //  inverse covariances.
  void fit_to_points_wt_mat(const msm_points& pts,
                        const std::vector<msm_wt_mat_2d>& wt_mat);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const;

  //: Print class to os
  void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);
};


//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_ref_shape_instance& pts);


//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_ref_shape_instance& pts);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const msm_ref_shape_instance& pts);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_ref_shape_instance& pts);

#endif // msm_ref_shape_instance_h_
