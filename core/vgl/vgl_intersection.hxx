// This is core/vgl/vgl_intersection.hxx
#ifndef vgl_intersection_hxx_
#define vgl_intersection_hxx_
//:
// \file
// \author Gamze Tunali

#include <limits>
#include <cmath>
#include <vector>
#include "vgl_intersection.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_tolerance.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_lineseg_test.hxx>

static double eps = 1.0e-8; // tolerance for intersections
inline bool vgl_near_zero(double x) { return x < eps && x > -eps; }
inline bool vgl_near_eq(double x, double y) { return vgl_near_zero(x-y); }

//: Return the intersection of two boxes (which is itself is a box, possibly the empty box)
// \relatesalso vgl_box_2d
template <class T>
vgl_box_2d<T> vgl_intersection(vgl_box_2d<T> const& b1,vgl_box_2d<T> const& b2)
{
  T xmin = b1.min_x() > b2.min_x() ? b1.min_x() : b2.min_x();
  T ymin = b1.min_y() > b2.min_y() ? b1.min_y() : b2.min_y();
  T xmax = b1.max_x() < b2.max_x() ? b1.max_x() : b2.max_x();
  T ymax = b1.max_y() < b2.max_y() ? b1.max_y() : b2.max_y();
  return vgl_box_2d<T>(xmin,xmax,ymin,ymax);
}

//: Return true if line intersects box. If so, compute intersection points.
// \relatesalso vgl_box_3d
// \relatesalso vgl_infinite_line_3d
template <class T>
bool vgl_intersection(vgl_box_3d<T> const& box,
                      vgl_infinite_line_3d<T> const& line_3d,
                      vgl_point_3d<T>& p0,
                      vgl_point_3d<T>& p1)
{
  vgl_point_3d<T> lpt = line_3d.point();
  vgl_vector_3d<T> di = line_3d.direction();
  vgl_point_3d<double> dpt(static_cast<double>(lpt.x()),
                           static_cast<double>(lpt.y()),
                           static_cast<double>(lpt.z()));
  vgl_vector_3d<double> dir(static_cast<double>(di.x()),
                            static_cast<double>(di.y()),
                            static_cast<double>(di.z()));
  vgl_infinite_line_3d<double> dline_3d(dpt, dir);
  // expand box by epsilon tolerance
  double xmin = box.min_x(), xmax = box.max_x(),
         ymin = box.min_y(), ymax = box.max_y(),
         zmin = box.min_z(), zmax = box.max_z();
  vgl_point_3d<double> minp(xmin, ymin, zmin), maxp(xmax, ymax, zmax);
  // find intersection point of the line with each of the six box planes
  vgl_vector_3d<double> vxmin(-1.0, 0.0, 0.0), vxmax(1.0, 0.0, 0.0),
                        vymin(0.0, -1.0, 0.0), vymax(0.0, 1.0, 0.0),
                        vzmin(0.0, 0.0, -1.0), vzmax(0.0, 0.0, 1.0);
  vgl_plane_3d<double> pl_xmin(vxmin, minp),
                       pl_xmax(vxmax, maxp),
                       pl_ymin(vymin, minp),
                       pl_ymax(vymax, maxp),
                       pl_zmin(vzmin, minp),
                       pl_zmax(vzmax, maxp);
  vgl_point_3d<double> pt_xmin(.0,.0,.0), pt_xmax(.0,.0,.0), // dummy initializ.
                       pt_ymin(.0,.0,.0), pt_ymax(.0,.0,.0), // to avoid
                       pt_zmin(.0,.0,.0), pt_zmax(.0,.0,.0); // compiler warning
  bool xmin_good = vgl_intersection(dline_3d, pl_xmin, pt_xmin);
  bool xmax_good = vgl_intersection(dline_3d, pl_xmax, pt_xmax);
  bool ymin_good = vgl_intersection(dline_3d, pl_ymin, pt_ymin);
  bool ymax_good = vgl_intersection(dline_3d, pl_ymax, pt_ymax);
  bool zmin_good = vgl_intersection(dline_3d, pl_zmin, pt_zmin);
  bool zmax_good = vgl_intersection(dline_3d, pl_zmax, pt_zmax);
  // Go through the six cases and return the two intersection points
  // that lie on box faces. Find the pair that are farthest apart.
  // There could be multiple intersections if the line passes through the
  // corners of the box.
  unsigned int npts = 0;
  vgl_point_3d<double> dp0=pt_xmin, dp1=pt_xmax; // keep this initialisation!

  // y-z face at xmin
  if (xmin_good &&
      pt_xmin.y()>=ymin && pt_xmin.y()<=ymax &&
      pt_xmin.z()>=zmin && pt_xmin.z()<=zmax)
  {
    // dp0 = pt_xmin; // not needed: already set when dp0 was initialised
    ++npts;
  }
  // y-z face at xmax
  if (xmax_good &&
      pt_xmax.y()>=ymin && pt_xmax.y()<=ymax &&
      pt_xmax.z()>=zmin && pt_xmax.z()<=zmax)
  {
    if  (npts == 0) dp0 = pt_xmax;
    // else         dp1 = pt_xmax; // not needed: already set when dp1 was initialised
    ++npts;
  }
  // x-z face at ymin
  if (ymin_good &&
      pt_ymin.x()>=xmin && pt_ymin.x()<=xmax &&
      pt_ymin.z()>=zmin && pt_ymin.z()<=zmax)
  {
    if      (npts == 0) { dp0 = pt_ymin; ++npts; }
    else if (npts == 1) { dp1 = pt_ymin; ++npts; }
    else /* npts == 2*/ if (sqr_length(pt_ymin-dp0)>sqr_length(dp1-dp0)) dp1 = pt_ymin;
  }
  // x-z face at ymax
  if (ymax_good &&
      pt_ymax.x()>=xmin && pt_ymax.x()<=xmax &&
      pt_ymax.z()>=zmin && pt_ymax.z()<=zmax)
  {
    if      (npts == 0) { dp0 = pt_ymax; ++npts; }
    else if (npts == 1) { dp1 = pt_ymax; ++npts; }
    else /* npts == 2*/ if (sqr_length(pt_ymax-dp0)>sqr_length(dp1-dp0)) dp1 = pt_ymax;
  }
  // x-y face at zmin
  if (zmin_good &&
      pt_zmin.x()>=xmin && pt_zmin.x()<=xmax &&
      pt_zmin.y()>=ymin && pt_zmin.y()<=ymax)
  {
    if      (npts == 0) { dp0 = pt_zmin; ++npts; }
    else if (npts == 1) { dp1 = pt_zmin; ++npts; }
    else /* npts == 2*/ if (sqr_length(pt_zmin-dp0)>sqr_length(dp1-dp0)) dp1 = pt_zmin;
  }

  // x-y face at zmax
  if (zmax_good &&
      pt_zmax.x()>=xmin && pt_zmax.x()<=xmax &&
      pt_zmax.y()>=ymin && pt_zmax.y()<=ymax)
  {
    if      (npts == 0) { dp0 = pt_zmax; ++npts; }
    else if (npts == 1) { dp1 = pt_zmax; ++npts; }
    else /* npts == 2*/ if (sqr_length(pt_zmax-dp0)>sqr_length(dp1-dp0)) dp1 = pt_zmax;
  }

  if (npts==2) {
    p0.set(static_cast<T>(dp0.x()),
           static_cast<T>(dp0.y()),
           static_cast<T>(dp0.z()));
    p1.set(static_cast<T>(dp1.x()),
           static_cast<T>(dp1.y()),
           static_cast<T>(dp1.z()));
    return true;
  }
  else
    return false;
}

