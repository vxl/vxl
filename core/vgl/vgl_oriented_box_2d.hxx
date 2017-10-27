#include "vgl_oriented_box_2d.h"
#include <vgl/vgl_clip.h>
#include <vgl/vgl_point_2d.h>
#include <string>
#include <math.h>
#include <vgl/vgl_area.h>
template <class T>
vgl_oriented_box_2d<T>::vgl_oriented_box_2d(long length, long width, vgl_point_2d<T> center){
  T l = length/T(2), w = width/T(2);
  T cx = center.x(), cy = center.y();
  vgl_point_2d<T> pmaj0(cx-l, cy), pmaj1(cx+l, cy);
  vgl_point_2d<T> pmin0(cx, cy-w), pmin1(cx, cy+w);
  major_axis_.set(pmaj0, pmaj1);
  minor_axis_.set(pmin0, pmin1);
}


// axis-aligned bounding box
template <class T>
vgl_box_2d<T> vgl_oriented_box_2d<T>::bounding_box() const{
  vgl_box_2d<T> ret;
  ret.add(major_axis_.point1());
  ret.add(major_axis_.point2());
  ret.add(minor_axis_.point1());
  ret.add(minor_axis_.point2());
  return ret;
}
template <class T>
vgl_point_2d<T> vgl_oriented_box_2d<T>::length_width() const{

  const vgl_point_2d<T>  pmaj0 = major_axis_.point1();
  const vgl_point_2d<T>  pmaj1 = major_axis_.point2();
  double dx = static_cast<double>(pmaj1.x()-pmaj0.x());
  double dy = static_cast<double>(pmaj1.y()-pmaj0.y());
  double len = sqrt(dx*dx + dy*dy);
  //perpendicular vector
  double px = -dy/len, py = dx/len;
  // midpoint
  double midx = (0.5*dx) + static_cast<double>(pmaj0.x());
  double midy = (0.5*dy) + static_cast<double>(pmaj0.y());
  // vector to first point of minor axis
  double v0x = (minor_axis_.point1()).x()-midx;
  double v0y = (minor_axis_.point1()).y()-midy;
  // perp distance to first minor endpoint
  double r0 = v0x*px + v0y*py;
  // vector to 2nd point of minor axis
  double v1x = (minor_axis_.point2()).x()-midx;
  double v1y = (minor_axis_.point2()).y()-midy;
  // perp distance to second minor endpoint
  double r1 = v1x*px + v1y*py;
  return vgl_point_2d<T>(static_cast<T>(len), static_cast<T>(fabs(r0)+fabs(r1)));
}

template <class T>
static vgl_point_2d<T> rotate_point(vgl_point_2d<T> cent, vgl_point_2d<T> p, T ang){
  vgl_vector_2d<T> v = p-cent;
  T c = cos(ang), s = sin(ang);
  T vrx = v.x()*c - v.y()*s;
  T vry = v.x()*s + v.y()*c;
  vgl_vector_2d<T> pr(vrx, vry);
  return cent + pr;
}
template <class T>
vgl_oriented_box_2d<T> vgl_oriented_box_2d<T>::
rotate(vgl_point_2d<T> const& rot_center,T angle) const{
  vgl_point_2d<T> rot_maj_p1 =
    rotate_point(rot_center, major_axis_.point1(), angle);
  vgl_point_2d<T> rot_maj_p2 =
    rotate_point(rot_center, major_axis_.point2(), angle);
  vgl_point_2d<T> rot_min_p1 =
    rotate_point(rot_center, minor_axis_.point1(), angle);
  vgl_point_2d<T> rot_min_p2 =
    rotate_point(rot_center, minor_axis_.point2(), angle);

 vgl_line_segment_2d<T> rot_maj(rot_maj_p1, rot_maj_p2);
 vgl_line_segment_2d<T> rot_min(rot_min_p1, rot_min_p2);
 return vgl_oriented_box_2d<T>(rot_maj, rot_min);
}

template <class T>
vgl_oriented_box_2d<T> vgl_oriented_box_2d<T>::
translate(T tx, T ty) const{
  vgl_vector_2d<T>  t(tx, ty);
  return translate_obox(*this, t);
}


