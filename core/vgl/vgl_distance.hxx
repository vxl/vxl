// This is core/vgl/vgl_distance.hxx
#ifndef vgl_distance_hxx_
#define vgl_distance_hxx_
//:
// \file
// \author fsm

#include <cmath>
#include <utility>
#include "vgl_distance.h"
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_1d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_closest_point.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
static inline T square_of(T x) { return x*x; }

template <class T>
double vgl_distance_to_linesegment(T x1, T y1,
                                   T x2, T y2,
                                   T x, T y)
{
  return std::sqrt(vgl_distance2_to_linesegment(x1, y1, x2, y2, x, y));
}

template <class T>
double vgl_distance2_to_linesegment(T x1, T y1,
                                    T x2, T y2,
                                    T x, T y)
{
  // squared distance between endpoints :
  T ddh = square_of(x2-x1) + square_of(y2-y1);

  // squared distance to endpoints :
  T dd1 = square_of(x-x1) + square_of(y-y1);
  T dd2 = square_of(x-x2) + square_of(y-y2);

  // if closest to the start point :
  if (dd2 >= ddh + dd1)
    return dd1;

  // if closest to the end point :
  if (dd1 >= ddh + dd2)
    return dd2;

  // squared perpendicular distance to line :
  T a = y1-y2;
  T b = x2-x1;
  T c = x1*y2-x2*y1;
  return square_of(a*x + b*y + c)/double(a*a + b*b);
}

template <class T>
double vgl_distance_to_linesegment(T x1, T y1, T z1,
                                   T x2, T y2, T z2,
                                   T x, T y, T z)
{
  return std::sqrt(vgl_distance2_to_linesegment(x1, y1, z1, x2, y2, z2, x, y, z));
}

template <class T>
double vgl_distance2_to_linesegment(T x1, T y1, T z1,
                                    T x2, T y2, T z2,
                                    T x, T y, T z)
{
  // squared distance between endpoints :
  T ddh = square_of(x2-x1) + square_of(y2-y1) + square_of(z2-z1);

  // squared distance to endpoints :
  T dd1 = square_of(x-x1) + square_of(y-y1) + square_of(z-z1);
  T dd2 = square_of(x-x2) + square_of(y-y2) + square_of(z-z2);

  // if closest to the start point :
  if (dd2 >= ddh + dd1)
    return dd1;

  // if closest to the end point :
  if (dd1 >= ddh + dd2)
    return dd2;

  // squared perpendicular distance to line :
  // plane through (x,y,z) and orthogonal to the line is a(X-x)+b(Y-y)+c(Z-z)=0
  // where (a,b,c) is the direction of the line.
  T a = x2-x1, b = y2-y1, c = z2-z1;
  // The closest point is then the intersection of this plane with the line.
  // This point equals (x1,y1,z1) + lambda * (a,b,c), with this lambda:
  double lambda = (a*(x-x1)+b*(y-y1)+c*(z-z1))/double(a*a+b*b+c*c);
  // return squared distance:
  return square_of(x-x1-lambda*a) + square_of(y-y1-lambda*b) + square_of(z-z1-lambda*c);
}

template <class T>
double vgl_distance_to_non_closed_polygon(T const px[], T const py[], unsigned n,
                                          T x, T y)
{
  double dd = -1;
  for (unsigned i=0; i+1<n; ++i) {
    double nd = vgl_distance_to_linesegment(px[i  ], py[i  ],
                                            px[i+1], py[i+1],
                                            x, y);
    if (dd<0 || nd<dd)
      dd = nd;
  }
  return dd;
}

template <class T>
double vgl_distance_to_closed_polygon(T const px[], T const py[], unsigned n,
                                      T x, T y)
{
  double dd = vgl_distance_to_linesegment(px[n-1], py[n-1],
                                          px[0  ], py[0  ],
                                          x, y);
  for (unsigned i=0; i+1<n; ++i) {
    double nd = vgl_distance_to_linesegment(px[i  ], py[i  ],
                                            px[i+1], py[i+1],
                                            x, y);
    if (nd<dd)
      dd = nd;
  }

  return dd;
}

template <class T>
double vgl_distance_to_non_closed_polygon(T const px[], T const py[], T const pz[], unsigned int n,
                                          T x, T y, T z)
{
  double dd = -1;
  for (unsigned i=0; i+1<n; ++i) {
    double nd = vgl_distance_to_linesegment(px[i  ], py[i  ], pz[i  ],
                                            px[i+1], py[i+1], pz[i+1],
                                            x, y, z);
    if (dd<0 || nd<dd)
      dd = nd;
  }
  return dd;
}

