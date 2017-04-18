// This is core/vgl/algo/vgl_orient_box.h
#ifndef vgl_orient_box_2d_h_
#define vgl_orient_box_2d_h_

#include <iostream>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_point_2d.h>
#include <vector>
#include <map>
//:
// \file
// \brief An oriented box in 2-d
// \author J.L. Mundy
// \date April 18, 2017
//

template <class T>
class vgl_orient_box_2d {
 public:
  //:default constructor
 vgl_orient_box_2d():min_v_(T(1)), max_v_(-T(1)){}

  //:construct from center, width (major axis), height (minor axis)
  vgl_orient_box_2d(T width, T height, vgl_point_2d<T> const& center);

  //:construct from major minor axis endpoints
  // the orientation of the box is defined by the major axis
  // the minor axis does not have to be perpendicular to the major axis
  // the minor bounds(v) of the box are defined by the projection of the minor axis
  // onto the perpendular vector to the major axis
  vgl_orient_box_2d(vgl_point_2d<T> const& maj_p1, vgl_point_2d<T> const& maj_p2,
                    vgl_point_2d<T> const& min_p1, vgl_point_2d<T> const& min_p2);

  vgl_orient_box_2d(vgl_line_segment_2d<T> const& maj,
                    vgl_line_segment_2d<T> const& min);

  vgl_orient_box_2d(const vgl_box_2d<T>& box);

  //: major minor axes
  vgl_line_segment_2d<T> major_axis() const{ return major_axis_;}
  vgl_line_segment_2d<T> minor_axis() const{ return minor_axis_;}

  //: center (midpoint of major axis)
  vgl_point_2d<T> centroid() const;

  //: return width (first) and height (second)
 std::pair<T, T> width_height() const;

  //: width - length of major axis
  T width() const {std::pair<T, T> p = this->width_height(); return p.first;}

  //: height - length of minor axis
  T height() const {std::pair<T, T> p = this->width_height(); return p.second;}
  
  //::area
  T area() {std::pair<T, T> p = this->width_height(); return (p.first)*(p.second);}
  
  //: Return true if (x,y) is inside this box
  bool contains(T const& x, T const& y) const;

  //: Return true if point is inside this box
  bool contains(vgl_point_2d<T> const& p) const {return contains(p.x(), p.y());}

  //: bounds on v (can be asymmetric)
  std::pair<T, T> v_bounds() const {return std::pair<T, T>(min_v_, max_v_);}

  void set(vgl_line_segment_2d<T> const& major, vgl_line_segment_2d<T> const& minor){
    major_axis_ = major;
    minor_axis_ = minor;
  }

  //: axis-aligned bounding box for *this
  vgl_box_2d<T> enclosing_box() const;

  //: corners of the oriented rectangle
  std::vector<vgl_point_2d<T> > corners() const;

  //: map a point to the (u, v) coordinate system of the oriented box
  vgl_point_2d<T> transform_to_obox(vgl_point_2d<T> const& p) const;

    private:
  vgl_line_segment_2d<T> major_axis_;
  vgl_line_segment_2d<T> minor_axis_;
  T min_v_;
  T max_v_;
};
template <class T>
std::ostream&  operator<<(std::ostream& os, const vgl_orient_box_2d<T>& obox);
template <class T>
std::istream&  operator>>(std::istream& is,  vgl_orient_box_2d<T>& obox);

#define VGL_ORIENT_BOX_2D_INSTANTIATE(T) extern "please include vgl/vgl_point_2d.hxx first"

#endif // vgl_orient_box_2d_h_
