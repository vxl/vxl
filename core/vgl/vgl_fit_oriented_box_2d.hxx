#include <limits>
#include "vgl_fit_oriented_box_2d.h"
#include <vnl/vnl_math.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_area.h>

template <class T>
vgl_fit_oriented_box_2d<T>::vgl_fit_oriented_box_2d(vgl_polygon<T> const& poly, double dtheta):fixed_theta_(false), fit_valid_(false), dtheta_(dtheta){
  // extract points
  pts_.clear();
  size_t ns = poly.num_sheets();
  for(size_t is = 0; is<ns; ++is)
    for(size_t i = 0; i<poly[is].size(); ++i)
      pts_.push_back(poly[is][i]);
}
template <class T>
void vgl_fit_oriented_box_2d<T>::fit_obox(T theta_rad){

  vgl_oriented_box_2d<T> min_area_obox;
  T min_area = std::numeric_limits<T>::max();
  size_t n = pts_.size();
  if(n<2){
        std::cout << "insufficient number of points to fit obox " << n << std::endl;
        return;
  }
  vgl_point_2d<T>& vs = pts_[0];
  // +- 90 degrees
  T T_pi_2 = static_cast<T>(vnl_math::pi_over_2);
  T low_theta = -T_pi_2, high_theta = T_pi_2;
  if(fixed_theta_){
    //theta rotates the poly edge to align with the x axis
    //thus theta is negative the orientation of the box
    low_theta = -theta_rad;
    high_theta = -theta_rad;
  }
  T min_theta = T(0);
  for(T theta = low_theta; theta <= high_theta; theta += dtheta_){
    T c = cos(theta), s = sin(theta);
    vgl_box_2d<T> box;
    for(size_t j = 0; j<n; ++j){
      vgl_vector_2d<T> vp = pts_[j]-vs;
      vgl_point_2d<T> rpp((c*vp.x()-s*vp.y()),(s*vp.x() + c*vp.y()));
      box.add(rpp);
    }
    T area = vgl_area(box);
    if(area < min_area){
      min_area = area;
      min_theta = theta;
      T w = box.width(), h = box.height();
    vgl_point_2d<T> cent = box.centroid();
    //select major axis such that width > height
    T width = w, height = h;
    vgl_point_2d<T> pmaj0(cent.x()-width/2.0, cent.y()), pmaj1(cent.x()+width/2.0, cent.y());
    if(w<h){
      width = h;
      height = w;
      pmaj0.set(cent.x(), cent.y()-width/2.0); pmaj1.set(cent.x(), cent.y()+width/2.0);
    }
    // rotate major axis about vs
    c = cos(-theta); s = sin(-theta);
    T pmaj0x = pmaj0.x(), pmaj0y = pmaj0.y();
    T pmaj1x = pmaj1.x(), pmaj1y = pmaj1.y();
    vgl_point_2d<T> pmaj0r(c*pmaj0x - s*pmaj0y, s*pmaj0x + c*pmaj0y);
    vgl_point_2d<T> pmaj1r(c*pmaj1x - s*pmaj1y, s*pmaj1x + c*pmaj1y);

    // add back rotation center
    vgl_vector_2d<T> offset(vs.x(), vs.y());
    pmaj0r += offset; pmaj1r += offset;
    min_area_obox = vgl_oriented_box_2d<T>(pmaj0r, pmaj1r, height);
    }
  }
  obox_ = min_area_obox;
}

template <class T>
vgl_oriented_box_2d<T> vgl_fit_oriented_box_2d<T>::fitted_box(){
  fixed_theta_ = false;
  if(!fit_valid_){
    fit_obox();
    fit_valid_ = true;
  }
  return obox_;
}
#undef VGL_FIT_ORIENTED_BOX_2D_INSTANTIATE
#define VGL_FIT_ORIENTED_BOX_2D_INSTANTIATE(T) \
template class vgl_fit_oriented_box_2d<T >
