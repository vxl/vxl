// This is core/vgl/vgl_distance.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vgl_distance.h"
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_1d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_polygon.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h> // for vcl_sqrt()

template <class Type>
static inline Type square(Type x) { return x*x; }

double vgl_distance2_to_linesegment(double x0, double y0,
                                    double x1, double y1,
                                    double x, double y)
{
  // squared distance between endpoints :
  double ddh = square(x1-x0) + square(y1-y0);

  // squared distance to endpoints :
  double dd0 = square(x-x0) + square(y-y0);
  double dd1 = square(x-x1) + square(y-y1);

  // if closest to the start point :
  if (dd1 > ddh + dd0)
    return dd0;

  // if closest to the end point :
  if (dd0 > ddh + dd1)
    return dd1;

  // squared perpendicular distance to line :
  double a = y0-y1;
  double b = x1-x0;
  double c = x0*y1-x1*y0;
  double ddn = square(a*x + b*y + c)/(a*a + b*b);
  return ddn;
}

double vgl_distance_to_linesegment(double x1, double y1,
                                   double x2, double y2,
                                   double x, double y)
{
  return vcl_sqrt(vgl_distance2_to_linesegment(x1, y1, x2, y2, x, y));
}

#if 0 // deprecated interface; use vgl_distance(vgl_polygon,vgl_point_2d<T>,false)

double vgl_distance_to_non_closed_polygon(float const px[], float const py[], unsigned n,
                                          double x, double y)
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

double vgl_distance_to_closed_polygon(float const px[], float const py[], unsigned n,
                                      double x, double y)
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

#endif // 0

template <class Type>
double vgl_distance_origin(vgl_homg_line_2d<Type> const& l)
{
  if (l.c() == 0) return 0.0; // no call to sqrt if not necessary
  else return vcl_abs(l.c()) / vcl_sqrt( square(l.a())+square(l.b()) );
}

template <class Type>
double vgl_distance_origin(vgl_line_2d<Type> const& l)
{
  if (l.c() == 0) return 0.0; // no call to sqrt if not necessary
  else return vcl_abs(l.c()) / vcl_sqrt( square(l.a())+square(l.b()) );
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
double vgl_distance(vgl_polygon const& poly, vgl_point_2d<Type> const& point, bool closed)
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


template double vgl_distance_origin(vgl_line_2d<float> const&);
template double vgl_distance_origin(vgl_line_2d<double> const&);
template double vgl_distance_origin(vgl_homg_line_2d<float> const&);
template double vgl_distance_origin(vgl_homg_line_2d<double> const&);

template double vgl_distance(vgl_point_2d<float>const&, vgl_point_2d<float>const&);
template double vgl_distance(vgl_point_2d<double>const&, vgl_point_2d<double>const&);
template double vgl_distance(vgl_point_3d<float>const&, vgl_point_3d<float>const&);
template double vgl_distance(vgl_point_3d<double>const&, vgl_point_3d<double>const&);
template double vgl_distance(vgl_homg_point_1d<float>const&, vgl_homg_point_1d<float>const&);
template double vgl_distance(vgl_homg_point_1d<double>const&, vgl_homg_point_1d<double>const&);
template double vgl_distance(vgl_homg_point_2d<float>const&, vgl_homg_point_2d<float>const&);
template double vgl_distance(vgl_homg_point_2d<double>const&, vgl_homg_point_2d<double>const&);
template double vgl_distance(vgl_homg_point_3d<float>const&, vgl_homg_point_3d<float>const&);
template double vgl_distance(vgl_homg_point_3d<double>const&, vgl_homg_point_3d<double>const&);

template double vgl_distance(vgl_line_2d<float> const&, vgl_point_2d<float> const&);
template double vgl_distance(vgl_line_2d<double> const&, vgl_point_2d<double> const&);
template double vgl_distance(vgl_point_2d<float> const&, vgl_line_2d<float> const&);
template double vgl_distance(vgl_point_2d<double> const&, vgl_line_2d<double> const&);
template double vgl_distance(vgl_homg_line_2d<float> const&, vgl_homg_point_2d<float> const&);
template double vgl_distance(vgl_homg_line_2d<double> const&, vgl_homg_point_2d<double> const&);
template double vgl_distance(vgl_homg_point_2d<float> const&, vgl_homg_line_2d<float> const&);
template double vgl_distance(vgl_homg_point_2d<double> const&, vgl_homg_line_2d<double> const&);
template double vgl_distance(vgl_plane_3d<float> const& l, vgl_point_3d<float> const& p);
template double vgl_distance(vgl_plane_3d<double> const& l, vgl_point_3d<double> const& p);
template double vgl_distance(vgl_point_3d<float> const& p, vgl_plane_3d<float> const& l);
template double vgl_distance(vgl_point_3d<double> const& p, vgl_plane_3d<double> const& l);
template double vgl_distance(vgl_homg_plane_3d<float> const& l, vgl_homg_point_3d<float> const& p);
template double vgl_distance(vgl_homg_plane_3d<double> const& l, vgl_homg_point_3d<double> const& p);
template double vgl_distance(vgl_homg_point_3d<float> const& p, vgl_homg_plane_3d<float> const& l);
template double vgl_distance(vgl_homg_point_3d<double> const& p, vgl_homg_plane_3d<double> const& l);

template double vgl_distance(vgl_polygon const&, vgl_point_2d<float> const&);
template double vgl_distance(vgl_polygon const&, vgl_point_2d<double> const&);