//: Return true if ray intersects box. If so, compute intersection points.
// If ray origin is inside box then p0==p1
// \relatesalso vgl_box_3d
// \relatesalso vgl_ray_3d
template <class T>
bool vgl_intersection(vgl_box_3d<T> const& box,
                      vgl_ray_3d<T> const& ray,
                      vgl_point_3d<T>& p0,
                      vgl_point_3d<T>& p1)
{
  // convert ray to infinite line
  vgl_infinite_line_3d<T> linf(ray.origin(), ray.direction());
  bool good_inter = vgl_intersection(box, linf, p0, p1);
  if (!good_inter) return false;
  // check if ray origin is inside the box
  vgl_point_3d<T> org = ray.origin();
  if (!box.contains(org))
    //check if the intersection points are in the ray domain
    return ray.contains(p0)&&ray.contains(p1);

  //ray origin is inside the box so find the intersection point in the
  //positive ray domain
  if (ray.contains(p0)) {
    p1 = p0; return true;
  }
  if (ray.contains(p1)) {
    p0 = p1; return true;
  }
  return false;
}

//: Return true if a box and plane intersect in 3D
// \relatesalso vgl_plane_3d
// \relatesalso vgl_box_3d
template <class T>
bool vgl_intersection(vgl_box_3d<T> const& b, vgl_plane_3d<T> const& plane)
{
  // find the box corners
  std::vector<vgl_point_3d<T> > corners;
  corners.push_back(b.min_point());
  corners.push_back(vgl_point_3d<T> (b.min_x()+b.width(), b.min_y(), b.min_z()));
  corners.push_back(vgl_point_3d<T> (b.min_x()+b.width(), b.min_y()+b.height(), b.min_z()));
  corners.push_back(vgl_point_3d<T> (b.min_x(), b.min_y()+b.height(), b.min_z()));
  corners.push_back(vgl_point_3d<T> (b.min_x(), b.min_y(), b.max_z()));
  corners.push_back(vgl_point_3d<T> (b.min_x()+b.width(), b.min_y(), b.max_z()));
  corners.push_back(b.max_point());
  corners.push_back(vgl_point_3d<T> (b.min_x(), b.min_y()+b.height(), b.max_z()));

  // find the signed distance from the box corners to the plane
  int pos=0, neg=0;
  for (unsigned int c=0; c<corners.size(); c++) {
    vgl_point_3d<T> corner=corners[c];
    double d=(plane.a()*corner.x());
    d+=(plane.b()*corner.y());
    d+=(plane.c()*corner.z());
    d+=plane.d();
    if (d > 0)
      pos++;
    else if (d<0)
      neg++;
  }
  return neg!=8 && pos!=8; // completely inside polygon plane
}

//: Return the intersection of two boxes (which is itself either a box, or empty)
// \relatesalso vgl_box_3d
template <class T>
vgl_box_3d<T> vgl_intersection(vgl_box_3d<T> const& b1,vgl_box_3d<T> const& b2)
{
  T xmin = b1.min_x() > b2.min_x() ? b1.min_x() : b2.min_x();
  T ymin = b1.min_y() > b2.min_y() ? b1.min_y() : b2.min_y();
  T zmin = b1.min_z() > b2.min_z() ? b1.min_z() : b2.min_z();
  T xmax = b1.max_x() < b2.max_x() ? b1.max_x() : b2.max_x();
  T ymax = b1.max_y() < b2.max_y() ? b1.max_y() : b2.max_y();
  T zmax = b1.max_z() < b2.max_z() ? b1.max_z() : b2.max_z();
  return vgl_box_3d<T>(xmin,ymin,zmin,xmax,ymax,zmax);
}

//: compute the intersection of an infinite line with *this box.
//  p0 and p1 are the intersection points
// In the normal case (no degeneracies) there are six possible intersection combinations:
// \verbatim
//
//                C01 /    CY     \ C11
//                   /     |       \           .
//       ymax  -----/------|--------\-----
//            |    /       |         \    |
//            |   /        |          \   |
//            |  /         |           \  | \  .
//            | /          |            \ |  \_ Bounding Box
//            |/           |             \|
//            /            |              \    .
//           /|            |              |\   .
//           ---------------------------------- CX
//          \ |            |              /
//           \|            |             /|
//            \            |            / |
//            |\           |           /  |
//            | \          |          /   |
//            |  \         |         /    |
//       xmin  ---\--------|--------/-----   xmax
//       ymin      \       |       /
//              C00 \             / C10
// \endverbatim

