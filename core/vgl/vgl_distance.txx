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
double vgl_distance_to_linesegment(T x1, T y1,
                                   T x2, T y2,
                                   T x, T y)
{
  return vcl_sqrt(vgl_distance2_to_linesegment(x1, y1, x2, y2, x, y));
}

template <class T>
double vgl_distance2_to_linesegment(T x1, T y1,
                                    T x2, T y2,
                                    T x, T y)
{
  // squared distance between endpoints :
  T ddh = square(x2-x1) + square(y2-y1);

  // squared distance to endpoints :
  T dd1 = square(x-x1) + square(y-y1);
  T dd2 = square(x-x2) + square(y-y2);

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
  return square(a*x + b*y + c)/double(a*a + b*b);
}

template <class T>
double vgl_distance_to_linesegment(T x1, T y1, T z1,
                                   T x2, T y2, T z2,
                                   T x, T y, T z)
{
  return vcl_sqrt(vgl_distance2_to_linesegment(x1, y1, z1, x2, y2, z2, x, y, z));
}

template <class T>
double vgl_distance2_to_linesegment(T x1, T y1, T z1,
                                    T x2, T y2, T z2,
                                    T x, T y, T z)
{
  // squared distance between endpoints :
  T ddh = square(x2-x1) + square(y2-y1) + square(z2-z1);

  // squared distance to endpoints :
  T dd1 = square(x-x1) + square(y-y1) + square(z-z1);
  T dd2 = square(x-x2) + square(y-y2) + square(z-z2);

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
  return square(x-x1-lambda*a) + square(y-y1-lambda*b) + square(z-z1-lambda*c);
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
  else return vcl_abs(l.c()) / vcl_sqrt( l.a()*l.a()+l.b()*l.b() );
}

template <class T>
double vgl_distance_origin(vgl_line_2d<T> const& l)
{
  if (l.c() == 0) return 0.0; // no call to sqrt if not necessary
  else return vcl_abs(l.c()) / vcl_sqrt( l.a()*l.a()+l.b()*l.b() );
}

template <class T>
double vgl_distance_origin(vgl_homg_plane_3d<T> const& pl)
{
  if (pl.d() == 0) return 0.0; // no call to sqrt if not necessary
  else return vcl_abs(pl.d()) / vcl_sqrt( pl.a()*pl.a()+pl.b()*pl.b()+pl.c()*pl.c() );
}

template <class T>
double vgl_distance_origin(vgl_plane_3d<T> const& pl)
{
  if (pl.d() == 0) return 0.0; // no call to sqrt if not necessary
  else return vcl_abs(pl.d()) / vcl_sqrt( pl.a()*pl.a()+pl.b()*pl.b()+pl.c()*pl.c() );
}

template <class T>
double vgl_distance_origin(vgl_homg_line_3d_2_points<T> const& l)
{
  vgl_homg_point_3d<T> q = vgl_closest_point_origin(l);
  return vcl_sqrt(square(q.x())+square(q.y())+square(q.z()))/q.w();
}

template <class T>
double vgl_distance_origin(vgl_line_3d_2_points<T> const& l)
{
  vgl_point_3d<T> q = vgl_closest_point_origin(l);
  return vcl_sqrt(square(q.x())+square(q.y())+square(q.z()));
}

template <class T>
double vgl_distance(vgl_homg_point_1d<T>const& p1,
                    vgl_homg_point_1d<T>const& p2)
{
  return vcl_abs(p1.x()/p1.w() - p2.x()/p2.w());
}

template <class T>
double vgl_distance(vgl_line_2d<T> const& l, vgl_point_2d<T> const& p)
{
  T num = l.a()*p.x() + l.b()*p.y() + l.c();
  if (num == 0) return 0.0; // no call to sqrt if not necessary
  else return vcl_abs(num) / vcl_sqrt(l.a()*l.a() + l.b()*l.b());
}

template <class T>
double vgl_distance(vgl_homg_line_2d<T> const& l, vgl_homg_point_2d<T> const& p)
{
  T num = l.a()*p.x() + l.b()*p.y() + l.c()*p.w();
  if (num == 0) return 0.0; // always return 0 when point on line, even at infinity
  else return vcl_abs(num) / vcl_sqrt(l.a()*l.a() + l.b()*l.b()) / p.w(); // could be inf
}

template <class T>
double vgl_distance(vgl_plane_3d<T> const& l, vgl_point_3d<T> const& p)
{
  T num = l.nx()*p.x() + l.ny()*p.y() + l.nz()*p.z() + l.d();
  if (num == 0) return 0.0; // no call to sqrt if not necessary
  else return vcl_abs(num) / vcl_sqrt(l.nx()*l.nx() + l.ny()*l.ny() + l.nz()*l.nz());
}

template <class T>
double vgl_distance(vgl_homg_plane_3d<T> const& l, vgl_homg_point_3d<T> const& p)
{
  T num = l.nx()*p.x() + l.ny()*p.y() + l.nz()*p.z() + l.d()*p.w();
  if (num == 0) return 0.0; // always return 0 when point on plane, even at infinity
  else return vcl_abs(num/p.w()) / vcl_sqrt(l.nx()*l.nx() + l.ny()*l.ny() + l.nz()*l.nz());
}

template <class T>
double vgl_distance(vgl_polygon<T> const& poly, vgl_point_2d<T> const& point, bool closed)
{
  double dist = -1;
  for ( unsigned int s=0; s < poly.num_sheets(); ++s )
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
    for ( unsigned int i=0; i+1 < n; ++i )
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

template <class T>
double vgl_distance(vgl_homg_line_3d_2_points<T> const& line1,
                    vgl_homg_line_3d_2_points<T> const& line2)
{
  vcl_pair<vgl_homg_point_3d<T>, vgl_homg_point_3d<T> > pp =
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
template double vgl_distance(vgl_polygon<T >const&,vgl_point_2d<T >const&,bool); \
template double vgl_distance(vgl_homg_line_3d_2_points<T >const&,vgl_homg_line_3d_2_points<T >const&); \
template double vgl_distance(vgl_homg_line_3d_2_points<T >const&,vgl_homg_point_3d<T >const&); \
template double vgl_distance(vgl_line_3d_2_points<T >const&,vgl_point_3d<T >const&); \
template double vgl_distance(vgl_homg_point_3d<T > const&,vgl_homg_line_3d_2_points<T > const&)

#endif // vgl_distance_txx_
