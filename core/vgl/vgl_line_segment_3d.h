// This is core/vgl/vgl_line_segment_3d.h
#ifndef vgl_line_segment_3d_h_
#define vgl_line_segment_3d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief line segment in 3D nonhomogeneous space
// \author  awf@robots.ox.ac.uk
//
// \verbatim
// Modifications
// Peter Vanroose -  9 July 2001 - Inlined constructors
// Peter Vanroose - 27 June 2001 - Added operator==
// Kieran O'Mahony - 13 Aug 2007 - Added contains()
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_point_3d.h> // data member of this class

//: Represents a 3D line segment using two points.
template <class Type>
class vgl_line_segment_3d
{
  vgl_point_3d<Type> point1_;
  vgl_point_3d<Type> point2_;

 public:
  //: Default constructor - does not initialise!
  inline vgl_line_segment_3d() {}

  //: Copy constructor
  inline vgl_line_segment_3d(vgl_line_segment_3d<Type> const& l)
    : point1_(l.point1()), point2_(l.point2()) {}

  //: Construct from two end points
  inline vgl_line_segment_3d(vgl_point_3d<Type> const& p1,
                             vgl_point_3d<Type> const& p2)
    : point1_(p1), point2_(p2) {}

  inline ~vgl_line_segment_3d() {}

  inline vgl_point_3d<Type> point1() const { return point1_; } // return a copy
  inline vgl_point_3d<Type> point2() const { return point2_; } // return a copy

  //: the comparison operator
  inline bool operator==(vgl_line_segment_3d<Type> const& l) const {
    return (this==&l) || (point1() == l.point1() && point2() == l.point2())
                      || (point1() == l.point2() && point2() == l.point1()); }

  inline bool operator!=(vgl_line_segment_3d<Type>const& other)const{return !operator==(other);}

  //: assignment
  inline void set(vgl_point_3d<Type> const& p1, vgl_point_3d<Type> const& p2) { point1_ = p1; point2_ = p2; }

  //: Return the direction vector of this line (not normalised)
  inline vgl_vector_3d<Type> direction() const { return point2()-point1(); }
  
  //: Return a point on the line defined by a scalar parameter \a t.
  // \a t=0.0 corresponds to point1 and \a t=1.0 to point2.
  // 0<t<1 for points on the segment between point1 and point2.
  // t<0 for points on the (infinite) line, outside the segment, and closer to point1 than to point2.
  // t>1 for points on the (infinite) line, outside the segment, and closer to point2 than to point1.
  inline vgl_point_3d<Type> point_t(const double t) const { return point1() + t*direction(); }
  
  //: Check if point \a p is on the line segment
  inline bool contains(const vgl_point_3d<Type>& p ) const
  {
    double r = (point1_ - point2_).length() - ( (point1_ - p).length() + (point2_ - p).length() );    
    return r < 1e-8 && r > -1e-8;
  }
};

//: Write to stream
// \relates vgl_line_segment_3d
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_line_segment_3d<Type>& p);

//: Read from stream
// \relates vgl_line_segment_3d
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_line_segment_3d<Type>& p);
#define VGL_LINE_SEGMENT_3D_INSTANTIATE(T) extern "please include vgl/vgl_line_segment_3d.txx first"

#endif // vgl_line_segment_3d_h_
