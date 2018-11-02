#ifndef msm_pose_maker_h_
#define msm_pose_maker_h_
//:
// \file
// \author Tim Cootes
// \brief Compute a direction at each point, usually normal to curve

#include <iostream>
#include <iosfwd>
#include <msm/msm_points.h>
#include <msm/msm_curve.h>
#include <vgl/vgl_vector_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Compute a direction at each point, usually normal to curve.
//  Generates a unit direction vector for each point.
//  Typically these are defined as normals to the curve passing
//  through the point.
//
//  Define tangent at point i as line between end0_[i] and end1_[i].
//  If end0_[i]==end1_[i] then use vector (1,0) in ref. frame.
class msm_pose_maker
{
 private:
  //: Define tangent at point i as line between end0_[i] and end1_[i]
  std::vector<unsigned> end0_;

  //: Define tangent at point i as line between end0_[i] and end1_[i]
  std::vector<unsigned> end1_;

 public:
  ~msm_pose_maker() = default;

  //: Set up definitions of directions from the curves.
  //  Where multiple curves pass through a point, the direction
  //  is defined by the first in the list.
  void set_from_curves(unsigned n_points, const msm_curves& curves);

  //: Returns true if direction defined at point i
  bool defined(unsigned i) const { return end0_[i]!=0 || end1_[i]!=0; }

  //: Compute a direction at each point, usually normal to curve.
  void create_vectors(const msm_points& points,
                      std::vector<vgl_vector_2d<double> >& dir) const;

  //: Compute mean distance between equivalent points, measured along normals to curves
  //  d_i = abs dot((points2[i]-points1[i]),normal[i]) where normals are to curves
  //  through points1.
  //  Where normal not defined, use the absolute distance.
  double mean_normal_distance(const msm_points& points1, const msm_points& points2) const;

  //: Print class to os
  void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_pose_maker& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_pose_maker& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const msm_pose_maker& b);

#endif // msm_pose_maker_h_