template <class T>
double vgl_distance_to_closed_polygon(T const px[], T const py[], T const pz[], unsigned int n,
                                      T x, T y, T z)
{
  double dd = vgl_distance_to_linesegment(px[n-1], py[n-1], pz[n-1],
                                          px[0  ], py[0  ], pz[0  ],
                                          x, y, z);
  for (unsigned i=0; i+1<n; ++i) {
    double nd = vgl_distance_to_linesegment(px[i  ], py[i  ], pz[i  ],
                                            px[i+1], py[i+1], pz[i+1],
                                            x, y, z);
    if (nd<dd)
      dd = nd;
  }

  return dd;
}

template <class T>
double vgl_distance_origin(vgl_homg_line_2d<T> const& l)
{
  if (l.c() == 0) return 0.0; // no call to sqrt if not necessary
  else return std::abs(static_cast<double>(l.c())) / std::sqrt(static_cast<double>( l.a()*l.a()+l.b()*l.b() ));
}

template <class T>
double vgl_distance_origin(vgl_line_2d<T> const& l)
{
  if (l.c() == 0) return 0.0; // no call to sqrt if not necessary
  else return std::abs(static_cast<double>(l.c())) / std::sqrt(static_cast<double>( l.a()*l.a()+l.b()*l.b() ));
}

template <class T>
double vgl_distance_origin(vgl_homg_plane_3d<T> const& pl)
{
  if (pl.d() == 0) return 0.0; // no call to sqrt if not necessary
  else return std::abs(static_cast<double>(pl.d())) / std::sqrt(static_cast<double>( pl.a()*pl.a()+pl.b()*pl.b()+pl.c()*pl.c()) );
}

template <class T>
double vgl_distance_origin(vgl_plane_3d<T> const& pl)
{
  if (pl.d() == 0) return 0.0; // no call to sqrt if not necessary
  else return std::abs(static_cast<double>(pl.d())) / std::sqrt(static_cast<double>( pl.a()*pl.a()+pl.b()*pl.b()+pl.c()*pl.c()) );
}

template <class T>
double vgl_distance_origin(vgl_homg_line_3d_2_points<T> const& l)
{
  vgl_homg_point_3d<T> q = vgl_closest_point_origin(l);
  return std::sqrt(static_cast<double>(square_of(q.x())+square_of(q.y())+square_of(q.z())))/q.w();
}

template <class T>
double vgl_distance_origin(vgl_line_3d_2_points<T> const& l)
{
  vgl_point_3d<T> q = vgl_closest_point_origin(l);
  return std::sqrt(static_cast<double>(square_of(q.x())+square_of(q.y())+square_of(q.z())));
}

template <class T>
double vgl_distance(vgl_homg_point_1d<T>const& p1,
                    vgl_homg_point_1d<T>const& p2)
{
  return std::abs(static_cast<double>(p1.x()/p1.w() - p2.x()/p2.w()));
}

template <class T>
double vgl_distance(vgl_line_2d<T> const& l, vgl_point_2d<T> const& p)
{
  T num = l.a()*p.x() + l.b()*p.y() + l.c();
  if (num == 0) return 0.0; // no call to sqrt if not necessary
  else return std::abs(static_cast<double>(num)) / std::sqrt(static_cast<double>(l.a()*l.a() + l.b()*l.b()));
}

template <class T>
double vgl_distance(vgl_homg_line_2d<T> const& l, vgl_homg_point_2d<T> const& p)
{
  T num = l.a()*p.x() + l.b()*p.y() + l.c()*p.w();
  if (num == 0) return 0.0; // always return 0 when point on line, even at infinity
  else return std::abs(static_cast<double>(num)) / std::sqrt(static_cast<double>(l.a()*l.a() + l.b()*l.b())) / p.w(); // could be inf
}

template <class T>
double vgl_distance(vgl_plane_3d<T> const& l, vgl_point_3d<T> const& p)
{
  T num = l.nx()*p.x() + l.ny()*p.y() + l.nz()*p.z() + l.d();
  if (num == 0) return 0.0; // no call to sqrt if not necessary
  else return std::abs(static_cast<double>(num)) / std::sqrt(static_cast<double>(l.nx()*l.nx() + l.ny()*l.ny() + l.nz()*l.nz()));
}