template <class Type>
bool vgl_intersection(const vgl_box_2d<Type>& box,
                      const vgl_line_2d<Type>& line,
                      vgl_point_2d<Type>& p0,
                      vgl_point_2d<Type>& p1)
{
  double a = line.a(), b = line.b(), c = line.c();
  double xmin=box.min_x(), xmax=box.max_x();
  double ymin=box.min_y(), ymax=box.max_y();

  // Run through the cases
  //
  if (vgl_near_zero(a))// The line is y = -c/b
  {
    float y0 = static_cast<float>(-c/b);
    // The box edge is collinear with line?
    if (vgl_near_eq(ymin,y0))
    {
      p0.set(static_cast<Type>(xmin), static_cast<Type>(ymin));
      p1.set(static_cast<Type>(xmax), static_cast<Type>(ymin));
      return true;
    }
    if (vgl_near_eq(ymax,y0))
    {
      p0.set(static_cast<Type>(xmin), static_cast<Type>(ymax));
      p1.set(static_cast<Type>(xmax), static_cast<Type>(ymax));
      return true;
    }

    if ((ymin > y0) || (y0 > ymax)) // The line does not intersect the box
      return false;
    else // The line does intersect
    {
      p0.set(static_cast<Type>(xmin), static_cast<Type>(y0));
      p1.set(static_cast<Type>(xmax), static_cast<Type>(y0));
      return true;
    }
  }

  if (vgl_near_zero(b))// The line is x = -c/a
  {
    float x0 = static_cast<float>(-c/a);
    // The box edge is collinear with l?
    if (vgl_near_eq(xmin,x0))
    {
      p0.set(static_cast<Type>(xmin), static_cast<Type>(ymin));
      p1.set(static_cast<Type>(xmin), static_cast<Type>(ymax));
      return true;
    }
    if (vgl_near_eq(xmax,x0))
    {
      p0.set(static_cast<Type>(xmax), static_cast<Type>(ymin));
      p1.set(static_cast<Type>(xmax), static_cast<Type>(ymax));
      return true;
    }

    if (xmin <= x0 && x0 <= xmax) // The line intersects the box
    {
      p0.set(static_cast<Type>(x0), static_cast<Type>(ymin));
      p1.set(static_cast<Type>(x0), static_cast<Type>(ymax));
      return true;
    }
    else
      return false;
  }

  // The normal case with no degeneracies
  //
  // Intersection with x = xmin
  float y_xmin_int = static_cast<float>(-(c + a*xmin)/b);
  bool inside_xmin = (y_xmin_int >= ymin) && (y_xmin_int <= ymax);

  // Intersection with x = xmax
  float y_xmax_int = static_cast<float>(-(c + a*xmax)/b);
  bool inside_xmax = (y_xmax_int >= ymin) && (y_xmax_int <= ymax);

  // Intersection with y = ymin
  float x_ymin_int = static_cast<float>(-(c + b*ymin)/a);
  bool inside_ymin = (x_ymin_int >= xmin) && (x_ymin_int <= xmax);

  // Intersection with y = ymax
  float x_ymax_int = static_cast<float>(-(c + b*ymax)/a);
  bool inside_ymax = (x_ymax_int >= xmin) && (x_ymax_int <= xmax);

  // Case CX
  if (inside_xmin && inside_xmax &&
      !(vgl_near_eq(y_xmin_int,ymin) && vgl_near_eq(y_xmax_int,ymax)))
  {
    p0.set(static_cast<Type>(xmin), static_cast<Type>(y_xmin_int));
    p1.set(static_cast<Type>(xmax), static_cast<Type>(y_xmax_int));
    return true;
  }

  // Case CY
  if (inside_ymin && inside_ymax &&
      !(vgl_near_eq(x_ymin_int,xmin) && vgl_near_eq(x_ymax_int,xmax)))
  {
    p0.set(static_cast<Type>(x_ymin_int), static_cast<Type>(ymin));
    p1.set(static_cast<Type>(x_ymax_int), static_cast<Type>(ymax));
    return true;
  }

  // Case C00
  if (inside_xmin && inside_ymin &&
      !(inside_xmax && inside_ymax))
  {
    p0.set(static_cast<Type>(xmin), static_cast<Type>(y_xmin_int));
    p1.set(static_cast<Type>(x_ymin_int), static_cast<Type>(ymin));
    return true;
  }

  // Case C01
  if (inside_xmin && inside_ymax &&
      !(inside_xmax && inside_ymin))
  {
    p0.set(static_cast<Type>(xmin), static_cast<Type>(y_xmin_int));
    p1.set(static_cast<Type>(x_ymax_int), static_cast<Type>(ymax));
    return true;
  }

  // Case C10
  if (inside_ymin && inside_xmax &&
      !(inside_xmin && inside_ymax))
  {
    p0.set(static_cast<Type>(x_ymin_int), static_cast<Type>(ymin));
    p1.set(static_cast<Type>(xmax), static_cast<Type>(y_xmax_int));
    return true;
  }

  // Case C11
  if (inside_ymax && inside_xmax &&
      !(inside_xmin && inside_ymin))
  {
    p0.set(static_cast<Type>(x_ymax_int), static_cast<Type>(ymax));
    p1.set(static_cast<Type>(xmax), static_cast<Type>(y_xmax_int));
    return true;
  }
  // Exactly passing through diagonal of BB
  if (inside_xmin && inside_xmax && inside_ymin && inside_ymax)
  {
    if (a>0) // 45 degrees
    {
      p0.set(static_cast<Type>(xmin), static_cast<Type>(ymin));
      p1.set(static_cast<Type>(xmax), static_cast<Type>(ymax));
      return true;
    }
    else // 135 degrees
    {
      p0.set(static_cast<Type>(xmin), static_cast<Type>(ymax));
      p1.set(static_cast<Type>(xmax), static_cast<Type>(ymin));
      return true;
    }
  }
  return false;
}

