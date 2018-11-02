#ifndef msm_shape_instance_h_
#define msm_shape_instance_h_
//:
// \file
// \brief Representation of an instance of a shape model.
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <msm/msm_ref_shape_instance.h>
#include <msm/msm_shape_model.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Representation of an instance of a shape model.
//  Contains shape model parameters and the parameters of
//  the global (model to world) transformation.
//  Includes functions to fit instances to sets of points
//  and to generate sets of points.
//
//  By default, all shape parameters are used and params() returns
//  a vector of length equal to the full number of shape modes.
//  To use fewer modes, create a parameter vector with the desired
//  number, and call set_params(b).
//
//  points() and model_points() use lazy evaluation
class msm_shape_instance
{
 private:
  //: Shape model of which this is an instance
  const msm_shape_model *model_;

  //: Shape in reference frame
  msm_ref_shape_instance ref_shape_;

  //: Global transformation parameters
  vnl_vector<double> pose_;

  //: Current points in world frame (transformed model pts)
  msm_points points_;

  //: True if points up to date with b_ and pose_
  bool points_valid_;

  //: Workspace for points in fit_to_points
  msm_points tmp_points_;

 public:

  // Dflt ctor
  msm_shape_instance();

  //: Set up model (retains pointer to model)
  msm_shape_instance(const msm_shape_model& model);

  // Destructor
  ~msm_shape_instance();

  //: Set up model (retains pointer to model)
  void set_shape_model(const msm_shape_model& model);

  //: Pointer to current shape model
  const msm_shape_model* model_ptr() const
  { return model_; }

  //: Reference to current model
  const msm_shape_model& model() const
  { assert(model_!=nullptr); return *model_; }

  //: Current pose parameters
  const vnl_vector<double>& pose() const { return pose_; }

  //: Current shape parameters
  const vnl_vector<double>& params() const
  { return ref_shape_.params(); }

  //: Define current pose
  void set_pose(const vnl_vector<double>& pose);

  //: Define parameters
  void set_params(const vnl_vector<double>& b);

  //: Set all shape parameters to zero (pose unchanged)
  void set_to_mean();

  //: Current shape (uses lazy evaluation)
  const msm_points& points();

  //: Returns approximate scale of points
  //  Actually returns scale of mean after applying current pose
  double approx_points_scale() const;

  //: Current shape in model frame (uses lazy evaluation)
  const msm_points& model_points()
  { return ref_shape_.points(); }

  //: Shape in reference frame
  msm_ref_shape_instance& ref_shape() { return ref_shape_; }

  //: Finds parameters and pose to best match to points
  //  All points equally weighted.
  //  If pt_var>0, and use_prior(), then effect of
  //  Gaussian prior is to scale parameters by
  //  mode_var/(mode_var+pv), where pv is the pt_var scaled
  //  to the reference frame.
  // \param pt_var Variance on each point
  void fit_to_points(const msm_points& points, double pt_var=0);

  //: Finds parameters and pose to best match to points
  //  Errors on point i are weighted by wts[i].
  //  wts[i] is treated as the inverse of the error variance of
  //  point i, in the target frame.
  void fit_to_points_wt(const msm_points& points,
                        const vnl_vector<double>& wts);

  //: Finds parameters and pose to best match to points
  //  Errors on point i are weighted by wt_mat[i] in target frame
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
void vsl_b_write(vsl_b_ostream& bfs, const msm_shape_instance& pts);


//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_shape_instance& pts);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const msm_shape_instance& pts);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_shape_instance& pts);

#endif // msm_shape_instance_h_
