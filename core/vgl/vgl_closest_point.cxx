// This is core/vgl/vgl_closest_point.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Peter Vanroose, KULeuven, ESAT/PSI

#include "vgl_closest_point.h"
#include <vgl/vgl_distance.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_polygon.h>
#include <vcl_cassert.h>

void vgl_closest_point_to_linesegment(double& ret_x, double& ret_y,
                                      double x1, double y1,
                                      double x2, double y2,
                                      double x0, double y0)
{
  // squared distance between endpoints:
  double ddh = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);

  // squared distance to endpoints:
  double dd0 = (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1);
  double dd1 = (x0-x2)*(x0-x2) + (y0-y2)*(y0-y2);

  // if closest to the start point:
  if (dd1 > ddh + dd0) { ret_x=x1; ret_y=y1; return; }

  // if closest to the end point :
  if (dd0 > ddh + dd1) { ret_x=x2; ret_y=y2; return; }

  // line through (x0,y0) and perpendicular to the given line is
  // the line with equation (x-x0)(x2-x1)+(y-y0)(y2-y1)=0.
  // Then it just remains to intersect these two lines:
  double dx = x2-x1;
  double dy = y2-y1;
  double c = dx*dx+dy+dy;
  ret_x = (dx*dx*x0+dy*dy*x1-dx*dy*(y0-y1))/c;
  ret_y = (dx*dx*y1+dy*dy*y0-dx*dy*(x1-x0))/c;
  return;
}

void vgl_closest_point_to_non_closed_polygon(double& ret_x, double& ret_y,
                                             float const px[], float const py[], unsigned n,
                                             double x, double y)
{
  assert(n>1);
  double dd = vgl_distance_to_linesegment(px[0],py[0], px[1],py[1], x,y);
  int di = 0;
  for (unsigned i=1; i<n-1; ++i)
  {
    double nd = vgl_distance_to_linesegment(px[i],py[i], px[i+1],py[i+1], x,y);
    if (nd<dd) { dd=nd; di=i; }
  }
  vgl_closest_point_to_linesegment(ret_x,ret_y, px[di],py[di], px[di+1],py[di+1], x,y);
}

void vgl_closest_point_to_closed_polygon(double& ret_x, double& ret_y,
                                         float const px[], float const py[], unsigned n,
                                         double x, double y)
{
  assert(n>1);
  double dd = vgl_distance_to_linesegment(px[0],py[0], px[n-1],py[n-1], x,y);
  int di = -1;
  for (unsigned i=0; i<n-1; ++i)
  {
    double nd = vgl_distance_to_linesegment(px[i],py[i], px[i+1],py[i+1], x,y);
    if (nd<dd) { dd=nd; di=i; }
  }
  if (di == -1)
    vgl_closest_point_to_linesegment(ret_x,ret_y, px[0],py[0], px[n-1],py[n-1], x,y);
  else
    vgl_closest_point_to_linesegment(ret_x,ret_y, px[di],py[di], px[di+1],py[di+1], x,y);
}

template <class Type>
vgl_point_2d<Type> vgl_closest_point_origin(vgl_line_2d<Type> const& l)
{
  Type d = l.a()*l.a()+l.b()*l.b();
  return vgl_point_2d<Type>(-l.a()*l.c()/d, -l.b()*l.c()/d);
}

template <class Type>
vgl_homg_point_2d<Type> vgl_closest_point_origin(vgl_homg_line_2d<Type> const& l)
{
  return vgl_homg_point_2d<Type>(l.a()*l.c(), l.b()*l.c(),
                                 -l.a()*l.a()-l.b()*l.b());
}

template <class Type>
vgl_point_2d<Type> vgl_closest_point(vgl_line_2d<Type> const& l,
                                     vgl_point_2d<Type> const& p)
{
  Type d = l.a()*l.a()+l.b()*l.b();
  return vgl_point_2d<Type>((l.b()*l.b()*p.x()-l.a()*l.b()*p.y()-l.a()*l.c())/d,
                            (l.a()*l.a()*p.y()-l.a()*l.b()*p.x()-l.b()*l.c())/d);
}

template <class Type>
vgl_homg_point_2d<Type> vgl_closest_point(vgl_homg_line_2d<Type> const& l,
                                          vgl_homg_point_2d<Type> const& p)
{
  return vgl_homg_point_2d<Type>(l.b()*l.b()*p.x()-l.a()*l.b()*p.y()-l.a()*l.c(),
                                 l.a()*l.a()*p.y()-l.a()*l.b()*p.x()-l.b()*l.c(),
                                 l.a()*l.a()+l.b()*l.b());
}

