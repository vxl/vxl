// This is core/vgl/vgl_distance.txx
#ifndef vgl_distance_txx_
#define vgl_distance_txx_
//:
// \file
// \author fsm

#include "vgl_distance.h"
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_1d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_closest_point.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h> // for vcl_sqrt()
#include <vcl_utility.h> // for vcl_pair<T,U>

template <class T>
static inline T square(T x) { return x*x; }

template <class T>
T vgl_distance2_to_linesegment(T x0, T y0,
                               T x1, T y1,
                               T x, T y)
{
  // squared distance between endpoints :
  T ddh = square(x1-x0) + square(y1-y0);

  // squared distance to endpoints :
  T dd0 = square(x-x0) + square(y-y0);
  T dd1 = square(x-x1) + square(y-y1);

  // if closest to the start point :
  if (dd1 > ddh + dd0)
    return dd0;

  // if closest to the end point :
  if (dd0 > ddh + dd1)
    return dd1;

  // squared perpendicular distance to line :
  T a = y0-y1;
  T b = x1-x0;
  T c = x0*y1-x1*y0;
  T ddn = square(a*x + b*y + c)/(a*a + b*b);
  return ddn;
}

template <class T>
double vgl_distance_to_linesegment(T x1, T y1,
                                   T x2, T y2,
                                   T x, T y)
{
  return vcl_sqrt(vgl_distance2_to_linesegment(x1, y1, x2, y2, x, y));
}

template <class T>
double vgl_distance_to_non_closed_polygon(T const px[], T const py[], unsigned n,
                                          T x, T y)
{
  double dd = -1;
  for (unsigned i=0; i<n-1; ++i) {
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
  for (unsigned i=0; i<n-1; ++i) {
    double nd = vgl_distance_to_linesegment(px[i  ], py[i  ],
                                            px[i+1], py[i+1],
                                            x, y);
    if (nd<dd)
      dd = nd;
  }

  return dd;
}

template <class Type>
double vgl_distance_origin(vgl_homg_line_2d<Type> const& l)
{
  if (l.c() == 0) return 0.0; // no call to sqrt if not necessary
  else return vcl_abs(l.c()) / vcl_sqrt( l.a()*l.a()+l.b()*l.b() );
}

template <class Type>
double vgl_distance_origin(vgl_line_2d<Type> const& l)
{
  if (l.c() == 0) return 0.0; // no call to sqrt if not necessary
  else return vcl_abs(l.c()) / vcl_sqrt( l.a()*l.a()+l.b()*l.b() );
}

template <class Type>
double vgl_distance(vgl_homg_point_1d<Type>const& p1,
                    vgl_homg_point_1d<Type>const& p2)
{
  return vcl_abs(p1.x()/p1.w() - p2.x()/p2.w());
}

template <class Type>
double vgl_distance(vgl_line_2d<Type> const& l, vgl_point_2d<Type> const& p)
{
  Type num = l.a()*p.x() + l.b()*p.y() + l.c();
  if (num == 0) return 0.0; // no call to sqrt if not necessary
  else return vcl_abs(num) / vcl_sqrt(l.a()*l.a() + l.b()*l.b());
}

template <class Type>
double vgl_distance(vgl_homg_line_2d<Type> const& l, vgl_homg_point_2d<Type> const& p)
{
  Type num = l.a()*p.x() + l.b()*p.y() + l.c()*p.w();
  if (num == 0) return 0.0; // always return 0 when point on line, even at infinity
  else return vcl_abs(num) / vcl_sqrt(l.a()*l.a() + l.b()*l.b()) / p.w(); // could be inf
}

template <class Type>
double vgl_distance(vgl_plane_3d<Type> const& l, vgl_point_3d<Type> const& p)
{
  Type num = l.nx()*p.x() + l.ny()*p.y() + l.nz()*p.z() + l.d();
  if (num == 0) return 0.0; // no call to sqrt if not necessary
  else return vcl_abs(num) / vcl_sqrt(l.nx()*l.nx() + l.ny()*l.ny() + l.nz()*l.nz());
}

template <class Type>
double vgl_distance(vgl_homg_plane_3d<Type> const& l, vgl_homg_point_3d<Type> const& p)
{
  Type num = l.nx()*p.x() + l.ny()*p.y() + l.nz()*p.z() + l.d()*p.w();
  if (num == 0) return 0.0; // always return 0 when point on plane, even at infinity
  else return vcl_abs(num/p.w()) / vcl_sqrt(l.nx()*l.nx() + l.ny()*l.ny() + l.nz()*l.nz());
}

template <class Type>
double vgl_distance(vgl_polygon<Type> const& poly, vgl_point_2d<Type> const& point, bool closed)
{
  double dist = -1;
  for ( int s=0; s < poly.num_sheets(); ++s )
  {
    unsigned int n = poly[s].size();
    assert( n > 1 );
    double dd = closed ?
                vgl_distance_to_linesegment(poly[s][n-1].x(), poly[s][n-1].y(),
                                            poly[s][0  ].x(), poly[s][0  ].y(),
                                            point.x(), point.y()) :
                vgl_distance_to_linesegment(poly[s][0  ].x(), poly[s][0  ].y(),
                                            poly[s][1  ].x(), poly[s][1  ].y(),
                                            point.x(), point.y());
    for ( unsigned int i=0; i < n-1; ++i )
    {
      double nd = vgl_distance_to_linesegment(poly[s][i  ].x(), poly[s][i  ].y(),
                                              poly[s][i+1].x(), poly[s][i+1].y(),
                                              point.x(), point.y());
      if ( nd<dd )  dd=nd;
    }
    if ( dist < 0 || dd < dist )  dist = dd;
  }

  return dist;
}

template <class Type>
double vgl_distance(vgl_homg_line_3d_2_points<Type> const& line1,
                    vgl_homg_line_3d_2_points<Type> const& line2)
{
  vcl_pair<vgl_homg_point_3d<Type>, vgl_homg_point_3d<Type> > pp =
    vgl_closest_points(line1, line2);
  if (pp.first.w() != 0)
    return vgl_distance(pp.first,pp.second);
  else // the two lines are parallel
    return vgl_distance(line1.point_finite(), line2);
}


template <class Type>
double vgl_distance(vgl_homg_line_3d_2_points<Type> const& l,
                    vgl_homg_point_3d<Type> const& p)
{
  vgl_homg_point_3d<Type> q = vgl_closest_point(l, p);
  return vgl_distance(p,q);
}

#undef VGL_DISTANCE_INSTANTIATE
#define VGL_DISTANCE_INSTANTIATE(T) \
template T      vgl_distance2_to_linesegment(T,T,T,T,T,T); \
template double vgl_distance_to_linesegment(T,T,T,T,T,T); \
template double vgl_distance_to_non_closed_polygon(T const[],T const[],unsigned int,T,T); \
template double vgl_distance_to_closed_polygon(T const[],T const[],unsigned int,T,T); \
template double vgl_distance_origin(vgl_homg_line_2d<T >const&); \
template double vgl_distance_origin(vgl_line_2d<T >const&); \
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
template double vgl_distance(vgl_polygon<T >const&,vgl_point_2d<T >const&,bool); \
template double vgl_distance(vgl_homg_line_3d_2_points<T >const&,vgl_homg_line_3d_2_points<T >const&); \
template double vgl_distance(vgl_homg_line_3d_2_points<T >const&,vgl_homg_point_3d<T >const&); \
template double vgl_distance(vgl_homg_point_3d<T > const&,vgl_homg_line_3d_2_points<T > const&)

#endif // vgl_distance_txx_
