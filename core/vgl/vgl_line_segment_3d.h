//-*- c++ -*-------------------------------------------------------------------
#ifndef vgl_line_segment_3d_h_
#define vgl_line_segment_3d_h_
#ifdef __GNUC__
#pragma interface
#endif
// .SECTION Author
//    awf@robots.ox.ac.uk
// Created: 19 Mar 00
// Created: Feb 15 2000

#include <vcl/vcl_iosfwd.h>

#include <vgl/vgl_point_3d.h>

//: Represents a 3D line segment using two points.
template <class Type>
class vgl_line_segment_3d {
public:
  
  vgl_line_segment_3d() {}
  vgl_line_segment_3d(vgl_line_segment_3d<Type> const& that);
  vgl_line_segment_3d(vgl_point_3d<Type> const &, vgl_point_3d<Type> const &);
 ~vgl_line_segment_3d();

  vgl_point_3d<Type> const & get_point1() const { return point1_; }
  vgl_point_3d<Type>       & get_point1() { return point1_; }
  vgl_point_3d<Type> const & get_point2() const { return point2_; } 
  vgl_point_3d<Type>       & get_point2() { return point2_; }
  
private:
  vgl_point_3d<Type> point1_;
  vgl_point_3d<Type> point2_;
};

// stream operators 
template <class Type> 
ostream&  operator<<(ostream& s, const vgl_line_segment_3d<Type>& p);

template <class Type>
istream&  operator>>(istream& is,  vgl_line_segment_3d<Type>& p);

#endif 
