#ifndef msm_aligner_h_
#define msm_aligner_h_

//:
// \file
// \author Tim Cootes
// \brief Base for functions which calculate and apply 2D transformations

#include <string>
#include <iostream>
#include <memory>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <msm/msm_points.h>
#include <msm/msm_wt_mat_2d.h>
class mbl_read_props_type;

//: Base for functions which calculate and apply 2D transformations.
//  Derived classes represent transformations, eg translation, similarity, affine.
//  The parameters of the transformation are stored in a vector, arranged so
//  that the zero vector corresponds to the identity transformation.
//  Each class contains functions to apply the transformation, invert it
//  and to estimate the best transform parameters to map one set of points to
//  another.
class msm_aligner
{
 public:
  virtual ~msm_aligner() = default;

  //: Return number of parameters defining the transformation
  virtual unsigned size() const=0;

  //: Compute parameters for inverse transformation
  virtual vnl_vector<double> inverse(const vnl_vector<double>&) const = 0;

  //: Apply the transformation to the given points
  virtual void apply_transform(const msm_points& points,
                     const vnl_vector<double>& trans,
                     msm_points& new_points) const = 0;

  //: Return scaling applied by the transform with given parameters.
  virtual double scale(const vnl_vector<double>& trans) const = 0;

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
  virtual void calc_transform_from_ref(const msm_points& ref_points,
                              const msm_points& points2,
                              vnl_vector<double>& trans) const =0;

  //: Estimate parameter which best map points1 to points2
  //  Minimises ||points2-T(points1)||^2
  virtual void calc_transform(const msm_points& points1,
                              const msm_points& points2,
                              vnl_vector<double>& trans) const =0;

  //: Estimate parameters which map points1 to points2 allowing for weights
  //  Minimises sum of weighted squares error in frame of pts2,
  //  ie sum w_i * ||p2_i - T(p1_i)||
  virtual void calc_transform_wt(const msm_points& points1,
                              const msm_points& points2,
                              const vnl_vector<double>& wts,
                              vnl_vector<double>& trans) const =0;

  //: Estimate parameters which map points allowing for anisotropic wts
//  Errors on point i are weighted by wt_mat[i] in pts2 frame.
//  ie error is sum (p2_i-T(p1_i)'*wt_mat[i]*(p2_i-T(p1_i)
  virtual void calc_transform_wt_mat(const msm_points& points1,
                              const msm_points& points2,
                              const std::vector<msm_wt_mat_2d>& wt_mat,
                              vnl_vector<double>& trans) const =0;

  //: Apply transform to weight matrices (ie ignore translation component)
  virtual void transform_wt_mat(const std::vector<msm_wt_mat_2d>& wt_mat,
                                const vnl_vector<double>& trans,
                                std::vector<msm_wt_mat_2d>& new_wt_mat) const = 0;

  //: Returns params of pose such that pose(x) = pose1(pose2(x))
  virtual vnl_vector<double> compose(const vnl_vector<double>& pose1,
                         const vnl_vector<double>& pose2) const = 0;

  //: Apply transform to generate points in some reference frame
  //  For instance, depending on transform, may translate so the
  //  centre of gravity is at the origin and scale to a unit size.
  virtual void normalise_shape(msm_points& points) const = 0;

  //: Options for defining pose in reference frame
  enum ref_pose_source { first_shape, mean_pose };

  //: Find poses which align a set of points
  //  On exit ref_mean_shape is the mean shape in the reference
  //  frame, pose_to_ref[i] maps points[i] into the reference
  //  frame (ie pose is the mapping from the reference frame to
  //  the target frames).
  // \param pose_source defines how alignment of ref_mean_shape is calculated
  // \param average_pose Some estimate of the average mapping
  virtual void align_set(const std::vector<msm_points>& points,
                         msm_points& ref_mean_shape,
                         std::vector<vnl_vector<double> >& pose_to_ref,
                         vnl_vector<double>& average_pose,
                         ref_pose_source pose_source=first_shape) const =0;

  //: Compute mean of points[i] after transforming with pose[i]
  void mean_of_transformed(const std::vector<msm_points>& points,
                         const std::vector<vnl_vector<double> >& pose,
                         msm_points& mean) const;

  //: Name of the class
  virtual std::string is_a() const = 0;

  //: Create a copy on the heap and return base class pointer
  virtual msm_aligner* clone() const = 0;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: Create a concrete msm_aligner-derived object, from a text specification.
  static std::unique_ptr<msm_aligner> create_from_stream(std::istream &is);

  //: Initialise from a text stream.
  // The default implementation is for attribute-less normalisers,
  // and throws if it finds any data in the stream.
  virtual void config_from_stream(std::istream &is);
};

//: Allows derived class to be loaded by base-class pointer
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(bfs,base_ptr);".  This loads derived class
//  objects from the disk, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const msm_aligner& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_aligner& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_aligner& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const msm_aligner& b);

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const msm_aligner* b);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_aligner& b);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_aligner* b);

#endif // msm_aligner_h_
