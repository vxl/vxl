// This is vgl/vgl_oriented_box_2d.h
#ifndef vgl_oriented_box_2d_h_
#define vgl_oriented_box_2d_h_

#include <iostream>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vector>
#include <map>
//:
// \file
// \brief A 2-d oriented box
// \author J.L. Mundy
// \date 10 December 2016
//

// public functions
template <class T>
class vgl_oriented_box_2d {
 public:
  //:default constructor
 vgl_oriented_box_2d(){}

  //:construct from center, length, width
  vgl_oriented_box_2d(long length, long width, vgl_point_2d<T> center);

  //:construct from major minor axis endpoints
  vgl_oriented_box_2d(vgl_point_2d<T> const& maj_p1,vgl_point_2d<T> const& maj_p2,
                      vgl_point_2d<T> const& min_p1,vgl_point_2d<T> const& min_p2){
    major_axis_ = vgl_line_segment_2d<T>(maj_p1, maj_p2);
    minor_axis_ = vgl_line_segment_2d<T>(min_p1, min_p2);
  }
  vgl_oriented_box_2d(vgl_line_segment_2d<T> const& maj,
		       vgl_line_segment_2d<T> const& min): major_axis_(maj), minor_axis_(min){
  }
  //: major minor axes
  vgl_line_segment_2d<T> major_axis() const{ return major_axis_;}
  vgl_line_segment_2d<T> minor_axis() const{ return minor_axis_;}

  //: center (midpoint of major axis)
  vgl_point_2d<T> center() const;

  //: return length (point.x()) and width (point.y())
  vgl_point_2d<T> length_width() const;

  //: line segment perpendicular to major axis at normalized major axis position u
  //  where u is in (-0.5, 0.5);
  vgl_line_segment_2d<T> perp_line_seg(double u) const;

  //: line segment parallel to the major axis at normalized minor axis position v
  //  where v is in {-0.5, 0.5)
  vgl_line_segment_2d<T> para_line_seg(double v) const;

  //: absolute grid locations in inside the oriented box at normalized spacings du, dv.
  // the points are in the range (-du*u_radius, du*u_radius), (-dv*v_radius, dv*v_radius)
  std::vector<vgl_point_2d<T> > grid_locations(double du, double dv, int u_radius, int v_radius) const;

  void set(vgl_line_segment_2d<T> const& major, vgl_line_segment_2d<T> const& minor){
    major_axis_ = major;
    minor_axis_ = minor;
  }

  /*static double slope_radians(vgl_line_segment_2d<T> const& seg);*/

  //:rotate *this so that major axis is oriented at 0 or 180 degrees (x axis)
  // if the angle is greater than +90 or less than -90 then rotate to
  // the -x axis (180 degrees). Return the resulting rotation angle.
  T align_major_axis(vgl_point_2d<T> const& rot_center, vgl_oriented_box_2d<T>& rot_box) const;

  //:rotate *this by angle in radians
  vgl_oriented_box_2d<T> rotate(vgl_point_2d<T> const& rot_center,T angle) const;

  //:translate *this by displacement (tx, ty)
  vgl_oriented_box_2d<T> translate(T tx, T ty) const;

  //: axis-aligned bounding box for *this
  vgl_box_2d<T> bounding_box() const;

  //: corners of the oriented rectangle
  std::vector<vgl_point_2d<T> > corners() const;

  inline bool operator==(vgl_oriented_box_2d<T> const& obox) const {
    return (this==&obox) || (major_axis_ == obox.major_axis()&&(minor_axis_ == obox.minor_axis())); }

  //: write a compact ascii form (4 pts)
  void write(std::ostream& os) const;
  //: read a compact ascii form (4 pts)
  void read(std::istream& is);
 private:
  vgl_line_segment_2d<T> major_axis_;
  vgl_line_segment_2d<T> minor_axis_;
};
//: the intersection area over the union area of two oriented boxes (a measure of similarity)
template <class T>
T obox_int_over_union(vgl_oriented_box_2d<T> const& ob0, vgl_oriented_box_2d<T> const& ob1);

//: oriented bounding box from axis-aligned rectangle
template <class T>
vgl_oriented_box_2d<T> obox_from_rect(vgl_box_2d<T> rect);

//: translate an oriented box
template <class T>
vgl_oriented_box_2d<T> translate_obox(vgl_oriented_box_2d<T> const& obox,
                                      vgl_vector_2d<T> const& trans);
//: transform a 2-d point into the coordinate system of the oriented box
template <class T>
vgl_point_2d<T> transform_to_obox(vgl_oriented_box_2d<T> const& obox, vgl_point_2d<T> const& p);

template <class T>
std::ostream&  operator<<(std::ostream& os, const vgl_oriented_box_2d<T>& obox);

template <class T>
std::istream&  operator>>(std::istream& is,  vgl_oriented_box_2d<T>& obox);

#define VGL_ORIENTED_BOX_2D_INSTANTIATE(T) extern "please include vgl/vgl_oriented_box_2d.hxx first"
#endif // vgl_oriented_box_2d_h_
