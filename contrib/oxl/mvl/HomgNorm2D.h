// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
#ifndef HomgNorm2D_h_
#define HomgNorm2D_h_
#ifdef __GNUC__
#pragma interface "HomgNorm2D.h"
#endif
//
// Class : HomgNorm2D
//
// .SECTION Description
//    @{ HomgNorm2D is a class that normalizes a set of homogeneous points
//    by subtracting their centroid and uniformly scaling them so that
//    the average length (nonhomogenous) is $\sqrt2$.
//
//    In addition the class stores the matrices that describe the
//    transformations between normalized and unnormalized representations. @}
//
// .NAME        HomgNorm2D - Normalize homogeneous points.
// .LIBRARY     MViewCompute
// .HEADER	MultiView Package
// .INCLUDE     mvl/HomgNorm2D.h
// .FILE        HomgNorm2D.cxx
// .EXAMPLE     ../Examples/exampleHomgNorm2D.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 18 Aug 96
//
// .SECTION Modifications:
//     200598 FSM added diagnostic method allowing caller to detect coincident points.
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_vector.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/SimilarityMetric.h>

class HomgNorm2D : public SimilarityMetric {
public:
  // Constructors/Destructors--------------------------------------------------

// -- Construct a HomgNorm2D that will hold n normalized points.  
  HomgNorm2D(int n, bool unit_omega = true): _normalized(n),_unit_omega(unit_omega) {}

// -- Construct a HomgNorm2D from an array of homogeneous points.
// The points will be normalized as described above and the results
// stored in this class.  If the optional parameter unit_omega is
// set to false, then the points will not be scaled to ensure that
// the homogeneous parameter is one.
  HomgNorm2D(const vcl_vector<HomgPoint2D>& points, bool unit_omega = true);

// -- Destructor
 ~HomgNorm2D();

  // Computations--------------------------------------------------------------

// -- Perform the normalization
  void normalize(const vcl_vector<HomgPoint2D>& points);

  bool was_coincident(void) const { return was_coincident_; } // FSM

  void set(const vcl_vector<HomgPoint2D>& points) { normalize(points); }

  // Operations----------------------------------------------------------------

// -- Apply the normalization to the given point
  HomgPoint2D apply_normalization(const HomgPoint2D& p) { return imagehomg_to_homg(p); }

// -- Apply the inverse normalization to the given point
  HomgPoint2D apply_denormalization(const HomgPoint2D& p) { return homg_to_imagehomg(p); }

  // Data Access---------------------------------------------------------------
  
// -- Return the array of normalized points
  vcl_vector<HomgPoint2D>& get_normalized_points() { return _normalized; }

// -- Have the points been scaled so their third components are one?
  bool points_have_unit_omega() const { return _unit_omega; }

// -- Return the i'th normalized point.
  HomgPoint2D& operator [] (int i) { return _normalized[i]; }

// -- Return the i'th normalized point.
  HomgPoint2D& get (int i) { return _normalized[i]; }

protected:
  // Data Members--------------------------------------------------------------
  vcl_vector<HomgPoint2D> _normalized;
  bool _unit_omega;
  bool was_coincident_;  // FSM
  
private:
  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS HomgNorm2D.

