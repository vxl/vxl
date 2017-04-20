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
 vgl_orient_box_2d():half_height_(T(0)){}

  //:construct from center, width (major axis), height and orientation with respect to x-axis
  // rotation is around the center point. width must be greater than height!!
  vgl_orient_box_2d(T width, T height, vgl_point_2d<T> const& center, T angle_in_rad);

  //:construct from major axis and height
  // the center is the midpoint of the major axis
  vgl_orient_box_2d(vgl_point_2d<T> const& maj_p1, vgl_point_2d<T> const& maj_p2, T height){
    major_axis_.set(maj_p1, maj_p2); half_height_ = height/T(2);}

  vgl_orient_box_2d(vgl_line_segment_2d<T> const& maj, T height){ major_axis_ = maj; half_height_ = height/T(2);}

  //: construct from an axis aligned box
  vgl_orient_box_2d(const vgl_box_2d<T>& box);

  //: construct from an axis aligned box with orientation 
  // the major axis of the box is rotated by angle_in_rad around the centroid
  vgl_orient_box_2d(const vgl_box_2d<T>& box, T angle_in_rad);

  //: constructor from three corner points.
  //  The two directions from p0 to the two other points must be
  //  mutually orthogonal, i.e., a local box coordinate frame centered on p0
  vgl_orient_box_2d(vgl_point_2d<T> const& p0, vgl_point_2d<T> const& p1, vgl_point_2d<T> const& p2);

  //: major axis
  vgl_line_segment_2d<T> major_axis() const{ return major_axis_;}

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
  
  bool operator==(vgl_orient_box_2d<T> const& ob) const {
    return (this == &ob) || (ob.major_axis_ == this->major_axis_ && ob.half_height_ == this->half_height_);
  }

  //: Return true if (x,y) is inside this box
  bool contains(T const& x, T const& y) const;

  //: Return true if point is inside this box
  bool contains(vgl_point_2d<T> const& p) const {return contains(p.x(), p.y());}


  void set(vgl_line_segment_2d<T> const& major, T half_height){
    major_axis_ = major;
    half_height_ = half_height;
  }

  //: axis-aligned bounding box for *this
  vgl_box_2d<T> enclosing_box() const;

  //: corners of the oriented rectangle
  std::vector<vgl_point_2d<T> > corners() const;

  //: map a point to the (u, v) coordinate system of the oriented box
  vgl_point_2d<T> transform_to_obox(vgl_point_2d<T> const& p) const;

    private:
  vgl_line_segment_2d<T> major_axis_;
  T half_height_;
};
template <class T>
std::ostream&  operator<<(std::ostream& os, const vgl_orient_box_2d<T>& obox);
template <class T>
std::istream&  operator>>(std::istream& is,  vgl_orient_box_2d<T>& obox);

#define VGL_ORIENT_BOX_2D_INSTANTIATE(T) extern "please include vgl/vgl_point_2d.hxx first"

#endif // vgl_orient_box_2d_h_
