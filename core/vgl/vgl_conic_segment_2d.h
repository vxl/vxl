// This is core/vgl/vgl_conic_segment_2d.h
#ifndef vgl_conic_segment_2d_h_
#define vgl_conic_segment_2d_h_
//:
// \file
// \author J.L. Mundy  June 18, 2005
// \brief A curve segment with the geometry of a conic
//
// A conic segment contains those points of a given conic which lie between the
// given start point (inclusive) and the given end point, in a certain direction
// (counter-clockwise by default).
// No automatic validation checking is done for those two points effectively
// lying on the conic; this is left to the user of the class. Operations on
// vgl_conic_segment_2d<T> like intersection should actually take the "points
// closest to the given end points" as the effective end points.
//
// The concept of counter-clockwise-ness is not projectively invariant, but is
// invariant under most projective transformations that are useful in vision,
// and most certainly is under affine and Euclidean transformations. Actually,
// as long as the centre of the conic does not traverse the line at infinity,
// orientation and thus counter-clockwise-ness of the conic is kept. There is
// only one affine situation which is still ambiguous, viz. when the conic is
// a hyperbola. In that case, the definition of "counter-clockwise" is based
// --by definition-- on the view of the start point. Only when both end points
// are at infinity, i.e. when, the segment is one of the two branches of the
// hyperbola, this is still ambiguous and there is no way to distinguish which
// of the two branches is meant by just giving the two end points.
//
// The only projectively "correct" way to define a segment would be to specify
// a third point on the conic, or a direction vector from the start point.
// This would be an overkill for most applications, though.
// Those applications that require a fully projectively invariant segment should
// consider storing an additional third point together with the conic segment,
// and swap the two endpoints (by using the swap_endpoints() method) whenever
// a transformation is applied that would invert the orientation of the conic.
//
// One more ambiguous situation left is a conic segment specified by two
// identical end points. By definition, in this case, the segment only contains
// of this single point. This is compatible with the use of conic segments in
// conic fitting algorithms, where the shortest rather than the longest segment
// should be selected.
//
// When the conic is an ellipse, the semantics of the conic segment are clear:
// seen from the inside of the ellipse, and starting from the first end point,
// one goes to the left to traverse the conic segment in a counter-clockwise way.
//
// If the conic is a parabola, the first endpoint should typically lie to the
// right of the second one (as seen from the focal point of the parabola): in
// that case, the conic segment has finite length when traversed in counter-
// clockwise direction. Otherwise, we still have a valid conic segment but it
// consists of two separate, infinite branches.
//
// If the conic is a hyperbola, there are even three cases to be considered:
// if both end points lie on the same branch of the hyperbola, and the first
// one lies to the right of the second one as seen from the "inside" of that
// branch, the conic segment is a finite curve. This is the only finite case.
// By swapping the end points, the segment will have three branches, not just
// two: it will contain the two infinite fragments "outside" the end points on
// the hyperbola branch of the end points, and also the complete other branch
// of the hyperbola.
// Finally, if the end points lie on different hyperbola branches, the segment
// consists of all points to the left of the start point (as seen from the
// "inside" of the start point's branch) and of all points to the left of the
// end point (as seen from the "inside" of the end point's branch).
// In this case, swapping the end points does not change the segment! As a
// consequence, it will never be possible to specify the complement of this
// segment as a single vgl_conic_segment_2d, only as the union of two.
//
// End points can of course happily be points at infinity (if the conic is
// either a parabola or a hyperbola). A parabolic segment with the start point
// at infinity contains all points to the left of the end point, as seen from
// the focal point of the parabola. If the end point lies at infinity, it's the
// points to the left of the start point which form the segment.
// A hyperbolic segment for which the two endpoints are the two different points
// at infinity of the hyperbola, contains all points of one of the two hyperbola
// branches and none of the other branch. This is the only ambiguous situation
// so it should be avoided unless a third point is used to define the segment.
// If only one end point of a hyperbolic segment lies at infinity, the segment
// either consists of just the points to the left on the branch of the other end
// point, or of those points plus all points on the other branch.
//
// \verbatim
//  Modifications
//   2009-06-06 Peter Vanroose - Added member "counterclockwise_"
//   2009-06-06 Peter Vanroose - Added swap_endpoints(), swap_direction(), normalize()
//   2009-06-06 Peter Vanroose - Added the is_finite() method (not yet implem.)
//   2009-06-06 Peter Vanroose - Re-implemented to be fully homogeneous
//   2009-06-06 Peter Vanroose - Added explicit "semantics" documentation
//   2009-06-06 Peter Vanroose - Added the contains() method (not yet implem.)
// \endverbatim