//: Returns the number of intersections of a line segment with a box, up to two are returned in p0 and p1.
template <class Type>
unsigned int vgl_intersection(const vgl_box_2d<Type>& box,
                              const vgl_line_segment_2d<Type>& line_seg,
                              vgl_point_2d<Type>& p0,
                              vgl_point_2d<Type>& p1)
{
  vgl_line_2d<Type> line(line_seg.a(), line_seg.b(), line_seg.c());
  vgl_point_2d<Type> pi0, pi1;
  // if no intersection just return
  if (!vgl_intersection<Type>(box, line, pi0, pi1))
    return 0;
  unsigned int nint = 0;
  // check if intersection points are interior to the line segment
  if (vgl_lineseg_test_point<Type>(pi0, line_seg)) {
    p0 = pi0;
    nint++;
  }
  if (vgl_lineseg_test_point<Type>(pi1, line_seg)) {
    p1 = pi1;
    nint++;
  }
  return nint;
}
// return the line segment that lies inside the box. If none, return false.
template <class T>
bool vgl_intersection(vgl_box_2d<T> const& box,
                      vgl_line_segment_2d<T> const& line_seg,
                      vgl_line_segment_2d<T>& int_line_seg){
  // check if both segment endpoints are inside the box
  const vgl_point_2d<T>& p1 = line_seg.point1();
  const vgl_point_2d<T>& p2 = line_seg.point2();
  bool p1_in_box = box.contains(p1);
  bool p2_in_box = box.contains(p2);
  if(p1_in_box&&p2_in_box){
    int_line_seg = line_seg;
    return true;
  }
  // form an infinite line
  vgl_line_2d<T> line(line_seg.a(), line_seg.b(), line_seg.c());
  vgl_point_2d<T> pia, pib;
  // if no intersection just return
  if (!vgl_intersection<T>(box, line, pia, pib))
    return false;

  // check if intersection points are interior to the line segment
  bool pia_valid = vgl_lineseg_test_point<T>(pia, line_seg);
  bool pib_valid = vgl_lineseg_test_point<T>(pib, line_seg);
  // shouldn't happen but to be complete ...
  if((!pia_valid)&&(!pib_valid))
    return false;
  //if both intersection points are interior to the line segment then
  //return a segment that spans the box interior
  if(pia_valid&&pib_valid){
    int_line_seg.set(pia, pib);
    return true;
  }
  //only one intersection point is valid so form the line segment interior
  //to the box
  // find the original segment endpoint inside the box
  if(p1_in_box){// p1 is inside the box
    if(pia_valid)
      int_line_seg.set(p1, pia);
    else
      int_line_seg.set(p1, pib);
    return true;
  }else{// p2 is inside the box
    if(pia_valid)
      int_line_seg.set(p2, pia);
    else
      int_line_seg.set(p2, pib);
    return true;
  }
  return false;
}

//: Return the intersection point of two concurrent lines
template <class T>
vgl_point_3d<T> vgl_intersection(vgl_line_3d_2_points<T> const& l1,
                                 vgl_line_3d_2_points<T> const& l2)
{
  assert(concurrent(l1,l2));
  T a0=l1.point1().x(),a1=l1.point2().x(),a2=l2.point1().x(),a3=l2.point2().x(),
    b0=l1.point1().y(),b1=l1.point2().y(),b2=l2.point1().y(),b3=l2.point2().y(),
    c0=l1.point1().z(),c1=l1.point2().z(),c2=l2.point1().z(),c3=l2.point2().z();
  T t1 = (b3-b2)*(a1-a0)-(a3-a2)*(b1-b0), t2 = (b0-b2)*(a1-a0)-(a0-a2)*(b1-b0);
  if (std::abs(t1) < 0.000001)
  {
    t1 = (c3-c2)*(a1-a0)-(a3-a2)*(c1-c0), t2 = (c0-c2)*(a1-a0)-(a0-a2)*(c1-c0);
    if (std::abs(t1) < 0.000001)
      t1 = (c3-c2)*(b1-b0)-(b3-b2)*(c1-c0), t2 = (c0-c2)*(b1-b0)-(b0-b2)*(c1-c0);
  }

  return vgl_point_3d<T>(((t1-t2)*a2+t2*a3)/t1,
                         ((t1-t2)*b2+t2*b3)/t1,
                         ((t1-t2)*c2+t2*c3)/t1);
}

//: Return the intersection point of segments of two concurrent lines
// \relatesalso vgl_line_segment_3d
template <class T>
bool vgl_intersection(vgl_line_segment_3d<T> const& l1,
                      vgl_line_segment_3d<T> const& l2,
                      vgl_point_3d<T>& i_pnt)
{
  vgl_line_3d_2_points<T> l21(l1.point1(),l1.point2());
  vgl_line_3d_2_points<T> l22(l2.point1(),l2.point2());
  if (!concurrent(l21, l22))
    return false;
  i_pnt = vgl_intersection(l21, l22);

  double l1_len =   length(l1.point1() - l1.point2());
  double l1_idist = length(l1.point1() - i_pnt) + length(l1.point2() - i_pnt);
  double l2_len =   length(l2.point1() - l2.point2());
  double l2_idist = length(l2.point1() - i_pnt) + length(l2.point2() - i_pnt);
  return vgl_near_zero(l1_len - l1_idist) && vgl_near_zero(l2_len - l2_idist);
}

//: Return the intersection point of segments of two concurrent lines
// \relatesalso vgl_line_segment_3d
// \relatesalso vgl_line_3d_2_points
template <class T>
bool vgl_intersection(vgl_line_3d_2_points<T> const& l1,
                      vgl_line_segment_3d<T> const& l2,
                      vgl_point_3d<T>& i_pnt)
{
  vgl_line_3d_2_points<T> l22(l2.point1(),l2.point2());
  if (!concurrent(l1, l22))
    return false;
  i_pnt = vgl_intersection(l1, l22);

  double l1_len =   length(l1.point1() - l1.point2());
  double l1_idist = length(l1.point1() - i_pnt) + length(l1.point2() - i_pnt);
  double l2_len =   length(l2.point1() - l2.point2());
  double l2_idist = length(l2.point1() - i_pnt) + length(l2.point2() - i_pnt);

  return vgl_near_zero(l1_len - l1_idist) && vgl_near_zero(l2_len - l2_idist);
}

//: Return the intersection point of two lines, if concurrent.
// \relatesalso vgl_infinite_line_3d
template <class T>
bool vgl_intersection(vgl_infinite_line_3d<T> const& l1,
                      vgl_infinite_line_3d<T> const& l2,
                      vgl_point_3d<T>& i_pnt)
{
  vgl_line_3d_2_points<T> l21(l1.point(),l1.point_t(T(1)));
  vgl_line_3d_2_points<T> l22(l2.point(),l2.point_t(T(1)));
  if (!concurrent(l21, l22))
    return false;
  i_pnt = vgl_intersection(l21, l22);
  return l1.contains(i_pnt) && l2.contains(i_pnt);
}

