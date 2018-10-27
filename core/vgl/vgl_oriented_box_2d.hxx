#include <cmath>
#include <iostream>
#include <string>
#include <cassert>
#include "vgl_oriented_box_2d.h"
#include "vgl_closest_point.h"
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_tolerance.h>
template <class T>
vgl_oriented_box_2d<T>::vgl_oriented_box_2d(T width, T height, vgl_point_2d<T> const& center, T angle_in_rad)
{
  if(width<height){
      std::cout << "width must be greater than height!!!" << std::endl;
      half_height_ = T(0);
      return;
  }
  half_height_ = height/T(2);
  T half_width = width/T(2);
  T cx = center.x(), cy = center.y();
  T c = cos(angle_in_rad), s = sin(angle_in_rad);
  // major axis points translated to the origin (subtract center)
  T p0x = - half_width, p1x =  half_width;
  T p0rx = c*p0x , p0ry = s*p0x ;
  T p1rx = c*p1x , p1ry = s*p1x ;
  // add center back in
  p0rx += cx; p1rx += cx;
  p0ry += cy; p1ry += cy;
  vgl_point_2d<T> p0r(p0rx, p0ry), p1r(p1rx, p1ry);
  major_axis_.set(p0r, p1r);
}

template <class T>
vgl_oriented_box_2d<T>::vgl_oriented_box_2d(const vgl_box_2d<T>& box){
  *this = vgl_oriented_box_2d<T>(box, T(0));
}

template <class T>
vgl_oriented_box_2d<T>::vgl_oriented_box_2d(const vgl_box_2d<T>& box, T angle_in_rad){
  T w = box.width(), h = box.height();
  T width = w, height = h;
  T ang = angle_in_rad;
  if(w<h){
    width = h;
    height = w;
    ang += T(3.1415926535897932384626433832795)/T(2);
  }
  *this = vgl_oriented_box_2d<T>(width, height, box.centroid(), ang);
}

template <class T>
vgl_oriented_box_2d<T>::vgl_oriented_box_2d(vgl_point_2d<T> const& p0, vgl_point_2d<T> const& p1, vgl_point_2d<T> const& p2){
  vgl_vector_2d<T> v01 = p1-p0, v02 = p2-p0;
  T len_01 = v01.length(), len_02 = v02.length();
  if(len_01>len_02){
    v02/=len_02;
    half_height_ = len_02/T(2);
    vgl_point_2d<T> p0p = p0 + half_height_*v02;
    vgl_point_2d<T> p1p = p1 + half_height_*v02;
    major_axis_.set( p0p, p1p);
    return;
  }
    v01/=len_01;
    half_height_ = len_01/T(2);
    vgl_point_2d<T> p0p = p0 + half_height_*v01;
    vgl_point_2d<T> p2p = p2 + half_height_*v01;
    major_axis_.set( p0p, p2p);
}

template <class T>
vgl_oriented_box_2d<T>::vgl_oriented_box_2d(vgl_point_2d<T> const& maj_p1, vgl_point_2d<T> const& maj_p2,
                                            vgl_point_2d<T> const& min_p1, vgl_point_2d<T> const& min_p2){
  vgl_vector_2d<T> dir = maj_p2 - maj_p1;
  assert(dir.length()>T(0));
  vgl_vector_2d<T> perp(-dir.y(), dir.x());
  perp /= perp.length();
  vgl_vector_2d<T> min_dir = min_p2 - min_p1;
  assert(min_dir.length()>T(0));
  half_height_ = fabs(dot_product(min_dir, perp))/T(2);
  // construct the perpendicular through maj_p1
  vgl_line_2d<T> pline(maj_p1, perp);
  vgl_point_2d<T> cp1 = vgl_closest_point(pline, min_p1);
  vgl_point_2d<T> cp2 = vgl_closest_point(pline, min_p2);
  vgl_point_2d<T> maj_p1_c((cp1.x()+cp2.x())/T(2), (cp1.y()+cp2.y())/T(2));
  vgl_point_2d<T> maj_p2_c = maj_p1_c + dir;
  major_axis_.set(maj_p1_c, maj_p2_c);
}

// axis-aligned bounding box
template <class T>
vgl_box_2d<T> vgl_oriented_box_2d<T>::enclosing_box() const{
  vgl_box_2d<T> ret;
  std::vector<vgl_point_2d<T> > corns = this->corners();
  for(typename std::vector<vgl_point_2d<T> >::iterator cit = corns.begin();
      cit != corns.end(); ++cit)
    ret.add(*cit);
  return ret;
}
template <class T>
std::pair<T, T> vgl_oriented_box_2d<T>::width_height() const{
  vgl_vector_2d<T> v = major_axis_.point2() - major_axis_.point1();
  T len = v.length();
  std::pair<T, T> ret(len, T(2)*half_height_);
  return ret;
}

