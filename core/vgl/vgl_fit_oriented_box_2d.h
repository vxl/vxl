// This is core/vgl/vgl_fit_oriented_box_2d.h
#ifndef vgl_fit_oriented_box_2d_h_
#define vgl_fit_oriented_box_2d_h_

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_oriented_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_tolerance.h>
//:
// \file
// \brief Fit an oriented box to a set of 2d points
// \author J.L. Mundy
// \date November 12, 2017
//
// a brute force algorithm: try all angles with granularity dtheta return obox with minimum area
#define default_dtheta 0.0174524 // one degree
template <class T>
class vgl_fit_oriented_box_2d {
 public:
  //:default constructor
 vgl_fit_oriented_box_2d():fixed_theta_(false), fit_valid_(false), dtheta_(default_dtheta){}

  //: constructor with polygon
  vgl_fit_oriented_box_2d(vgl_polygon<T> const& poly, double dtheta = default_dtheta);

  //: constructor with points
 vgl_fit_oriented_box_2d(std::vector<vgl_point_2d<T> >  pts, double dtheta = default_dtheta):
  fixed_theta_(false), fit_valid_(false), dtheta_(dtheta), pts_(std::move(pts)){}

  //: brute force search
  vgl_oriented_box_2d<T> fitted_box();

  //: fit box with specified oriention
  // theta is defined ccw with respect to the x-axis
  vgl_oriented_box_2d<T> fitted_box(double theta_rad){
    if(!fit_valid_){
      fixed_theta_ = true;
      fit_obox(theta_rad);
      fit_valid_ = true;
    }
    return obox_;
  }

 private:
  bool fixed_theta_;
  void fit_obox(T theta_rad = T(0));
  bool fit_valid_;
  double dtheta_;
  std::vector<vgl_point_2d<T> > pts_;
  vgl_oriented_box_2d<T> obox_;
};

#define VGL_FIT_ORIENTED_BOX_2D_INSTANTIATE(T) extern "please include vgl/algo/vgl_fit_oriented_box_2d.hxx first"

#endif // vgl_fit_oriented_box_2d_h_