template <class T>
bool vgl_intersection(vgl_ray_3d<T> const& r1,
                      vgl_ray_3d<T> const& r2,
                      vgl_point_3d<T>& i_pnt)
{
  vgl_line_3d_2_points<T> l21(r1.origin(),r1.origin()+r1.direction());
  vgl_line_3d_2_points<T> l22(r2.origin(),r2.origin()+r2.direction());
  if (!concurrent(l21, l22))
    return false;
  i_pnt = vgl_intersection(l21, l22);
  return r1.contains(i_pnt)&&r2.contains(i_pnt);
}

//: Return the intersection point of a line and a plane.
// \relatesalso vgl_line_3d_2_points
// \relatesalso vgl_plane_3d
template <class T>
vgl_point_3d<T> vgl_intersection(vgl_line_3d_2_points<T> const& line,
                                 vgl_plane_3d<T> const& plane)
{
  vgl_vector_3d<T> dir = line.direction();

  vgl_point_3d<T> pt;

  double denom = plane.a()*dir.x() +
                 plane.b()*dir.y() +
                 plane.c()*dir.z();

  if (denom == 0)
  {
    const T inf = std::numeric_limits<T>::infinity();
    // Line is either parallel or coplanar
    // If the distance from a line endpoint to the plane is zero, coplanar
    if (vgl_distance(line.point1(), plane)==0.0)
      pt.set(inf,inf,inf);
    else
      pt.set(inf,0,0);
  }
  else
  {
    // Infinite line intersects plane
    double numer = - plane.a()*line.point1().x()
                   - plane.b()*line.point1().y()
                   - plane.c()*line.point1().z()
                   - plane.d();

    dir *= numer/denom;
    pt = line.point1() + dir;
  }

  return pt;
}

//: Return the intersection point of a line and a plane.
// \relatesalso vgl_line_segment_3d
// \relatesalso vgl_plane_3d
template <class T>
bool vgl_intersection(vgl_line_segment_3d<T> const& line,
                      vgl_plane_3d<T> const& plane,
                      vgl_point_3d<T> & i_pt)
{
  vgl_vector_3d<T> dir = line.direction();

  // The calculation of both denom and numerator are both very dodgy numerically, especially if
  // denom or numerator is small compared to the summands. It would be good to find a more
  // numerically stable solution. IMS.
  const double tol = std::numeric_limits<T>::epsilon() * 10e3;

  double denom = plane.a()*dir.x() +
                 plane.b()*dir.y() +
                 plane.c()*dir.z();

  if (std::abs(denom) < tol)
  {
    // Line is either parallel or coplanar
    // If the distance from a line endpoint to the plane is zero, coplanar
    if (vgl_distance(line.point1(), plane)!=0.0)
      return false;

    const T inf = std::numeric_limits<T>::infinity();
    i_pt.set(inf,inf,inf);
    return true;
  }

  double numer = - plane.a()*line.point1().x()
                 - plane.b()*line.point1().y()
                 - plane.c()*line.point1().z()
                 - plane.d();

  double t = numer/denom; // 0<t<1 between two points
  if (t < 0 || t > 1.0) return false;

  i_pt = line.point1() + t * dir;
  return true;
}

template <class T>
bool vgl_intersection(vgl_infinite_line_3d<T> const& line,
                      vgl_plane_3d<T> const& plane,
                      vgl_point_3d<T> & i_pt)
{
  vgl_vector_3d<T> dir = line.direction();
  vgl_point_3d<T> pt = line.point();
  // The calculation of both denom and numerator are both very dodgy numerically, especially if
  // denom or numerator is small compared to the summands. It would be good to find a more
  // numerically stable solution. IMS.
  const double tol = std::numeric_limits<T>::epsilon() * 10e3;

  double denom = plane.a()*dir.x() +
                 plane.b()*dir.y() +
                 plane.c()*dir.z();

  if (std::abs(denom) < tol)
  {
    // Line is either parallel or coplanar
    // If the distance from a line endpoint to the plane is zero, coplanar
    if (vgl_distance(pt, plane)!=0.0)
      return false;

    const T inf = std::numeric_limits<T>::infinity();
    i_pt.set(inf,inf,inf);
    return true;
  }

  double numer = - plane.a()*pt.x()
                 - plane.b()*pt.y()
                 - plane.c()*pt.z()
                 - plane.d();

  double t = numer/denom;
  i_pt = pt + t * dir;
  return true;
}

template <class T>
bool vgl_intersection(vgl_ray_3d<T> const& ray,
                      vgl_plane_3d<T> const& plane,
                      vgl_point_3d<T> & i_pt)
{
  vgl_vector_3d<T> dir = ray.direction();
  vgl_point_3d<T> pt = ray.origin();
  // The calculation of both denom and numerator are both very dodgy numerically, especially if
  // denom or numerator is small compared to the summands. It would be good to find a more
  // numerically stable solution. IMS.
  const double tol = std::numeric_limits<T>::epsilon() * 10e3;

  double denom = plane.a()*dir.x() +
                 plane.b()*dir.y() +
                 plane.c()*dir.z();

  if (std::abs(denom) < tol)
  {
    // Line is either parallel or coplanar
    // If the distance from a line endpoint to the plane is zero, coplanar
    if (vgl_distance(pt, plane)!=0.0)
      return false;

    const T inf = std::numeric_limits<T>::infinity();
    i_pt.set(inf,inf,inf);
    return true;
  }

  double numer = - plane.a()*pt.x()
                 - plane.b()*pt.y()
                 - plane.c()*pt.z()
                 - plane.d();

  double t = numer/denom;
  if (t<0) return false;
  i_pt = pt + t * dir;
  return true;
}

template <class T>
bool vgl_intersection( const vgl_line_2d<T> &line0,
                       const vgl_line_2d<T> &line1,
                       vgl_point_2d<T>      &intersection_point )
{
  T a0, b0, c0,  a1, b1, c1;
  a0 = line0.a(); b0 = line0.b(); c0 = line0.c();
  a1 = line1.a(); b1 = line1.b(); c1 = line1.c();

  T delta, delta_x, delta_y, x, y;
  delta = a0*b1 - a1*b0;
  if ( std::abs(delta) <= vgl_tolerance<T>::position ) // Lines are parallel
    return false;
  delta_x = -c0*b1 + b0*c1; delta_y = -a0*c1 + a1*c0;
  x = delta_x / delta; y = delta_y / delta;

  //   intersection_point.set( (Type)x, (Type)y );
  intersection_point.set( x, y );
  return true;
}