template <class T>
std::vector<vgl_point_2d<T> > vgl_oriented_box_2d<T>::corners() const{
  const vgl_point_2d<T>& pmaj0 = major_axis_.point1();
  const vgl_point_2d<T>& pmaj1 = major_axis_.point2();
  vgl_vector_2d<T> v = pmaj1-pmaj0;
  T len = v.length();
  v/=len;
  //perpendicular vector
  vgl_vector_2d<T> perp(-v.y(), v.x());
  // generate the corner points in counter-clockwise order
  vgl_point_2d<T> p0 = pmaj0 - half_height_*perp;
  vgl_point_2d<T> p1 = pmaj1 - half_height_*perp;
  vgl_point_2d<T> p2 = pmaj1 + half_height_*perp;
  vgl_point_2d<T> p3 = pmaj0 + half_height_*perp;
  std::vector<vgl_point_2d<T> > corns;
  corns.push_back(p0);  corns.push_back(p1);
  corns.push_back(p2);  corns.push_back(p3);
  return corns;
}
template <class T>
vgl_point_2d<T> vgl_oriented_box_2d<T>::centroid() const{
  const vgl_point_2d<T>& pmaj0 = major_axis_.point1();
  const vgl_point_2d<T>& pmaj1 = major_axis_.point2();
  vgl_point_2d<T> c((pmaj0.x() + pmaj1.x())/T(2),(pmaj0.y() + pmaj1.y())/T(2));
  return c;
}
template <class T>
std::ostream&  operator<<(std::ostream& os, const vgl_oriented_box_2d<T>& obox){
  os << "major axis: " << obox.major_axis() << " half_height: " << obox.height()/T(2)<< "\n";
  return os;
}
template <class T>
std::istream&  operator>>(std::istream& is,  vgl_oriented_box_2d<T>& obox){
  //is.skipws;
  vgl_line_segment_2d<T> maj;
  T half_height = T(0);
  std::string temp;
  is >> temp >> temp;
  is >>  maj >> temp >>  half_height;
  obox.set(maj, half_height);
  return is;
}
template <class T>
vgl_point_2d<T> vgl_oriented_box_2d<T>::transform_to_obox(vgl_point_2d<T> const& p) const{
  vgl_point_2d<T> c = this->centroid();
  vgl_vector_2d<T> vp = p-c;
  vgl_vector_2d<T> dir = major_axis_.direction();
  vgl_vector_2d<T> norm(-dir.y(), dir.x());
  T mag = (major_axis_.point2()-major_axis_.point1()).length();
  // orthogonal obox coordinate vector
  T u = dot_product(dir, vp)/mag;
  T v = dot_product(norm,vp)/(T(2)*half_height_);
  return vgl_point_2d<T>(u, v);
}
template <class T>
bool vgl_oriented_box_2d<T>::contains(vgl_point_2d<T> const& p) const{
  T tol = vgl_tolerance<T>::position;
  vgl_point_2d<T> uv = this->transform_to_obox(p);
  T half = T(1)/T(2);
  half += tol;
  if(uv.x()<-half || uv.x()>half)
    return false;
  if(uv.y()<-half || uv.y()>half)
    return false;
  return true;
}
template <class T>
bool vgl_oriented_box_2d<T>::near_equal(vgl_oriented_box_2d<T> const& ob, T tol) const{
  if(*this == ob)
    return true;
  T ob_half_height = ob.height()/T(2);
  if(fabs(ob_half_height - half_height_) > tol)
    return false;
  const vgl_line_segment_2d<T>& ma = ob.major_axis();
  const vgl_point_2d<T>& obp1 = ma.point1();
  const vgl_point_2d<T>& obp2 = ma.point2();
  const vgl_point_2d<T>& tp1 = major_axis_.point1();
  const vgl_point_2d<T>& tp2 = major_axis_.point2();
  vgl_vector_2d<T> p1p1 = obp1-tp1;
  vgl_vector_2d<T> p2p2 = obp2-tp2;
  if(p1p1.length()<tol && p2p2.length()<tol)
    return true;
  vgl_vector_2d<T> p1p2 = obp1-tp2;
  vgl_vector_2d<T> p2p1 = obp2-tp1;
  if(p1p2.length()<tol && p2p1.length()<tol)
    return true;
  return false;
}
template <class T>
T vgl_oriented_box_2d<T>::angle_in_rad() const{
  const vgl_point_2d<T>& tp1 = major_axis_.point1();
  const vgl_point_2d<T>& tp2 = major_axis_.point2();
  vgl_vector_2d<T> v = tp2-tp1;
  v /= v.length();
  return atan2(v.y(), v.x());
}
template <class T>
static vgl_point_2d<T> rotate_point(vgl_point_2d<T> cent, vgl_point_2d<T> p, T ang_rad){
  vgl_vector_2d<T> v = p-cent;
  T c = cos(ang_rad), s = sin(ang_rad);
  T vrx = v.x()*c - v.y()*s;
  T vry = v.x()*s + v.y()*c;
  vgl_vector_2d<T> pr(vrx, vry);
  return cent + pr;
}

template <class T>
vgl_oriented_box_2d<T> vgl_oriented_box_2d<T>::rotate(vgl_point_2d<T> const& rot_center,T angle_rad) const{
  vgl_point_2d<T> rot_maj_p1 =
    rotate_point(rot_center, major_axis_.point1(), angle_rad);
  vgl_point_2d<T> rot_maj_p2 =
    rotate_point(rot_center, major_axis_.point2(), angle_rad);
  return vgl_oriented_box_2d<T>(rot_maj_p1, rot_maj_p2, T(2)*half_height_);
}
template <class T>
vgl_oriented_box_2d<T> vgl_oriented_box_2d<T>::translate(T tx, T ty) const{
  vgl_vector_2d<T> t(tx, ty);
  vgl_point_2d<T> trans_maj_p1 = major_axis_.point1() + t;
  vgl_point_2d<T> trans_maj_p2 = major_axis_.point2() + t;
  return vgl_oriented_box_2d<T>(trans_maj_p1, trans_maj_p2, T(2)*half_height_);
}

#undef VGL_ORIENTED_BOX_2D_INSTANTIATE
#define VGL_ORIENTED_BOX_2D_INSTANTIATE(T) \
template class vgl_oriented_box_2d<T >; \
template std::ostream& operator<<(std::ostream&, const vgl_oriented_box_2d<T >&); \
template std::istream& operator>>(std::istream&, vgl_oriented_box_2d<T >&)