template <class T>
double vgl_distance(vgl_homg_plane_3d<T> const& l, vgl_homg_point_3d<T> const& p)
{
  T num = l.nx()*p.x() + l.ny()*p.y() + l.nz()*p.z() + l.d()*p.w();
  if (num == 0) return 0.0; // always return 0 when point on plane, even at infinity
  else return std::abs(static_cast<double>(num/p.w())) / std::sqrt(static_cast<double>(l.nx()*l.nx() + l.ny()*l.ny() + l.nz()*l.nz()));
}
template <class T>
double vgl_distance(vgl_point_3d<T> const& p,
                    vgl_sphere_3d<T> const& s){
  double r = static_cast<double>(s.radius());
  vgl_point_3d<T> c = s.centre();
  double d = vgl_distance<T>(p,c);
  return std::fabs(d-r);
}

template <class T>
double vgl_distance(vgl_polygon<T> const& poly, vgl_point_2d<T> const& point, bool closed)
{
  double dist = -1;
  for ( unsigned int s=0; s < poly.num_sheets(); ++s )
  {
    const std::vector<vgl_point_2d<T> > &sheet = poly[s];
    unsigned int n = (unsigned int)(sheet.size());
    assert( n > 1 );
    double dd = closed ?
                vgl_distance_to_linesegment(sheet[n-1].x(), sheet[n-1].y(),
                                            sheet[0  ].x(), sheet[0  ].y(),
                                            point.x(), point.y()) :
                vgl_distance_to_linesegment(sheet[0  ].x(), sheet[0  ].y(),
                                            sheet[1  ].x(), sheet[1  ].y(),
                                            point.x(), point.y());
    for ( unsigned int i=0; i+1 < n; ++i )
    {
      double nd = vgl_distance_to_linesegment(sheet[i  ].x(), sheet[i  ].y(),
                                              sheet[i+1].x(), sheet[i+1].y(),
                                              point.x(), point.y());
      if ( nd<dd )  dd=nd;
    }
    if ( dist < 0 || dd < dist )  dist = dd;
  }

  return dist;
}

template <class T>
double vgl_distance(vgl_homg_line_3d_2_points<T> const& line1,
                    vgl_homg_line_3d_2_points<T> const& line2)
{
  std::pair<vgl_homg_point_3d<T>, vgl_homg_point_3d<T> > pp =
    vgl_closest_points(line1, line2);
  if (pp.first.w() != 0)
    return vgl_distance(pp.first,pp.second);
  else // the two lines are parallel
    return vgl_distance(line1.point_finite(), line2);
}


template <class T>
double vgl_distance(vgl_homg_line_3d_2_points<T> const& l,
                    vgl_homg_point_3d<T> const& p)
{
  vgl_homg_point_3d<T> q = vgl_closest_point(l, p);
  return vgl_distance(p,q);
}

template <class T>
double vgl_distance(vgl_line_3d_2_points<T> const& l,
                    vgl_point_3d<T> const& p)
{
  vgl_point_3d<T> q = vgl_closest_point(l, p);
  return vgl_distance(p,q);
}

template <class T>
double vgl_distance(vgl_line_segment_2d<T> const& l,
                    vgl_point_2d<T> const& p)
{
  return vgl_distance_to_linesegment(l.point1().x(), l.point1().y(),
                                     l.point2().x(), l.point2().y(),
                                     p.x(), p.y());
}

template <class T>
double vgl_distance(vgl_line_segment_3d<T> const& l,
                    vgl_point_3d<T> const& p)
{
  return vgl_distance_to_linesegment(l.point1().x(), l.point1().y(), l.point1().z(),
                                     l.point2().x(), l.point2().y(), l.point2().z(),
                                     p.x(), p.y(), p.z());
}

//: return the closest distance from a point to a ray
template <class T>
double vgl_distance(vgl_ray_3d<T> const& r, vgl_point_3d<T> const& p)
{
  vgl_point_3d<T> q = vgl_closest_point(r, p);
  return vgl_distance(p,q);
}
//: return the closest distance from a point to an infinite line
template <class T>
double vgl_distance(vgl_infinite_line_3d<T> const& l,
                    vgl_point_3d<T> const& p){
  vgl_point_3d<T> q = vgl_closest_point(l, p);
  return vgl_distance(p,q);
}