//: Return the intersection line of two planes. Returns false if planes
// are effectively parallel
// \relatesalso vgl_infinite_line_3d
// \relatesalso vgl_plane_3d
template <class T>
bool vgl_intersection(vgl_plane_3d<T> const& plane0,
                      vgl_plane_3d<T> const& plane1,
                      vgl_infinite_line_3d<T>& line)
{
  double n0x = static_cast<double>(plane0.a());
  double n0y = static_cast<double>(plane0.b());
  double n0z = static_cast<double>(plane0.c());
  double n1x = static_cast<double>(plane1.a());
  double n1y = static_cast<double>(plane1.b());
  double n1z = static_cast<double>(plane1.c());
  vgl_vector_3d<double> n0(n0x, n0y, n0z);
  vgl_vector_3d<double> n1(n1x, n1y, n1z);
  // t is the direction vector of the line
  vgl_vector_3d<double> t = cross_product(n0, n1);
  double mag = t.length();
  if (vgl_near_zero(mag)) // planes are parallel (or coincident)
    return false;
  t/=mag; // create unit vector
  double tx = std::fabs(static_cast<double>(t.x_));
  double ty = std::fabs(static_cast<double>(t.y_));
  double tz = std::fabs(static_cast<double>(t.z_));
  // determine maximum component of t
  char component = 'x';
  if (ty>=tx&&ty>=tz)
    component = 'y';
  if (tz>=tx&&tz>=ty)
    component = 'z';
  double d0 = static_cast<double>(plane0.d());
  double d1 = static_cast<double>(plane1.d());
  vgl_point_3d<double> p0d;
  switch (component)
  {
    // x is the largest component of t
    case 'x':
    {
      double det = n0y*n1z-n1y*n0z;
      if (vgl_near_zero(det))
        return false;
      double neuy = d1*n0z - d0*n1z;
      double neuz = d0*n1y - d1*n0y;
      p0d.set(0.0, neuy/det, neuz/det);
      break;
    }
    case 'y':
    {
      double det = n0x*n1z-n1x*n0z;
      if (vgl_near_zero(det))
        return false;
      double neux = d1*n0z - d0*n1z;
      double neuz = d0*n1x - d1*n0x;
      p0d.set(neux/det, 0.0, neuz/det);
      break;
    }
    case 'z':
    default:
    {
      double det = n0x*n1y-n1x*n0y;
      if (vgl_near_zero(det))
        return false;
      double neux = d1*n0y - d0*n1y;
      double neuy = d0*n1x - d1*n0x;
      p0d.set(neux/det, neuy/det, 0.0);
      break;
    }
  }
  vgl_point_3d<T> p0(static_cast<T>(p0d.x()),
                     static_cast<T>(p0d.y()),
                     static_cast<T>(p0d.z()));
  vgl_vector_3d<T> tt(static_cast<T>(t.x()),
                      static_cast<T>(t.y()),
                      static_cast<T>(t.z()));
  line = vgl_infinite_line_3d<T>(p0, tt);
  return true;
}

//: Return the intersection point of three planes.
// \relatesalso vgl_plane_3d
template <class T>
vgl_point_3d<T> vgl_intersection(const vgl_plane_3d<T>& p1,
                                 const vgl_plane_3d<T>& p2,
                                 const vgl_plane_3d<T>& p3)
{
  vgl_point_3d<T> p(p1, p2, p3);
  return p;
}

//: Return true if any point on [p1,p2] is within tol of [q1,q2]
//  Tests two line segments for intersection or near intersection
//  (within given tolerance).
// \author Dan jackson
template <class T>
bool vgl_intersection(vgl_point_2d<T> const& p1,
                      vgl_point_2d<T> const& p2,
                      vgl_point_2d<T> const& q1,
                      vgl_point_2d<T> const& q2,
                      double tol)
{
  vgl_vector_2d<T> u = p2 - p1;
  vgl_vector_2d<T> v(-u.y(),u.x());
  double uq1 = dot_product(q1 - p1,u);
  double vq1 = dot_product(q1 - p1,v);
  double tol2 = tol*tol;
  double L2 = sqr_length(u);

  // Check if q1 is in central band (either side of line p1-p2
  if (uq1 > 0 && uq1 < L2)
  {
    // Check if q1 is within tol of the line, ie |vq1/L| < tol
    if (vq1*vq1 <=tol2*L2) return true;
  }
  else
  {
    // Check if q1 is within tol of either end of line
    if ( (q1-p1).sqr_length() <= tol2 || (q1-p2).sqr_length() <= tol2 )
      return true;
  }

  // Repeat test for q2
  double uq2 = dot_product(q2 - p1,u);
  double vq2 = dot_product(q2 - p1,v);

  // Check if q2 is in central band (either side of line p1-p2
  if (uq2 > 0 && uq2 < L2)
  {
    // Check if q1 is within tol of the line, ie |vq1/L| < tol
    if (vq2*vq2 <=tol2*L2)
      return true;
  }
  else
  {
    // Check if q1 is within tol of either end of line
    if ( (q2-p1).sqr_length() <= tol2 || (q2-p2).sqr_length() <= tol2 )
      return true;
  }

  // The points q1 and q2 do not lie within the tolerance region
  // around line segment (p1,p2)
  // Now repeat the test the other way around,
  // testing whether points p1 and p2 lie in tolerance region
  // of line (q1,q2)

  u = q2 - q1;
  v.set(-u.y(),u.x());
  L2 = sqr_length(u);

  double up1 = dot_product(p1 - q1,u);
  double vp1 = dot_product(p1 - q1,v);

  // Check if p1 is in central band either side of line q1-q2
  if (up1 > 0 && up1 < L2)
  {
    // Check if p1 is within tol of the line, ie |vp1/L| < tol
    if (vp1*vp1 <=tol2*L2)
      return true;
  }
  else
  {
    // Check if p1 is within tol of either end of line
    if ( (p1-q1).sqr_length() <= tol2 || (p1-q2).sqr_length() <= tol2 )
      return true;
  }

  double up2 = dot_product(p2 - q1,u);
  double vp2 = dot_product(p2 - q1,v);

  // Check if p2 is in central band either side of line q1-q2
  if (up2 > 0 && up2 < L2)
  {
    // Check if p1 is within tol of the line, ie |vp1/L| < tol
    if (vp2*vp2 <=tol2*L2)
      return true;
  }
  else
  {
    // Check if p2 is within tol of either end of line
    if ( (p2-q1).sqr_length() <= tol2 || (p2-q2).sqr_length() <= tol2)
      return true;
  }

  // Now check for actual intersection
  return vq1*vq2 < 0 && vp1*vp2 < 0;
}

