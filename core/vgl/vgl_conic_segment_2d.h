// This is core/vgl/vgl_conic_segment_2d.h
#ifndef vgl_conic_segment_2d_h_
#define vgl_conic_segment_2d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author J.L. Mundy  June 18, 2005
// \brief A curve segment with the geometry of an conic
// \verbatim
// Modifications - none
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_point_2d.h> // data members of this class
#include <vgl/vgl_conic.h>

//: Represents a 2D conic segment using two points.
template <class Type>
class vgl_conic_segment_2d
{
  //: One end of conic segment
  vgl_point_2d<Type> point1_;

  //: The other end of the conic segment
  vgl_point_2d<Type> point2_;

  //: The conic that represents the curve between point1 and point2
  vgl_conic<Type> conic_;

 public:
  //: Default constructor - does not initialise!
  inline vgl_conic_segment_2d() {}

  //: Copy constructor
  inline vgl_conic_segment_2d(vgl_conic_segment_2d<Type> const& l)
    : point1_(l.point1_), point2_(l.point2_), conic_(l.conic_) {}

  //: Construct from two end points and an conic
  inline vgl_conic_segment_2d(vgl_point_2d<Type> const& p1,
                                vgl_point_2d<Type> const& p2,
                                vgl_conic<Type> const& conic)
    : point1_(p1), point2_(p2), conic_(conic) {}

  //: Destructor
  inline ~vgl_conic_segment_2d() {}

  //: One end-point of the conic segment.
  inline vgl_point_2d<Type> point1() const { return point1_; } // return a copy

  //: The other end-point of the conic segment.
  inline vgl_point_2d<Type> point2() const { return point2_; } // return a copy

  //: The conic representing the segment
  inline vgl_conic<Type> conic() const { return conic_; } // return a copy

  //: The equality comparison operator
  inline bool operator==(vgl_conic_segment_2d<Type> const& l) const {
    return (this==&l) ||
			((l.conic() == conic_) &&
                          (point1() == l.point1() && point2() == l.point2()) ||
                          (point1() == l.point2() && point2() == l.point1()));}

  //: The inequality comparison operator.
  inline bool operator!=(vgl_conic_segment_2d<Type>const& other)const{return !operator==(other);}

  inline void set(vgl_point_2d<Type> const& p1, vgl_point_2d<Type> const& p2,
                  vgl_conic<Type> conic) {
    point1_ = p1; point2_ = p2; conic_ = conic; }
};

//: Write to stream
// \relates vgl_conic_segment_2d
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_conic_segment_2d<Type>& p);

//: Read from stream
// \relates vgl_conic_segment_2d
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_conic_segment_2d<Type>& p);

#define VGL_CONIC_SEGMENT_2D_INSTANTIATE(T) extern "please include vgl/vgl_conic_segment_2d.txx first"

#endif // vgl_conic_segment_2d_h_