template <class Type>
vgl_point_3d<Type> vgl_closest_point(vgl_plane_3d<Type> const& l,
                                     vgl_point_3d<Type> const& p)
{
  // The planes b(x-x0)=a(y-y0) and c(x-x0)=a(z-z0) are orthogonal
  // to ax+by+cz+d=0 and go through the point (x0,y0,z0).
  // Hence take the intersection of these three planes:
  Type d = l.a()*l.a()+l.b()*l.b()+l.c()*l.c();
  return vgl_point_3d<Type>(((l.b()*l.b()+l.c()*l.c())*p.x()-l.a()*l.b()*p.y()-l.a()*l.c()*p.z()-l.a()*l.d())/d,
                            ((l.a()*l.a()+l.c()*l.c())*p.y()-l.a()*l.b()*p.x()-l.b()*l.c()*p.z()-l.b()*l.d())/d,
                            ((l.a()*l.a()+l.b()*l.b())*p.z()-l.a()*l.c()*p.x()-l.b()*l.c()*p.y()-l.c()*l.d())/d);
}

template <class Type>
vgl_homg_point_3d<Type> vgl_closest_point(vgl_homg_plane_3d<Type> const& l,
                                          vgl_homg_point_3d<Type> const& p)
{
  return vgl_homg_point_3d<Type>((l.b()*l.b()+l.c()*l.c())*p.x()-l.a()*l.b()*p.y()-l.a()*l.c()*p.z()-l.a()*l.d(),
                                 (l.a()*l.a()+l.c()*l.c())*p.y()-l.a()*l.b()*p.x()-l.b()*l.c()*p.z()-l.b()*l.d(),
                                 (l.a()*l.a()+l.b()*l.b())*p.z()-l.a()*l.c()*p.x()-l.b()*l.c()*p.y()-l.c()*l.d(),
                                 l.a()*l.a()+l.b()*l.b()+l.c()*l.c());
}

template <class Type>
vgl_point_2d<Type> vgl_closest_point(vgl_polygon const& poly,
                                     vgl_point_2d<Type> const& point,
                                     bool closed)
{
  double x=point.x(), y=point.y();
  double dd = vgl_distance_to_linesegment(poly[0][0].x(),poly[0][0].y(), poly[0][1].x(),poly[0][1].y(), x,y);
  int si = 0, di = 0;
  for ( int s=0; s < poly.num_sheets(); ++s )
  {
    unsigned int n = poly[s].size();
    assert( n > 1 );
    for (unsigned i=0; i<n-1; ++i)
    {
      double nd = vgl_distance_to_linesegment(poly[s][i].x(),poly[s][i].y(), poly[s][i+1].x(),poly[s][i+1].y(), x,y);
      if (nd<dd) { dd=nd; di=i; si=s; }
    }
    if (closed)
    {
      double nd = vgl_distance_to_linesegment(poly[s][0].x(),poly[s][0].y(), poly[s][n-1].x(),poly[s][n-1].y(), x,y);
      if (nd<dd) { dd=nd; di=-1; si=s; }
    }
  }
  double ret_x, ret_y;
  unsigned int n = poly[si].size();
  if (di == -1)
    vgl_closest_point_to_linesegment(ret_x,ret_y, poly[si][0].x(),poly[si][0].y(), poly[si][n-1].x(),poly[si][n-1].y(), x,y);
  else
    vgl_closest_point_to_linesegment(ret_x,ret_y, poly[si][di].x(),poly[si][di].y(), poly[si][di+1].x(),poly[si][di+1].y(), x,y);
  return vgl_point_2d<Type>(Type(ret_x), Type(ret_y));
}