#include <iosfwd>
#include <vgl/vgl_homg_point_2d.h> // data member of this class
#include <vgl/vgl_conic.h>         // data member of this class
#include <vgl/vgl_point_2d.h>      // return type of some methods
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Represents a 2D conic segment using two points.
template <class Type>
class vgl_conic_segment_2d
{
  //: One end of conic segment
  vgl_homg_point_2d<Type> p1_;

  //: The other end of the conic segment
  vgl_homg_point_2d<Type> p2_;

  //: The conic that represents the curve between point1 and point2
  vgl_conic<Type> conic_;

  //: Whether traversal is in counter-clockwise direction (the default) or not
  bool counterclockwise_;

 public:
  //: Default constructor - does not initialise!
  //  Use the set() method to make this conic segment useful.
  inline vgl_conic_segment_2d() = default;

  //: Copy constructor
  inline vgl_conic_segment_2d(vgl_conic_segment_2d<Type> const& l)
  : p1_(l.p1_), p2_(l.p2_), conic_(l.conic_),
    counterclockwise_(l.counterclockwise_) {}

  //: Construct from two end points (homogeneous) and a conic
  inline vgl_conic_segment_2d(vgl_homg_point_2d<Type> const& p1,
                              vgl_homg_point_2d<Type> const& p2,
                              vgl_conic<Type> const& co,
                              bool counterclockwise = true)
  : p1_(p1), p2_(p2), conic_(co),
    counterclockwise_(counterclockwise) {}

  //: Construct from two end points (Cartesian) and a conic
  inline vgl_conic_segment_2d(vgl_point_2d<Type> const& p1,
                              vgl_point_2d<Type> const& p2,
                              vgl_conic<Type> const& co,
                              bool counterclockwise = true)
  : p1_(p1.x(), p1.y(), (Type)1), p2_(p2.x(), p2.y(), (Type)1),
    conic_(co), counterclockwise_(counterclockwise) {}

  //: Construct from a conic and two end points (homogeneous)
  inline vgl_conic_segment_2d(vgl_conic<Type> const& co,
                              vgl_homg_point_2d<Type> const& p1,
                              vgl_homg_point_2d<Type> const& p2,
                              bool counterclockwise = true)
  : p1_(p1), p2_(p2), conic_(co),
    counterclockwise_(counterclockwise) {}

  //: Construct from a conic and two end points (Cartesian)
  inline vgl_conic_segment_2d(vgl_conic<Type> const& co,
                              vgl_point_2d<Type> const& p1,
                              vgl_point_2d<Type> const& p2,
                              bool counterclockwise = true)
  : p1_(p1.x(), p1.y(), (Type)1), p2_(p2.x(), p2.y(), (Type)1),
    conic_(co), counterclockwise_(counterclockwise) {}

  //: Destructor
  inline ~vgl_conic_segment_2d() = default;

  //: Normalise the direction of the segment to counterclockwise.
  //  This will also swap the end points if the direction is to be swapped.
  void normalize() { if (!counterclockwise_) { counterclockwise_=true; swap_endpoints(); } }

  //: Interchange the two endpoints but keep the direction.
  //  This implies that now the conic segment contains those points of the conic
  //  which before did not belong to the conic segment! (Except for the two end
  //  points, of course.)
  void swap_endpoints() { vgl_homg_point_2d<Type> p=p1_; p1_=p2_; p2_=p; }

  //: Change the direction of the conic section but keep the end points.
  //  This implies that now the conic segment contains those points of the conic
  //  which before did not belong to the conic segment! (Except for the two end
  //  points, of course.)
  void swap_direction() { counterclockwise_ = !counterclockwise_; }

  //: The first end-point of the conic segment.
  inline vgl_homg_point_2d<Type> point1() const { return p1_; } // return a copy

  //: The second end-point of the conic segment.
  inline vgl_homg_point_2d<Type> point2() const { return p2_; } // return a copy

  //: The conic underlying the segment
  inline vgl_conic<Type> conic() const { return conic_; } // return a copy

  //: The direction of the segment (clockwise or counterclockwise)
  bool is_counterclockwise() const { return counterclockwise_; }

