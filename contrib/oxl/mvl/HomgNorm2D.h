// This is oxl/mvl/HomgNorm2D.h
#ifndef HomgNorm2D_h_
#define HomgNorm2D_h_
//:
//  \file
// \brief Normalize homogeneous points
//
//    HomgNorm2D is a class that normalizes a set of homogeneous points
//    by subtracting their centroid and uniformly scaling them so that
//    the average length (nonhomogeneous) is $\sqrt2$.
//
//    In addition the class stores the matrices that describe the
//    transformations between normalized and unnormalized representations.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 18 Aug 96
//
// \verbatim
//  Modifications:
//     200598 FSM added diagnostic method allowing caller to detect coincident points.
//     221002 Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_homg_point_2d.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/SimilarityMetric.h>

class HomgNorm2D : public SimilarityMetric
{
 public:
  // Constructors/Destructors--------------------------------------------------

//: Construct a HomgNorm2D that will hold n normalized points.
  HomgNorm2D(int n, bool unit_omega = true): normalized_(n),unit_omega_(unit_omega) {}

//: Construct a HomgNorm2D from an array of homogeneous points.
// The points will be normalized as described above and the results
// stored in this class.  If the optional parameter unit_omega is
// set to false, then the points will not be scaled to ensure that
// the homogeneous parameter is one.
  HomgNorm2D(const std::vector<HomgPoint2D>& points, bool unit_omega = true);
  HomgNorm2D(std::vector<vgl_homg_point_2d<double> > const& points, bool unit_omega = true);

//: Destructor
 ~HomgNorm2D() override;

  // Computations--------------------------------------------------------------

//: Perform the normalization
  void normalize(const std::vector<HomgPoint2D>& points);
  void normalize(std::vector<vgl_homg_point_2d<double> > const& points);

  bool was_coincident(void) const { return was_coincident_; } // FSM

  void set(const std::vector<HomgPoint2D>& points) { normalize(points); }
  void set(std::vector<vgl_homg_point_2d<double> > const& points) { normalize(points); }

  // Operations----------------------------------------------------------------

//: Apply the normalization to the given point
  HomgPoint2D apply_normalization(const HomgPoint2D& p) { return imagehomg_to_homg(p); }
  vgl_homg_point_2d<double> apply_normalization(vgl_homg_point_2d<double> const& p) { return imagehomg_to_homg(p); }

//: Apply the inverse normalization to the given point
  HomgPoint2D apply_denormalization(const HomgPoint2D& p) { return homg_to_imagehomg(p); }
  vgl_homg_point_2d<double> apply_denormalization(vgl_homg_point_2d<double> const& p) { return homg_to_imagehomg(p); }

  // Data Access---------------------------------------------------------------

//: Return the array of normalized points
  std::vector<HomgPoint2D>& get_normalized_points() { return normalized_; }
  std::vector<vgl_homg_point_2d<double> > normalized_points();

//: Have the points been scaled so their third components are one?
  bool points_have_unit_omega() const { return unit_omega_; }

//: Return the i'th normalized point.
  HomgPoint2D& operator [] (int i) { return normalized_[i]; }

//: Return the i'th normalized point.
  HomgPoint2D& get (int i) { return normalized_[i]; }

 protected:
  // Data Members--------------------------------------------------------------
  std::vector<HomgPoint2D> normalized_;
  bool unit_omega_;
  bool was_coincident_;  // FSM
};

#endif // HomgNorm2D_h_
