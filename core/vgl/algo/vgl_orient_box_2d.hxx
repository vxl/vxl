#include "vgl_orient_box_2d.h"
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <cmath>
template <class T>
vgl_orient_box_2d<T>::vgl_orient_box_2d(T width, T height, vgl_point_2d<T> const& center){
  T w = width/T(2), h = height/T(2);
  T cx = center.x(), cy = center.y();
  vgl_point_2d<T> pmaj0((cx-w), center.y()), pmaj1((cx+w), center.y());
  vgl_point_2d<T> pmin0(center.x(),(cy-h)), pmin1(center.x(),(cy+h));
  major_axis_.set(pmaj0,pmaj1);
  minor_axis_.set(pmin0,pmin1);
  max_v_ = T(1)/T(2);
  min_v_ = -max_v_;
}

template <class T>
vgl_orient_box_2d<T>::vgl_orient_box_2d(const vgl_box_2d<T>& box){
  vgl_point_2d<T> c = box.centroid();
  if(box.width()>box.height()){
    minor_axis_.set(vgl_point_2d<T>(c.x(),box.min_y()), vgl_point_2d<T>(c.x(), box.max_y()));
    major_axis_.set(vgl_point_2d<T>(box.min_x(), c.y()), vgl_point_2d<T>(box.max_y(), c.y()));
    return;
  }
  major_axis_.set(vgl_point_2d<T>(c.x(),box.min_y()), vgl_point_2d<T>(c.x(), box.max_y()));
  minor_axis_.set(vgl_point_2d<T>(box.min_x(), c.y()), vgl_point_2d<T>(box.max_y(), c.y()));
  max_v_ = T(1)/T(2);
  min_v_ = -max_v_;
}
template <class T>
vgl_orient_box_2d<T>::vgl_orient_box_2d(vgl_point_2d<T> const& maj_p1, vgl_point_2d<T> const& maj_p2,
                                        vgl_point_2d<T> const& min_p1, vgl_point_2d<T> const& min_p2){
    major_axis_.set( maj_p1, maj_p2);
    minor_axis_.set(min_p1, min_p2);
    vgl_vector_2d<T> dir = major_axis_.direction();
    vgl_vector_2d<T> norm(-dir.y(), dir.x());
    vgl_point_2d<T> c((maj_p1.x()+maj_p2.x())/T(2), (maj_p1.y()+maj_p2.y())/T(2));
    T v0 = dot_product((min_p1-c),norm), v1 = dot_product((min_p2-c),norm);
    T mag = fabs(dot_product(norm, (min_p2-min_p1)));
	v0/=mag;  v1/=mag;
    min_v_ = v0; max_v_ = v1;
    if(v1<v0){
      min_v_ = v1;
      max_v_ = v0;
    }
}
template <class T>
vgl_orient_box_2d<T>::vgl_orient_box_2d(vgl_line_segment_2d<T> const& maj,
                                        vgl_line_segment_2d<T> const& min){
  *this = vgl_orient_box_2d<T>(maj.point1(), maj.point2(), min.point1(), min.point2());
}
// axis-aligned bounding box
template <class T>
vgl_box_2d<T> vgl_orient_box_2d<T>::enclosing_box() const{
  vgl_box_2d<T> ret;
  std::vector<vgl_point_2d<T> > corns = this->corners();
  for(std::vector<vgl_point_2d<T> >::iterator cit = corns.begin();
      cit != corns.end(); ++cit)
    ret.add(*cit);
  return ret;
}
template <class T>
std::pair<T, T> vgl_orient_box_2d<T>::width_height() const{
  vgl_vector_2d<T> v = major_axis_.point2() - major_axis_.point1();
  T len = v.length();
  v/=len;
  vgl_vector_2d<T> norm(-v.y(), v.x());
  vgl_vector_2d<T> vmin = minor_axis_.point2() - minor_axis_.point1();
  T w = fabs(dot_product(vmin, norm));
  std::pair<T, T> ret(len, w);
  return ret;
}