// rotate the box until the major axis is pointing along the x-axis 0 or 180 deg
template <class T>
T vgl_oriented_box_2d<T>::align_major_axis(vgl_point_2d<T> const& rot_center, vgl_oriented_box_2d<T>& rot_box) const{
  T right_ang = atan2(T(1), T(0));
  T slope = major_axis_.slope_radians();
  // slope can be in cut domains [-180, -90), (90, 180]
  // or in the continuous domain [-90, 90]
  T rot_ang = T(0);
  if((slope <= right_ang) && slope >= -right_ang)
    rot_ang = -slope;
  else if((slope > right_ang) && (slope <= T(2)*right_ang))
    rot_ang = (T(2)*right_ang) - slope;
  else if((slope < -right_ang) && (slope >= -T(2)*right_ang))
    rot_ang = -((T(2)*right_ang)+slope);
  else
    std::cout << " can't happen\n";
 vgl_point_2d<T> rot_maj_p1 =
   rotate_point(rot_center, major_axis_.point1(), rot_ang);
 vgl_point_2d<T> rot_maj_p2 =
   rotate_point(rot_center, major_axis_.point2(), rot_ang);
 vgl_point_2d<T> rot_min_p1 =
   rotate_point(rot_center, minor_axis_.point1(), rot_ang);
 vgl_point_2d<T> rot_min_p2 =
   rotate_point(rot_center, minor_axis_.point2(), rot_ang);

 vgl_line_segment_2d<T> rot_maj(rot_maj_p1, rot_maj_p2);
 vgl_line_segment_2d<T> rot_min(rot_min_p1, rot_min_p2);

 rot_box.set(rot_maj, rot_min);
 return rot_ang;
}  
template <class T>
vgl_line_segment_2d<T> vgl_oriented_box_2d<T>::perp_line_seg(double u) const{
  const vgl_point_2d<T>& pmaj0 = major_axis_.point1();
  const vgl_point_2d<T>& pmaj1 = major_axis_.point2();
  double dx = static_cast<double>(pmaj1.x()-pmaj0.x());
  double dy = static_cast<double>(pmaj1.y()-pmaj0.y());
  double len = sqrt(dx*dx + dy*dy);
  //perpendicular unit vector
  double px = -dy/len, py = dx/len;
  // midpoint
  double midx = ((0.5+u)*dx) + static_cast<double>(pmaj0.x());
  double midy = ((0.5+u)*dy) + static_cast<double>(pmaj0.y());
  // vector to first point of minor axis
  double v0x = (minor_axis_.point1()).x()-midx;
  double v0y = (minor_axis_.point1()).y()-midy;
  // perp distance to first minor endpoint
  double r0 = v0x*px + v0y*py;
  // vector to 2nd point of minor axis
  double v1x = (minor_axis_.point2()).x()-midx;
  double v1y = (minor_axis_.point2()).y()-midy;
  // perp distance to second minor endpoint
  double r1 = v1x*px + v1y*py;
  vgl_point_2d<T> p0(static_cast<T>(midx + r0*px), static_cast<T>(midy + r0*py));
  vgl_point_2d<T> p1(static_cast<T>(midx+ r1*px), static_cast<T>(midy+ r1*py));
  return vgl_line_segment_2d<T>(p0, p1);
}
template <class T>

