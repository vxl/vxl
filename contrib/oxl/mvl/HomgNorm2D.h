// This is oxl/mvl/HomgNorm2D.h
#ifndef HomgNorm2D_h_
#define HomgNorm2D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
// \brief Normalize homogeneous points
//
//    HomgNorm2D is a class that normalizes a set of homogeneous points
//    by subtracting their centroid and uniformly scaling them so that
//    the average length (nonhomogenous) is $\sqrt2$.
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
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/SimilarityMetric.h>

class HomgNorm2D : public SimilarityMetric
{
 public:
  // Constructors/Destructors--------------------------------------------------

//: Construct a HomgNorm2D that will hold n normalized points.
  HomgNorm2D(int n, bool unit_omega = true): _normalized(n),_unit_omega(unit_omega) {}

//: Construct a HomgNorm2D from an array of homogeneous points.
// The points will be normalized as described above and the results
// stored in this class.  If the optional parameter unit_omega is
// set to false, then the points will not be scaled to ensure that
// the homogeneous parameter is one.
  HomgNorm2D(const vcl_vector<HomgPoint2D>& points, bool unit_omega = true);

//: Destructor
 ~HomgNorm2D();

  // Computations--------------------------------------------------------------

//: Perform the normalization
  void normalize(const vcl_vector<HomgPoint2D>& points);

  bool was_coincident(void) const { return was_coincident_; } // FSM

  void set(const vcl_vector<HomgPoint2D>& points) { normalize(points); }

  // Operations----------------------------------------------------------------

//: Apply the normalization to the given point
  HomgPoint2D apply_normalization(const HomgPoint2D& p) { return imagehomg_to_homg(p); }

//: Apply the inverse normalization to the given point
  HomgPoint2D apply_denormalization(const HomgPoint2D& p) { return homg_to_imagehomg(p); }

  // Data Access---------------------------------------------------------------

//: Return the array of normalized points
  vcl_vector<HomgPoint2D>& get_normalized_points() { return _normalized; }

//: Have the points been scaled so their third components are one?
  bool points_have_unit_omega() const { return _unit_omega; }

//: Return the i'th normalized point.
  HomgPoint2D& operator [] (int i) { return _normalized[i]; }

//: Return the i'th normalized point.
  HomgPoint2D& get (int i) { return _normalized[i]; }

 protected:
  // Data Members--------------------------------------------------------------
  vcl_vector<HomgPoint2D> _normalized;
  bool _unit_omega;
  bool was_coincident_;  // FSM
};

#endif // HomgNorm2D_h_