template <class T>
double vgl_distance(vgl_point_2d<T> const& p, vgl_box_2d<T> const& b){
  //create line segments for the box boundary
  vgl_point_2d<T> p0 = b.min_point();
  vgl_point_2d<T> p2 = b.max_point();
  vgl_point_2d<T> p1(p2.x(), p0.y());
  vgl_point_2d<T> p3(p0.x(), p2.y());
  // find distance to each line segment and return minimum
  vgl_line_segment_2d<T> l0(p0, p1), l1(p1, p2), l2(p2,p3), l3(p3,p0);
  T min_d = std::numeric_limits<T>::max();
  double d = vgl_distance(p,l0);
  if(d < min_d) min_d = d;
  d = vgl_distance(p,l1);
  if(d < min_d) min_d = d;
  d = vgl_distance(p,l2);
  if(d < min_d) min_d = d;
  d = vgl_distance(p,l3);
  if(d < min_d) min_d = d;
  return min_d;
}

#undef VGL_DISTANCE_INSTANTIATE
#define VGL_DISTANCE_INSTANTIATE(T) \
template double vgl_distance2_to_linesegment(T,T,T,T,T,T); \
template double vgl_distance_to_linesegment(T,T,T,T,T,T); \
template double vgl_distance2_to_linesegment(T,T,T,T,T,T,T,T,T); \
template double vgl_distance_to_linesegment(T,T,T,T,T,T,T,T,T); \
template double vgl_distance_to_non_closed_polygon(T const[],T const[],unsigned int,T,T); \
template double vgl_distance_to_non_closed_polygon(T const[],T const[],T const[],unsigned int,T,T,T); \
template double vgl_distance_to_closed_polygon(T const[],T const[],unsigned int,T,T); \
template double vgl_distance_to_closed_polygon(T const[],T const[],T const[],unsigned int,T,T,T); \
template double vgl_distance_origin(vgl_line_2d<T >const& l); \
template double vgl_distance_origin(vgl_homg_line_2d<T >const& l); \
template double vgl_distance_origin(vgl_plane_3d<T > const& pl); \
template double vgl_distance_origin(vgl_homg_plane_3d<T > const& pl); \
template double vgl_distance_origin(vgl_line_3d_2_points<T > const& l); \
template double vgl_distance_origin(vgl_homg_line_3d_2_points<T > const& l); \
template double vgl_distance(vgl_homg_point_1d<T >const&,vgl_homg_point_1d<T >const&); \
template double vgl_distance(vgl_point_2d<T >const&,vgl_point_2d<T >const&); \
template double vgl_distance(vgl_homg_point_2d<T >const&,vgl_homg_point_2d<T >const&); \
template double vgl_distance(vgl_point_3d<T >const&,vgl_point_3d<T >const&); \
template double vgl_distance(vgl_homg_point_3d<T >const&,vgl_homg_point_3d<T >const&); \
template double vgl_distance(vgl_line_2d<T >const&,vgl_point_2d<T >const&); \
template double vgl_distance(vgl_point_2d<T >const&,vgl_line_2d<T >const&); \
template double vgl_distance(vgl_homg_line_2d<T >const&,vgl_homg_point_2d<T >const&); \
template double vgl_distance(vgl_homg_point_2d<T >const&,vgl_homg_line_2d<T >const&); \
template double vgl_distance(vgl_plane_3d<T >const&,vgl_point_3d<T >const&); \
template double vgl_distance(vgl_point_3d<T >const&,vgl_plane_3d<T >const&); \
template double vgl_distance(vgl_homg_plane_3d<T >const&,vgl_homg_point_3d<T >const&); \
template double vgl_distance(vgl_homg_point_3d<T >const&,vgl_homg_plane_3d<T >const&); \
template double vgl_distance(vgl_point_3d<T> const&, vgl_sphere_3d<T> const&); \
template double vgl_distance(vgl_polygon<T >const&,vgl_point_2d<T >const&,bool); \
template double vgl_distance(vgl_homg_line_3d_2_points<T >const&,vgl_homg_line_3d_2_points<T >const&); \
template double vgl_distance(vgl_homg_line_3d_2_points<T >const&,vgl_homg_point_3d<T >const&); \
template double vgl_distance(vgl_line_3d_2_points<T >const&,vgl_point_3d<T >const&); \
template double vgl_distance(vgl_ray_3d<T > const& r, vgl_point_3d<T > const& p); \
template double vgl_distance(vgl_infinite_line_3d<T > const& l, vgl_point_3d<T > const& p); \
template double vgl_distance(vgl_homg_point_3d<T > const&,vgl_homg_line_3d_2_points<T > const&); \
template double vgl_distance(vgl_line_segment_2d<T > const&, vgl_point_2d<T > const&); \
template double vgl_distance(vgl_line_segment_3d<T > const&, vgl_point_3d<T > const&); \
template double vgl_distance(vgl_point_2d<T> const& p, vgl_box_2d<T> const& b)
#endif // vgl_distance_hxx_
