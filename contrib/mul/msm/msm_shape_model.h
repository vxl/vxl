#ifndef msm_shape_model_h_
#define msm_shape_model_h_
//:
// \file
// \brief Contains mean/modes etc of a shape model and aligner details
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <msm/msm_ref_shape_model.h>
#include <msm/msm_aligner.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Contains mean/modes etc of a shape model
//  Container to hold the components of a statistical shape model.
//  Uses a linear model the shape variation.
//  Global pose form defined by the choice of msm_aligner class.
//
//  The associated msm_shape_instance class contains functions to
//  synthesize and match to shapes using this model.
class msm_shape_model : public msm_ref_shape_model
{
 private:
  //: Pose to use as a default (for convenience)
  vnl_vector<double> default_pose_;

  //: Object used to deal with global transformations
  mbl_cloneable_ptr<msm_aligner> aligner_;

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

  //: Pose to use as a default (for convenience)
  const vnl_vector<double> default_pose() const
  { return default_pose_; }

  //: Object used to deal with global transformations
  const msm_aligner& aligner() const { return aligner_; }

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
  bool operator==(const msm_shape_model& model) const;
};


//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_shape_model& pts);


//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_shape_model& pts);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const msm_shape_model& pts);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_shape_model& pts);

#endif // msm_shape_model_h_