  //: The direction of the segment (clockwise or counterclockwise)
  bool is_clockwise() const { return !counterclockwise_; }

  //: The equality comparison operator
  //  Two conic segments are only identical if the underlying conic is identical
  //  and if direction and  both endpoints are identical, in the same order!
  //  Two conic segments with identical conic and identical end points but
  //  in the opposite order are not identical but rather complementary: they
  //  share no other points than the two end points.
  //  Use the swap_direction or the swap_endpoints() method on one of the two
  //  segments to turn complementary segments into identical ones.
  //  Note that two conic segments \e are equal if both the direction and the
  //  two end points are swapped. To normalize a conic segment such that its
  //  direction becomes counterclockwise, use the normalize() method.
  inline bool operator==(vgl_conic_segment_2d<Type> const& l) const {
    return this==&l ||
           (l.conic() == conic_ &&
            l.is_counterclockwise() == counterclockwise_ &&
            point1() == l.point1() &&
            point2() == l.point2()) ||
           (l.conic() == conic_ &&
            l.is_counterclockwise() != counterclockwise_ &&
            point2() == l.point1() &&
            point1() == l.point2());
  }

  //: The inequality comparison operator.
  inline bool operator!=(vgl_conic_segment_2d<Type>const& other) const { return !operator==(other); }

  //: (Re)initialise the conic segment by passing it its three "constructors"
  inline void set(vgl_homg_point_2d<Type> const& p1, vgl_homg_point_2d<Type> const& p2,
                  vgl_conic<Type> co, bool counterclockwise = true)
  { p1_ = p1; p2_ = p2; conic_ = co; counterclockwise_ = counterclockwise; }

  //: (Re)initialise the conic segment by passing it its three "constructors"
  inline void set(vgl_conic<Type> co,
                  vgl_homg_point_2d<Type> const& p1, vgl_homg_point_2d<Type> const& p2,
                  bool counterclockwise = true)
  { p1_ = p1; p2_ = p2; conic_ = co; counterclockwise_ = counterclockwise; }

  //: (Re)initialise the conic segment by passing it its three "constructors"
  inline void set(vgl_point_2d<Type> const& p1, vgl_point_2d<Type> const& p2,
                  vgl_conic<Type> co, bool counterclockwise = true)
  { p1_.set(p1.x(), p1.y()); p2_.set(p2.x(), p2.y()); conic_ = co;
    counterclockwise_ = counterclockwise; }

  //: (Re)initialise the conic segment by passing it its three "constructors"
  inline void set(vgl_conic<Type> co,
                  vgl_point_2d<Type> const& p1, vgl_point_2d<Type> const& p2,
                  bool counterclockwise = true)
  { p1_.set(p1.x(), p1.y()); p2_.set(p2.x(), p2.y()); conic_ = co;
    counterclockwise_ = counterclockwise; }

  //: Finds out whether this curve has a finite length.
  //  If the conic segment has an underlying ellipse, the segment is of course
  //  always finite. Otherwise, is_finite returns false whenever the segment
  //  passes through one of the points at infinity of the hyperbola or parabola.
  //  Note that the methods swap_endpoints() and swap_direction() always swaps
  //  finiteness of a parabolic segment (unless the endpoints coincide). For
  //  hyperbolic segments this is not necessarily the case: both can be infinite.
  //  \todo not yet implemented
  bool is_finite() const { assert(!"Not yet implemented"); return true; }

  //: Finds out whether the given point lies on the conic segment.
  //  More specifically, lying on the segment implies lying on the conic.
  //  Moreover, the two endpoints (if effectively on the conic) will always
  //  lie on the segment. All other points of the conic lie either on this
  //  segment, or on the "swapped" segment, but never on both.
  //  \todo not yet implemented
  bool contains(vgl_homg_point_2d<Type> const& /*pt*/) const { assert(!"Not yet implemented"); return false; }
};

//: Write to stream
// \relatesalso vgl_conic_segment_2d
template <class Type>
std::ostream&  operator<<(std::ostream& s, const vgl_conic_segment_2d<Type>& c_s);

//: Read from stream
// \relatesalso vgl_conic_segment_2d
template <class Type>
std::istream& operator>>(std::istream& is, vgl_conic_segment_2d<Type>& c_s);

#define VGL_CONIC_SEGMENT_2D_INSTANTIATE(T) extern "please include vgl/vgl_conic_segment_2d.hxx first"

#endif // vgl_conic_segment_2d_h_