template <class T>
std::vector<vgl_point_2d<T> > vgl_orient_box_2d<T>::corners() const{
  vgl_point_2d<T>& pmaj0 = major_axis_.point1();
  vgl_point_2d<T>& pmaj1 = major_axis_.point2();
  double dx = static_cast<double>(pmaj1.x()-pmaj0.x());
  double dy = static_cast<double>(pmaj1.y()-pmaj0.y());
  double len = std::sqrt(dx*dx + dy*dy);
  //perpendicular vector
  double px = -dy/len, py = dx/len;
  // midpoint
  double midx = (0.5*dx) + static_cast<double>(pmaj0.x());
  double midy = (0.5*dy) + static_cast<double>(pmaj0.y());
  // vector to first point of minor axis
  double v0x = static_cast<double>(minor_axis_.point1().x())-midx;
  double v0y = static_cast<double>(minor_axis_.point1().y())-midy;
  // perp distance to first minor endpoint
  double r0 = v0x*px + v0y*py;
  // vector to 2nd point of minor axis
  double v1x = static_cast<double>(minor_axis_.point2().x())-midx;
  double v1y = static_cast<double>(minor_axis_.point2().y())-midy;;
  // perp distance to second minor endpoint
  double r1 = v1x*px + v1y*py;
  // generate the corner points in counter-clockwise order
  T x0 = pmaj0.x() +static_cast<T>(px*r1);
  T y0 = pmaj0.y() + static_cast<T>(py*r1);
  vgl_point_2d<T> p0(x0, y0);
  T x1 = pmaj1.x() + static_cast<T>(px*r1);
  T y1 = pmaj1.y() + static_cast<T>(py*r1);
  vgl_point_2d<T> p1(x1, y1);
  T x2 = pmaj1.x() + static_cast<T>(px*r0);
  T y2 = pmaj1.y() + static_cast<T>(py*r0);
  vgl_point_2d<T> p2(x2, y2);
  T x3 = pmaj0.x() + static_cast<T>(px*r0);
  T y3 = pmaj0.y() + static_cast<T>(py*r0);
  vgl_point_2d<T> p3(x3, y3);
  std::vector<vgl_point_2d<T> > corns;
  corns.push_back(p0);  corns.push_back(p1);
  corns.push_back(p2);  corns.push_back(p3);
  return corns;
}
template <class T>
vgl_point_2d<T> vgl_orient_box_2d<T>::centroid() const{
  vgl_point_2d<T>& pmaj0 = major_axis_.point1();
  vgl_point_2d<T>& pmaj1 = major_axis_.point2();
  vgl_point_2d<T> c((pmaj0.x() + pmaj1.x())/T(2),(pmaj0.y() + pmaj1.y())/T(2));
  return c;
}
template <class T>
std::ostream&  operator<<(std::ostream& os, const vgl_orient_box_2d<T>& obox){
  os << "major axis:" << obox.major_axis() << " minor_axis:" << obox.minor_axis()<< "\n";
  return os;
}
template <class T>
std::istream&  operator>>(std::istream& is,  vgl_orient_box_2d<T>& obox){
  vgl_line_segment_2d<T> maj, min;
  is >> maj >> min;
  obox.set(maj, min);
  return is;
}
template <class T>
vgl_point_2d<T> vgl_orient_box_2d<T>::transform_to_obox(vgl_point_2d<T> const& p) const{
  vgl_point_2d<T> c = this->centroid();
  vgl_vector_2d<T> vp = p-c;
  vgl_vector_2d<T> dir = major_axis_.direction();
  vgl_vector_2d<T> norm(-dir.y(), dir.x());
  T mag = (major_axis_.point2()-major_axis_.point1()).length();
  T mag_minor = fabs(dot_product(norm, (minor_axis_.point2()-minor_axis_.point1())));
  // orthogonal obox coordinate vector
  T u = dot_product(dir, vp)/mag;
  T v = dot_product(norm,vp)/mag_minor;
  return vgl_point_2d<T>(u, v);
}
template <class T>
bool vgl_orient_box_2d<T>::contains(T const& x, T const& y) const{
  vgl_point_2d<T> p(x, y);
  vgl_point_2d<T> uv = this->transform_to_obox(p);
  T half = T(1)/T(2);
  if(uv.x()<-half || uv.x()>half)
    return false;
  if(uv.y()<min_v_ || uv.y()>max_v_)
    return false;
  return true;
}
#undef VGL_ORIENT_BOX_2D_INSTANTIATE
#define VGL_ORIENT_BOX_2D_INSTANTIATE(T) \
template class vgl_orient_box_2d<T >; \
template std::ostream& operator<<(std::ostream&, const vgl_orient_box_2d<T >&); \
template std::istream& operator>>(std::istream&, vgl_orient_box_2d<T >&)
