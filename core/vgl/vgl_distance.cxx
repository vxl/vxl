/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vgl_distance.h"

static inline float square(float x) { return x*x; }
static inline double square(double x) { return x*x; }

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
  return sqrt(vgl_distance2_to_linesegment(x1, y1, x2, y2, x, y));
}

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
  double dd = -1;
  for (unsigned i=0; i<n-1; ++i) {
    double nd = vgl_distance_to_linesegment(px[i  ], py[i  ],
                                            px[i+1], py[i+1],
                                            x, y);
    if (dd<0 || nd<dd)
      dd = nd;
  }
  double nd = vgl_distance_to_linesegment(px[n-1], py[n-1],
                                          px[0  ], py[0  ],
                                          x, y);
  if (dd<0 || nd<dd)
    dd = nd;

  return dd;
}

template <class Type>
Type dist_origin(vgl_homg_line_2d<Type> const& l)
{
  return l.c() / sqrt( square(l.a())+square(l.b()) );
}

template <class Type>
Type dist_origin(vgl_line_2d<Type> const& l)
{
  return l.c() / sqrt( square(l.a())+square(l.b()) );
}

template float dist_origin(vgl_line_2d<float> const&);
template double dist_origin(vgl_line_2d<double> const&);
template float dist_origin(vgl_homg_line_2d<float> const&);
template double dist_origin(vgl_homg_line_2d<double> const&);

template <class Type>
Type vgl_distance(vgl_point_2d<Type>const& p1, vgl_point_2d<Type>const& p2)
{
  return sqrt(square(p1.x()-p2.x())+square(p1.y()-p2.y()));
}

template <class Type>
Type vgl_distance(vgl_point_3d<Type>const& p1, vgl_point_3d<Type>const& p2)
{
  return sqrt(square(p1.x()-p2.x())+square(p1.y()-p2.y())+square(p1.z()-p2.z()));
}

template <class Type>
Type vgl_distance(vgl_homg_point_2d<Type>const& p1, vgl_homg_point_2d<Type>const& p2)
{
  return sqrt(square(p1.x()/p1.w()-p2.x()/p2.w())+square(p1.y()/p1.w()-p2.y()/p2.w()));
}

template <class Type>
Type vgl_distance(vgl_homg_point_3d<Type>const& p1, vgl_homg_point_3d<Type>const& p2)
{
  return sqrt(square(p1.x()/p1.w()-p2.x()/p2.w())+square(p1.y()/p1.w()-p2.y()/p2.w())+square(p1.z()/p1.w()-p2.z()/p2.w()));
}

template float vgl_distance(vgl_point_2d<float>const&, vgl_point_2d<float>const&);
template double vgl_distance(vgl_point_2d<double>const&, vgl_point_2d<double>const&);
template float vgl_distance(vgl_point_3d<float>const&, vgl_point_3d<float>const&);
template double vgl_distance(vgl_point_3d<double>const&, vgl_point_3d<double>const&);
template float vgl_distance(vgl_homg_point_2d<float>const&, vgl_homg_point_2d<float>const&);
template double vgl_distance(vgl_homg_point_2d<double>const&, vgl_homg_point_2d<double>const&);
template float vgl_distance(vgl_homg_point_3d<float>const&, vgl_homg_point_3d<float>const&);
template double vgl_distance(vgl_homg_point_3d<double>const&, vgl_homg_point_3d<double>const&);
