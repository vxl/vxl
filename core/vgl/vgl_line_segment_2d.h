// vgl/vgl_line_segment_2d.h
//-*- c++ -*-------------------------------------------------------------------
#ifndef vgl_line_segment_2d_h_
#define vgl_line_segment_2d_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgl_line_segment_2d - Represents a 2D line segment using two points
// .LIBRARY vgl
// .INCLUDE vgl/vgl_line_segment_2d.h
// .FILE    vgl_line_segment_2d.txx
// .SECTION Author
//    mccane@cs.otago.ac.nz: but copied from vgl_line_segment_3d
// Created: Dec 5 2000

#include <vcl_iosfwd.h>

#include <vgl/vgl_point_2d.h>

//: Represents a 2D line segment using two points.
template <class Type>
class vgl_line_segment_2d {
public:
  
  vgl_line_segment_2d() {}
  vgl_line_segment_2d(vgl_line_segment_2d<Type> const& that);
  vgl_line_segment_2d(vgl_point_2d<Type> const &, vgl_point_2d<Type> const &);
 ~vgl_line_segment_2d();

  vgl_point_2d<Type> const & get_point1() const { return point1_; }
  vgl_point_2d<Type>       & get_point1() { return point1_; }
  vgl_point_2d<Type> const & get_point2() const { return point2_; } 
  vgl_point_2d<Type>       & get_point2() { return point2_; }
  
private:
  vgl_point_2d<Type> point1_;
  vgl_point_2d<Type> point2_;
};

// stream operators 
template <class Type> 
ostream&  operator<<(ostream& s, const vgl_line_segment_2d<Type>& p);

template <class Type>
istream&  operator>>(istream& is,  vgl_line_segment_2d<Type>& p);

#endif // vgl_line_segment_2d_h_