template <class T>
bool vgl_intersection(const vgl_box_2d<T>& b,
                      const vgl_polygon<T>& poly)
{
  // easy checks first
  // check if any poly vertices are inside the box
  unsigned int ns = poly.num_sheets();
  bool hit = false;
  for (unsigned int s = 0; s<ns&&!hit; ++s) {
    unsigned int n = (unsigned int)(poly[s].size());
    for (unsigned int i = 0; i<n&&!hit; ++i) {
      vgl_point_2d<T> p = poly[s][i];
      hit = b.contains(p.x(), p.y());
    }
  }
  if (hit) return true;
  // check if any box vertices are inside the polygon
  T minx = b.min_x(), maxx = b.max_x();
  T miny = b.min_y(), maxy = b.max_y();
  hit = poly.contains(minx, miny) || poly.contains(maxx, maxy) ||
    poly.contains(minx, maxy) || poly.contains(maxx, miny);
  if (hit) return true;
  // check if any polygon edges intersect the box
  for (unsigned int s = 0; s<ns&&!hit; ++s)
  {
    unsigned int n = (unsigned int)(poly[s].size());
    vgl_point_2d<T> ia, ib;
    vgl_point_2d<T> last = poly[s][0];
    for (unsigned int i = 1; i<n&&!hit; ++i)
    {
      vgl_point_2d<T> p = poly[s][i];
      vgl_line_segment_2d<T> l(last, p);
      hit = vgl_intersection<T>(b, l, ia, ib)>0;
      last = p;
    }
    if (!hit) {
      vgl_point_2d<T> start = poly[s][0];
      vgl_line_segment_2d<T> ll(last, start);
      hit = vgl_intersection<T>(b,ll, ia, ib)>0;
    }
  }
  return hit;
}

//: Return the points from the list that lie inside the box
// \relatesalso vgl_point_2d
// \relatesalso vgl_box_2d
template <class T>
std::vector<vgl_point_2d<T> > vgl_intersection(vgl_box_2d<T> const& b, std::vector<vgl_point_2d<T> > const& p)
{
  std::vector<vgl_point_2d<T> > r;
  typename std::vector<vgl_point_2d<T> >::const_iterator i;
  for (i = p.begin(); i != p.end(); ++i)
    if (vgl_intersection(b, *i))
      r.push_back(*i);
  return r;
}

//: Return the points from the list that lie inside the box
// \relatesalso vgl_point_2d
// \relatesalso vgl_box_2d
template <class T>
std::vector<vgl_point_2d<T> > vgl_intersection(std::vector<vgl_point_2d<T> > const& p, vgl_box_2d<T> const& b)
{
  std::vector<vgl_point_2d<T> > r;
  typename std::vector<vgl_point_2d<T> >::const_iterator i;
  for (i = p.begin(); i != p.end(); ++i)
    if (vgl_intersection(b, *i))
      r.push_back(*i);
  return r;
}

//: Return the points from the list that lie inside the box
// \relatesalso vgl_point_3d
// \relatesalso vgl_box_3d
template <class T>
std::vector<vgl_point_3d<T> > vgl_intersection(vgl_box_3d<T> const& b, std::vector<vgl_point_3d<T> > const& p)
{
  std::vector<vgl_point_3d<T> > r;
  typename std::vector<vgl_point_3d<T> >::const_iterator i;
  for (i = p.begin(); i != p.end(); ++i)
    if (vgl_intersection(b, *i))
      r.push_back(*i);
  return r;
}

//: Return the points from the list that lie inside the box
// \relatesalso vgl_point_3d
// \relatesalso vgl_box_3d
template <class T>
std::vector<vgl_point_3d<T> > vgl_intersection(std::vector<vgl_point_3d<T> > const& p, vgl_box_3d<T> const& b)
{
  std::vector<vgl_point_3d<T> > r;
  typename std::vector<vgl_point_3d<T> >::const_iterator i;
  for (i = p.begin(); i != p.end(); ++i)
    if (vgl_intersection(b, *i))
      r.push_back(*i);
  return r;
}

template <class T>
std::vector<vgl_point_2d<T> > vgl_intersection(vgl_polygon<T> const& poly,
                                              vgl_line_2d<T> const& line) {
  std::vector<vgl_point_2d<T> > ret;
  T tol = std::sqrt(vgl_tolerance<T>::position);
  T a = line.a(), b = line.b(), c = line.c();
  T norm = std::sqrt(a*a + b*b);
  a/=norm; b/=norm; c/=norm;
  unsigned ns = poly.num_sheets();
  for (unsigned s = 0; s<ns; ++s) {
    std::vector<vgl_point_2d<T> > sh = poly[s];
    unsigned nv = static_cast<unsigned>(sh.size());
    for (unsigned i = 0; i<nv; ++i) {
      unsigned next = (i+1)%nv;
      vgl_point_2d<T> pa = sh[i];
      vgl_point_2d<T> pb = sh[next];
      //algebraic distances
      T ad_a = a*pa.x() +b*pa.y() +c;
      T ad_b = a*pb.x() +b*pb.y() +c;
      bool sign_a = ad_a>T(0);
      bool sign_b = ad_b>T(0);
      bool zero = std::abs(ad_a)<tol;
      //cases
      // 1) no intersections
      // 2) current vertex intersects
      // 3) intersection interior to poly edge
      // case 1
      if (!zero&&(sign_a == sign_b))
        continue;
      // case 2
      if (zero) {
        ret.push_back(pa);
        continue;
      }
      //case 3
      // find the intersection
      vgl_line_2d<T> edge(pa, pb);
      vgl_point_2d<T> p_int;
      if (!vgl_intersection(line, edge, p_int))
        continue;
      ret.push_back(p_int);
    }
  }
  return ret;
}

