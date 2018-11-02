#ifndef msm_ref_shape_model_h_
#define msm_ref_shape_model_h_
//:
// \file
// \brief Contains mean/modes etc of a shape model in ref. frame
// No information about global transformation
// - see msm_shape_model for that.
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <string>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_fwd.h>
#include <msm/msm_points.h>
#include <vnl/vnl_matrix.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <msm/msm_param_limiter.h>

//: Contains mean/modes etc of a shape model in a reference frame.
//  Container to hold the components of a statistical shape model
//  in the reference frame.
//  Uses a linear model the shape variation.
//  This has no information about the global transformation
//  (msm_aligner) - see msm_shape_model for that.
//
//  The associated msm_ref_shape_instance class contains functions to
//  synthesize and match to shapes using this model.
class msm_ref_shape_model
{
 protected:
  //: Mean of shape model
  msm_points mean_;

  //: Scale of mean points (=mean_.scale())
  double mean_points_scale_;

  //: Modes of variation
  vnl_matrix<double> modes_;

  //: Variance for each mode
  vnl_vector<double> mode_var_;

  //: Default choice of parameter limiter
  mbl_cloneable_ptr<msm_param_limiter> param_limiter_;

 public:

  // Dflt ctor
  msm_ref_shape_model();

  // Destructor
  ~msm_ref_shape_model();

  //: Set up model
  void set(const msm_points& mean,
           const vnl_matrix<double>& modes,
           const vnl_vector<double>& mode_var,
           const msm_param_limiter& param_limiter);

  //: Mean of shape model as vector
  const vnl_vector<double>& mean() const { return mean_.vector(); }

  //: Scale of mean points in ref. frame (=mean_.scale())
  double ref_mean_points_scale() const { return mean_points_scale_; }


  //: Mean of shape model as points
  const msm_points& mean_points() const { return mean_; }

  //: Modes of variation
  const vnl_matrix<double>& modes() const { return modes_; }

  //: Variance for each mode
  const vnl_vector<double>& mode_var() const { return mode_var_; }

  //: Current object which limits parameters
  const msm_param_limiter& param_limiter() const
  { return param_limiter_; }

  //: Current object which limits parameters (non-const)
  msm_param_limiter& param_limiter()
  { return param_limiter_; }

  //: Number of points
  unsigned size() const { return mean_.size(); }

  //: Number of modes
  unsigned n_modes() const { return modes_.columns(); }

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

  //: Equality test
  bool operator==(const msm_ref_shape_model& points) const;
};


//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_ref_shape_model& pts);


//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_ref_shape_model& pts);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const msm_ref_shape_model& pts);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_ref_shape_model& pts);

#endif // msm_ref_shape_model_h_
