#ifndef msm_shape_model_h_
#define msm_shape_model_h_
//:
// \file
// \brief Contains mean/modes etc of a shape model
// \author Tim Cootes

#include <vcl_cassert.h>
#include <vcl_iosfwd.h>
#include <vcl_string.h>
#include <vsl/vsl_fwd.h>
#include <msm/msm_points.h>
#include <msm/msm_aligner.h>
#include <vnl/vnl_matrix.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <msm/msm_param_limiter.h>

//: Contains mean/modes etc of a shape model
//  Container to hold the components of a statistical shape model.
//  Uses a linear model the shape variation.
//  Global pose form defined by the choice of msm_aligner class.
//
//  The associated msm_shape_instance class contains functions to
//  synthesize and match to shapes using this model.
class msm_shape_model
{
 private:
  //: Mean of shape model
  msm_points mean_;

  //: Modes of variation
  vnl_matrix<double> modes_;

  //: Variance for each mode
  vnl_vector<double> mode_var_;

  //: Pose to use as a default (for convenience)
  vnl_vector<double> default_pose_;

  //: Object used to deal with global transformations
  mbl_cloneable_ptr<msm_aligner> aligner_;

  //: Default choice of parameter limiter
  mbl_cloneable_ptr<msm_param_limiter> param_limiter_;

 public:

  // Dflt ctor
  msm_shape_model();

  // Destructor
  ~msm_shape_model();

  //: Set up model
  void set(const msm_points& mean,
           const vnl_matrix<double>& modes,
           const vnl_vector<double>& mode_var,
           const vnl_vector<double>& default_pose,
           const msm_aligner& aligner,
           const msm_param_limiter& param_limiter);

  //: Mean of shape model as vector
  const vnl_vector<double>& mean() const { return mean_.vector(); }

  //: Mean of shape model as points
  const msm_points& mean_points() const { return mean_; }

  //: Modes of variation
  const vnl_matrix<double>& modes() const { return modes_; }

  //: Variance for each mode
  const vnl_vector<double>& mode_var() const { return mode_var_; }

  //: Pose to use as a default (for convenience)
  const vnl_vector<double> default_pose() const
  { return default_pose_; }

  //: Object used to deal with global transformations
  const msm_aligner& aligner() const { return aligner_; }

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
  vcl_string is_a() const;

  //: Print class to os
  void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);

  //: Equality test
  bool operator==(const msm_shape_model& points);
};


//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_shape_model& pts);


//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_shape_model& pts);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const msm_shape_model& pts);

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const msm_shape_model& pts);

#endif // msm_shape_model_h_