//: find points that intersect the plane within the specified tolerance on normal distance to the plane.
template <class T>
vgl_pointset_3d<T> vgl_intersection(vgl_plane_3d<T> const& plane, vgl_pointset_3d<T> const& ptset, T tol){
  vgl_pointset_3d<T> ret;
  bool hasn = ptset.has_normals();
  unsigned npts = ptset.npts();
  for(unsigned i = 0; i<npts; ++i){
    vgl_point_3d<T> p = ptset.p(i);
    vgl_point_3d<T> cp = vgl_closest_point(plane, p);
    T d = static_cast<T>((p-cp).length());
    if(d<tol){
      if(hasn){
        vgl_vector_3d<T> norm = ptset.n(i);
        ret.add_point_with_normal(p, norm);
      }else{
        ret.add_point(p);
      }
    }
  }
  return ret;
}
template <class T>
vgl_pointset_3d<T> vgl_intersection(vgl_box_3d<T> const& box, vgl_pointset_3d<T> const& ptset){
  unsigned npts = ptset.npts();
  std::vector<vgl_point_3d<T> > pts;
  std::vector<vgl_vector_3d<T> > normals;
  bool hasn = ptset.has_normals();
  for(unsigned i = 0; i<npts; ++i){
    vgl_point_3d<T> p = ptset.p(i);
    if(box.contains(p)){
      pts.push_back(p);
      if(hasn)
        normals.push_back(ptset.n(i));
    }
  }
  if(hasn)
    return vgl_pointset_3d<T>(pts, normals);
  return vgl_pointset_3d<T>(pts);
}

// Instantiate those functions which are suitable for integer instantiation.
#undef VGL_INTERSECTION_BOX_INSTANTIATE
#define VGL_INTERSECTION_BOX_INSTANTIATE(T) \
template vgl_box_2d<T > vgl_intersection(vgl_box_2d<T > const&,vgl_box_2d<T > const&); \
template vgl_box_3d<T > vgl_intersection(vgl_box_3d<T > const&,vgl_box_3d<T > const&); \
template std::vector<vgl_point_2d<T > > vgl_intersection(vgl_box_2d<T > const&,std::vector<vgl_point_2d<T > > const&); \
template std::vector<vgl_point_2d<T > > vgl_intersection(std::vector<vgl_point_2d<T > > const&,vgl_box_2d<T > const&); \
template std::vector<vgl_point_3d<T > > vgl_intersection(vgl_box_3d<T > const&,std::vector<vgl_point_3d<T > > const&); \
template std::vector<vgl_point_3d<T > > vgl_intersection(std::vector<vgl_point_3d<T > > const&,vgl_box_3d<T > const&); \
template bool vgl_intersection(vgl_box_2d<T > const&,vgl_line_2d<T > const&,vgl_point_2d<T >&,vgl_point_2d<T >&); \
template bool vgl_intersection(vgl_box_2d<T> const&,vgl_line_segment_2d<T> const&, vgl_line_segment_2d<T>& ); \
template bool vgl_intersection(vgl_box_3d<T > const&,vgl_plane_3d<T > const&); \
template bool vgl_intersection(vgl_box_3d<T > const&,vgl_infinite_line_3d<T > const&,vgl_point_3d<T >&,vgl_point_3d<T >&);\
template bool vgl_intersection(vgl_box_3d<T > const&,vgl_ray_3d<T > const&,vgl_point_3d<T >&,vgl_point_3d<T >&)
#undef VGL_INTERSECTION_INSTANTIATE
#define VGL_INTERSECTION_INSTANTIATE(T) \
template vgl_pointset_3d<T> vgl_intersection(vgl_plane_3d<T> const&, vgl_pointset_3d<T> const&, T); \
template vgl_pointset_3d<T> vgl_intersection(vgl_box_3d<T> const&, vgl_pointset_3d<T> const&); \
template vgl_point_3d<T > vgl_intersection(vgl_line_3d_2_points<T > const&,vgl_line_3d_2_points<T > const&); \
template bool vgl_intersection(vgl_line_segment_3d<T > const&,vgl_line_segment_3d<T > const&,vgl_point_3d<T >&); \
template bool vgl_intersection(vgl_line_3d_2_points<T > const&,vgl_line_segment_3d<T > const&,vgl_point_3d<T >&); \
template bool vgl_intersection(vgl_ray_3d<T > const&,vgl_ray_3d<T > const&,vgl_point_3d<T >&); \
template vgl_point_3d<T > vgl_intersection(vgl_line_3d_2_points<T > const&,vgl_plane_3d<T > const&); \
template bool vgl_intersection(vgl_line_segment_3d<T > const&,vgl_plane_3d<T > const&,vgl_point_3d<T >&); \
template bool vgl_intersection(vgl_infinite_line_3d<T > const&,vgl_plane_3d<T > const&,vgl_point_3d<T >&); \
template bool vgl_intersection(vgl_ray_3d<T > const& ray, vgl_plane_3d<T > const& plane, vgl_point_3d<T > & i_pt); \
template bool vgl_intersection(vgl_infinite_line_3d<T > const&,vgl_infinite_line_3d<T > const&,vgl_point_3d<T >&); \
template vgl_point_3d<T > vgl_intersection(vgl_plane_3d<T > const&,vgl_plane_3d<T > const&,vgl_plane_3d<T > const&); \
template unsigned vgl_intersection(vgl_box_2d<T > const&,vgl_line_segment_2d<T > const&,vgl_point_2d<T >&,vgl_point_2d<T >&); \
template bool vgl_intersection(vgl_line_2d<T > const&,vgl_line_2d<T > const&,vgl_point_2d<T >&); \
template bool vgl_intersection(vgl_point_2d<T > const&,vgl_point_2d<T > const&,vgl_point_2d<T > const&,vgl_point_2d<T > const&,double); \
template bool vgl_intersection(vgl_box_2d<T > const&,vgl_polygon<T > const&); \
template bool vgl_intersection(vgl_plane_3d<T > const&,vgl_plane_3d<T > const&,vgl_line_segment_3d<T > &); \
template bool vgl_intersection(vgl_plane_3d<T > const&,vgl_plane_3d<T > const&,vgl_infinite_line_3d<T >&); \
template std::vector<vgl_point_2d<T > > vgl_intersection(vgl_polygon<T > const&, vgl_line_2d<T > const&); \
VGL_INTERSECTION_BOX_INSTANTIATE(T)

#endif // vgl_intersection_hxx_
