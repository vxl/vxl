// This is core/vgl/vgl_line_segment_2d.h
#ifndef vgl_line_segment_2d_h_
#define vgl_line_segment_2d_h_
//:
// \file
// \author mccane@cs.otago.ac.nz: but copied from vgl_line_segment_3d
//
// \verbatim
// Modifications
// Peter Vanroose -  9 July 2001 - Inlined constructors
// Peter Vanroose - 27 June 2001 - Added operator==
// J.L. Mundy     - 13 April 2003 - Added angle and line coefficient functions
// \endverbatim

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h> // data member of this class

//: Represents a 2D line segment using two points.
template <class Type>
class vgl_line_segment_2d
{
  //: One end of line segment
  vgl_point_2d<Type> point1_;
  //: The other end of the line segment
  vgl_point_2d<Type> point2_;

 public:
  //: Default constructor - does not initialise!
  inline vgl_line_segment_2d() = default;

  //: Copy constructor
  inline vgl_line_segment_2d(vgl_line_segment_2d<Type> const& l)
    : point1_(l.point1_), point2_(l.point2_) {}

  //: Construct from two end points
  inline vgl_line_segment_2d(vgl_point_2d<Type> const& p1,
                             vgl_point_2d<Type> const& p2)
    : point1_(p1), point2_(p2) {}

  //: Destructor
  inline ~vgl_line_segment_2d() = default;

  //: One end-point of the line segment.
  inline vgl_point_2d<Type> point1() const { return point1_; } // return a copy

  //: The other end-point of the line segment.
  inline vgl_point_2d<Type> point2() const { return point2_; } // return a copy

  //: The equality comparison operator
  inline bool operator==(vgl_line_segment_2d<Type> const& l) const {
    return (this==&l) || (point1() == l.point1() && point2() == l.point2())
                      || (point1() == l.point2() && point2() == l.point1()); }

  //: The inequality comparison operator.
  inline bool operator!=(vgl_line_segment_2d<Type>const& other)const{return !operator==(other);}

  // A consistent interface with vgl_line_2d:

  //: Parameter a of line a*x + b*y + c = 0
  Type a() const;

  //: Parameter b of line a*x + b*y + c = 0
  Type b() const;

  //: Parameter c of line a*x + b*y + c = 0
  Type c() const;

  //: unit vector describing line direction
  vgl_vector_2d<Type> direction() const;

  //: unit vector orthogonal to line
  vgl_vector_2d<Type> normal() const;

  //: angle with the oriented horizontal line y=0, measured in radians.
  //  Returns values between -pi and pi.
  double slope_radians() const;

  //: angle with the oriented horizontal line y=0, measured in 360-degrees.
  //  Returns values between -180 and 180.
  double slope_degrees() const;

  //: Assignment
  inline void set(vgl_point_2d<Type> const& p1, vgl_point_2d<Type> const& p2) {
    point1_ = p1; point2_ = p2; }

  //: Return a point on the line defined by a scalar parameter \a t.
  // \a t=0.0 corresponds to point1 and \a t=1.0 to point2.
  // 0<t<1 for points on the segment between point1 and point2.
  // t<0 for points on the (infinite) line, outside the segment, and closer to point1 than to point2.
  // t>1 for points on the (infinite) line, outside the segment, and closer to point2 than to point1.
  inline vgl_point_2d<Type> point_t(const double t) const { return point1() + t*(point2_-point1_); }
};

//: Write to stream
// \relatesalso vgl_line_segment_2d
template <class Type>
std::ostream&  operator<<(std::ostream& s, const vgl_line_segment_2d<Type>& p);

//: Read from stream
// \relatesalso vgl_line_segment_2d
template <class Type>
std::istream&  operator>>(std::istream& is,  vgl_line_segment_2d<Type>& p);

#define VGL_LINE_SEGMENT_2D_INSTANTIATE(T) extern "please include vgl/vgl_line_segment_2d.hxx first"

#endif // vgl_line_segment_2d_h_