template <class Type>
vcl_pair<vgl_homg_point_3d<Type>, vgl_homg_point_3d<Type> >
vgl_closest_points(vgl_homg_line_3d_2_points<Type> const& line1,
                   vgl_homg_line_3d_2_points<Type> const& line2)
{
  vcl_pair<vgl_homg_point_3d<Type>, vgl_homg_point_3d<Type> > ret;
  // parallel or equal lines
  if ((line1==line2)||(line1.point_infinite()==line2.point_infinite()))
  {
    ret.first = ret.second = line1.point_infinite();
  }
  // intersecting lines
  else if (concurrent(line1, line2))
  {
    ret.first = ret.second = intersection(line1, line2);
  }
  // neither parallel nor intersecting
  // this is the Paul Bourke code - suitably modified for vxl
  else
  {
    // direction of the two lines and of a crossing line
    vgl_homg_point_3d<Type> p21 = line1.point_infinite();
    vgl_homg_point_3d<Type> p43 = line2.point_infinite();
    vgl_vector_3d<Type> p13 = line1.point_finite()-line2.point_finite();

    Type d1343 = p13.x() * p43.x() + p13.y() * p43.y() + p13.z() * p43.z();
    Type d4321 = p43.x() * p21.x() + p43.y() * p21.y() + p43.z() * p21.z();
    Type d1321 = p13.x() * p21.x() + p13.y() * p21.y() + p13.z() * p21.z();
    Type d4343 = p43.x() * p43.x() + p43.y() * p43.y() + p43.z() * p43.z();
    Type d2121 = p21.x() * p21.x() + p21.y() * p21.y() + p21.z() * p21.z();

    Type denom = d2121 * d4343 - d4321 * d4321;
    Type numer = d1343 * d4321 - d1321 * d4343;

    // avoid divisions:
    //Type mua = numer / denom;
    //Type mub = (d1343 + d4321 * mua) / d4343;
    Type mu_n = d1343 * denom + d4321 * numer;
    Type mu_d = d4343 * denom;

    ret.first.set(denom*line1.point_finite().x()+numer*p21.x(),
                  denom*line1.point_finite().y()+numer*p21.y(),
                  denom*line1.point_finite().z()+numer*p21.z(),
                  denom);
    ret.second.set(mu_d*line2.point_finite().x()+mu_n*p43.x(),
                   mu_d*line2.point_finite().y()+mu_n*p43.y(),
                   mu_d*line2.point_finite().z()+mu_n*p43.z(),
                   mu_d);
  }
  return ret;
}

template vcl_pair<vgl_homg_point_3d<float>, vgl_homg_point_3d<float> >
  vgl_closest_points(vgl_homg_line_3d_2_points<float> const&,
                     vgl_homg_line_3d_2_points<float> const&);
template vcl_pair<vgl_homg_point_3d<double>, vgl_homg_point_3d<double> >
  vgl_closest_points(vgl_homg_line_3d_2_points<double> const&,
                     vgl_homg_line_3d_2_points<double> const&);

template vgl_point_2d<float> vgl_closest_point_origin(vgl_line_2d<float> const& l);
template vgl_point_2d<double> vgl_closest_point_origin(vgl_line_2d<double> const& l);
template vgl_homg_point_2d<float> vgl_closest_point_origin(vgl_homg_line_2d<float> const& l);
template vgl_homg_point_2d<double> vgl_closest_point_origin(vgl_homg_line_2d<double> const& l);

template vgl_point_2d<float> vgl_closest_point(vgl_line_2d<float> const&, vgl_point_2d<float> const&);
template vgl_point_2d<double> vgl_closest_point(vgl_line_2d<double> const&, vgl_point_2d<double> const&);
template vgl_point_2d<float> vgl_closest_point(vgl_point_2d<float> const&, vgl_line_2d<float> const&);
template vgl_point_2d<double> vgl_closest_point(vgl_point_2d<double> const&, vgl_line_2d<double> const&);
template vgl_homg_point_2d<float> vgl_closest_point(vgl_homg_line_2d<float> const&, vgl_homg_point_2d<float> const&);
template vgl_homg_point_2d<double> vgl_closest_point(vgl_homg_line_2d<double> const&, vgl_homg_point_2d<double> const&);
template vgl_homg_point_2d<float> vgl_closest_point(vgl_homg_point_2d<float> const&, vgl_homg_line_2d<float> const&);
template vgl_homg_point_2d<double> vgl_closest_point(vgl_homg_point_2d<double> const&, vgl_homg_line_2d<double> const&);
template vgl_point_3d<float> vgl_closest_point(vgl_plane_3d<float> const& l, vgl_point_3d<float> const& p);
template vgl_point_3d<double> vgl_closest_point(vgl_plane_3d<double> const& l, vgl_point_3d<double> const& p);
template vgl_point_3d<float> vgl_closest_point(vgl_point_3d<float> const& p, vgl_plane_3d<float> const& l);
template vgl_point_3d<double> vgl_closest_point(vgl_point_3d<double> const& p, vgl_plane_3d<double> const& l);
template vgl_homg_point_3d<float> vgl_closest_point(vgl_homg_plane_3d<float> const& l, vgl_homg_point_3d<float> const& p);
template vgl_homg_point_3d<double> vgl_closest_point(vgl_homg_plane_3d<double> const& l, vgl_homg_point_3d<double> const& p);
template vgl_homg_point_3d<float> vgl_closest_point(vgl_homg_point_3d<float> const& p, vgl_homg_plane_3d<float> const& l);
template vgl_homg_point_3d<double> vgl_closest_point(vgl_homg_point_3d<double> const& p, vgl_homg_plane_3d<double> const& l);

template vgl_point_2d<float> vgl_closest_point(vgl_polygon const&, vgl_point_2d<float> const&, bool);
template vgl_point_2d<double> vgl_closest_point(vgl_polygon const&, vgl_point_2d<double> const&, bool);