vgl_line_segment_2d<T> vgl_oriented_box_2d<T>::para_line_seg(double v) const{
  const vgl_point_2d<T>& pmaj0 = major_axis_.point1();
  const vgl_point_2d<T>& pmaj1 = major_axis_.point2();
  double dx = static_cast<double>(pmaj1.x()-pmaj0.x());
  double dy = static_cast<double>(pmaj1.y()-pmaj0.y());
  //perpendicular vector
  double px = -dy, py = dx;
  // box midpoint in pixel coordinates
  double midx = (0.5*dx) + static_cast<double>(pmaj0.x());
  double midy = (0.5*dy) + static_cast<double>(pmaj0.y());
  // add perp vector to midpoint
  midx += px*v;
  midy += py*v;
  T p0x = static_cast<T>(midx - 0.5*dx), p0y = static_cast<T>(midy - 0.5*dy);
  T p1x = static_cast<T>(midx + 0.5*dx), p1y = static_cast<T>(midy + 0.5*dy);
  return vgl_line_segment_2d<T>(vgl_point_2d<T>(p0x, p0y), vgl_point_2d<T>(p1x, p1y));
}
template <class T>
std::vector<vgl_point_2d<T> > vgl_oriented_box_2d<T>::grid_locations(double du, double dv, int u_radius, int v_radius) const{
  std::vector<vgl_point_2d<T> > pix_locs;
  const vgl_point_2d<T>& pmaj0 = major_axis_.point1();
  const vgl_point_2d<T>& pmaj1 = major_axis_.point2();
  double dx = static_cast<double>(pmaj1.x()-pmaj0.x());
  double dy = static_cast<double>(pmaj1.y()-pmaj0.y());
  //perpendicular vector
  double px = -dy, py = dx;
  // box midpoint in pixel coordinates
  double midx = (0.5*dx) + static_cast<double>(pmaj0.x());
  double midy = (0.5*dy) + static_cast<double>(pmaj0.y());
  for(int iu = -u_radius; iu<=u_radius; ++iu)
    for(int iv = -v_radius; iv<=v_radius; ++iv){
      //point on u  axis in pixel coordinates
      double ux = midx + iu*du*dx;
      double uy = midy + iu*du*dy;
      //then add the v vector
      T uvx = static_cast<T>(ux + iv*dv*px);
      T uvy = static_cast<T>(uy + iv*dv*py);
      pix_locs.push_back(vgl_point_2d<T>(uvx, uvy));
    }
  return pix_locs;
}
template <class T>
std::vector<vgl_point_2d<T> > vgl_oriented_box_2d<T>::corners() const{
  const vgl_point_2d<T>& pmaj0 = major_axis_.point1();
  const vgl_point_2d<T>& pmaj1 = major_axis_.point2();
  double dx = static_cast<double>(pmaj1.x()-pmaj0.x());
  double dy = static_cast<double>(pmaj1.y()-pmaj0.y());
  double len = sqrt(dx*dx + dy*dy);
  //perpendicular vector
  double px = -dy/len, py = dx/len;
  // midpoint
  double midx = (0.5*dx) + static_cast<double>(pmaj0.x());
  double midy = (0.5*dy) + static_cast<double>(pmaj0.y());
  // vector to first point of minor axis
  double v0x = (minor_axis_.point1()).x()-midx;
  double v0y = (minor_axis_.point1()).y()-midy;
  // perp distance to first minor endpoint
  double r0 = v0x*px + v0y*py;
  // vector to 2nd point of minor axis
  double v1x = (minor_axis_.point2()).x()-midx;
  double v1y = (minor_axis_.point2()).y()-midy;
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
vgl_point_2d<T> vgl_oriented_box_2d<T>::center() const{
  const vgl_point_2d<T>& pmaj0 = major_axis_.point1();
  const vgl_point_2d<T>& pmaj1 = major_axis_.point2();
  vgl_point_2d<T> c((pmaj0.x() + pmaj1.x())/T(2),(pmaj0.y() + pmaj1.y())/T(2));
  return c;
}
template <class T>
void vgl_oriented_box_2d<T>::write(std::ostream& os) const{
  os << major_axis_.point1().x() << ' ' << major_axis_.point1().y() << ' ' << major_axis_.point2().x() << ' '
     << major_axis_.point2().y() << ' ' << minor_axis_.point1().x() << ' ' << minor_axis_.point1().y() << ' '
     << minor_axis_.point2().x() << ' ' << minor_axis_.point2().y() << std::endl;
}

template <class T>
void vgl_oriented_box_2d<T>::read(std::istream& is) {
  T pmaj1x, pmaj1y, pmaj2x, pmaj2y, pmin1x, pmin1y, pmin2x, pmin2y;
  is >> pmaj1x >>  pmaj1y >> pmaj2x >>  pmaj2y 
     >> pmin1x  >> pmin1y  >> pmin2x >> pmin2y; 
  vgl_point_2d<T> pmaj1(pmaj1x, pmaj1y), pmaj2(pmaj2x, pmaj2y);
  vgl_point_2d<T> pmin1(pmin1x, pmin1y), pmin2(pmin2x, pmin2y);
  major_axis_.set(pmaj1, pmaj2);
  minor_axis_.set(pmin1, pmin2);
}

template <class T>
std::ostream&  operator<<(std::ostream& os, const vgl_oriented_box_2d<T>& obox){
  os << "major axis:" << obox.major_axis() << " minor_axis:" << obox.minor_axis()<< "\n";
  return os;
}

template <class T>
std::istream&  operator>>(std::istream& is,  vgl_oriented_box_2d<T>& obox){
  vgl_line_segment_2d<T> maj, min;
  std::string temp;
  is >> temp;
  is >> maj;
  is >> temp;
  is >> min;
  obox.set(maj, min);
  return is;
}

template <class T>
T obox_int_over_union(vgl_oriented_box_2d<T> const& ob0, vgl_oriented_box_2d<T> const& ob1){
  std::vector<vgl_point_2d<T> > corners0 = ob0.corners();
  std::vector<vgl_point_2d<T> > corners1 = ob1.corners();
  vgl_polygon<T> p0(corners0), p1(corners1);
  vgl_polygon<T> poly_inter = vgl_clip(p0, p1, vgl_clip_type_intersect);
  vgl_polygon<T> poly_union = vgl_clip(p0, p1, vgl_clip_type_union);
  T area_int = vgl_area(poly_inter);
  T area_union = vgl_area(poly_union);
  return area_int/area_union;
}

template <class T>
vgl_oriented_box_2d<T> obox_from_rect(vgl_box_2d<T> box){
  vgl_point_2d<T> c = box.centroid();
  return vgl_oriented_box_2d<T>();
}
template <class T>
vgl_oriented_box_2d<T> translate_obox(vgl_oriented_box_2d<T> const& obox, vgl_vector_2d<T> const& trans){
  vgl_line_segment_2d<T> maj_seg = obox.major_axis();
  vgl_line_segment_2d<T> min_seg = obox.minor_axis();
  vgl_point_2d<T> maj1(maj_seg.point1()), maj2(maj_seg.point2());
  vgl_point_2d<T> min1(min_seg.point1()), min2(min_seg.point2());
  maj1 += trans; maj2 += trans;
  min1 += trans; min2 += trans;
  return vgl_oriented_box_2d<T>(maj1, maj2, min1, min2);
}

template <class T>
vgl_point_2d<T> transform_to_obox(vgl_oriented_box_2d<T> const& obox, vgl_point_2d<T> const& p){
  vgl_point_2d<T> c = obox.center();
  double px = static_cast<double>(p.x()-c.x()), py = static_cast<double>(p.y()-c.y());
  vgl_line_segment_2d<T> maj = obox.major_axis();
  const vgl_point_2d<T>& pmaj0 = maj.point1();
  const vgl_point_2d<T>& pmaj1 = maj.point2();
  // major axis vector
  double vmajx = static_cast<double>(pmaj1.x()-pmaj0.x()), vmajy = static_cast<double>(pmaj1.y()-pmaj0.y());
  // convert to unit vector
  double mag = sqrt(vmajx*vmajx + vmajy*vmajy);
  if(mag == 0.0){
    std::cout << "warning - zero length major axis, degenerate orientation\n";
    return vgl_point_2d<T>();
  }
  vmajx/=mag; vmajy/=mag;
  // orthogonal obox coordinate vector
  double vminx = -vmajy;
  double vminy = vmajx;
  double u = (px*vmajx + py*vmajy)/mag;
  double v = (px*vminx + py*vminy)/mag;
  return vgl_point_2d<T>(static_cast<T>(u), static_cast<T>(v));
}
#undef VGL_ORIENTED_BOX_2D_INSTANTIATE
#define VGL_ORIENTED_BOX_2D_INSTANTIATE(T) \
template class vgl_oriented_box_2d<T >; \
template std::ostream& operator<<(std::ostream&, vgl_oriented_box_2d<T >const&); \
template std::istream& operator>>(std::istream&, vgl_oriented_box_2d<T >&); \
template T obox_int_over_union(vgl_oriented_box_2d<T> const& ob0, vgl_oriented_box_2d<T> const& ob1); \
template vgl_oriented_box_2d<T> obox_from_rect(vgl_box_2d<T> rect); \
template vgl_oriented_box_2d<T> translate_obox(vgl_oriented_box_2d<T> const& obox, vgl_vector_2d<T> const& trans); \
template vgl_point_2d<T> transform_to_obox(vgl_oriented_box_2d<T> const& obox, vgl_point_2d<T> const& p)
