// This is core/vgl/vgl_closest_point.txx
#ifndef vgl_closest_point_txx_
#define vgl_closest_point_txx_
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

template <class T>
void vgl_closest_point_to_linesegment(T& ret_x, T& ret_y,
                                      T x1, T y1,
                                      T x2, T y2,
                                      T x0, T y0)
{
  // squared distance between endpoints:
  T ddh = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);

  // squared distance to endpoints:
  T dd0 = (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1);
  T dd1 = (x0-x2)*(x0-x2) + (y0-y2)*(y0-y2);

  // if closest to the start point:
  if (dd1 > ddh + dd0) { ret_x=x1; ret_y=y1; return; }

  // if closest to the end point :
  if (dd0 > ddh + dd1) { ret_x=x2; ret_y=y2; return; }

  // line through (x0,y0) and perpendicular to the given line is
  // the line with equation (x-x0)(x2-x1)+(y-y0)(y2-y1)=0.
  // Then it just remains to intersect these two lines:
  T dx = x2-x1;
  T dy = y2-y1;
  T c = dx*dx+dy+dy;
  ret_x = (dx*dx*x0+dy*dy*x1-dx*dy*(y0-y1))/c;
  ret_y = (dx*dx*y1+dy*dy*y0-dx*dy*(x1-x0))/c;
  return;
}

template <class T>
void vgl_closest_point_to_non_closed_polygon(T& ret_x, T& ret_y,
                                             T const px[], T const py[], unsigned int n,
                                             T x, T y)
{
  assert(n>1);
  T dd = vgl_distance_to_linesegment(px[0],py[0], px[1],py[1], x,y);
  int di = 0;
  for (unsigned i=1; i<n-1; ++i)
  {
    T nd = vgl_distance_to_linesegment(px[i],py[i], px[i+1],py[i+1], x,y);
    if (nd<dd) { dd=nd; di=i; }
  }
  vgl_closest_point_to_linesegment(ret_x,ret_y, px[di],py[di], px[di+1],py[di+1], x,y);
}

template <class T>
void vgl_closest_point_to_closed_polygon(T& ret_x, T& ret_y,
                                         T const px[], T const py[], unsigned int n,
                                         T x, T y)
{
  assert(n>1);
  T dd = vgl_distance_to_linesegment(px[0],py[0], px[n-1],py[n-1], x,y);
  int di = -1;
  for (unsigned i=0; i<n-1; ++i)
  {
    T nd = vgl_distance_to_linesegment(px[i],py[i], px[i+1],py[i+1], x,y);
    if (nd<dd) { dd=nd; di=i; }
  }
  if (di == -1)
    vgl_closest_point_to_linesegment(ret_x,ret_y, px[0],py[0], px[n-1],py[n-1], x,y);
  else
    vgl_closest_point_to_linesegment(ret_x,ret_y, px[di],py[di], px[di+1],py[di+1], x,y);
}

template <class T>
vgl_point_2d<T> vgl_closest_point_origin(vgl_line_2d<T> const& l)
{
  T d = l.a()*l.a()+l.b()*l.b();
  return vgl_point_2d<T>(-l.a()*l.c()/d, -l.b()*l.c()/d);
}

template <class T>
vgl_homg_point_2d<T> vgl_closest_point_origin(vgl_homg_line_2d<T> const& l)
{
  return vgl_homg_point_2d<T>(l.a()*l.c(), l.b()*l.c(),
                              -l.a()*l.a()-l.b()*l.b());
}

template <class T>
vgl_point_2d<T> vgl_closest_point(vgl_line_2d<T> const& l,
                                  vgl_point_2d<T> const& p)
{
  T d = l.a()*l.a()+l.b()*l.b();
  assert(d!=0); // line should not be the line at infinity
  return vgl_point_2d<T>((l.b()*l.b()*p.x()-l.a()*l.b()*p.y()-l.a()*l.c())/d,
                         (l.a()*l.a()*p.y()-l.a()*l.b()*p.x()-l.b()*l.c())/d);
}

template <class T>
vgl_homg_point_2d<T> vgl_closest_point(vgl_homg_line_2d<T> const& l,
                                       vgl_homg_point_2d<T> const& p)
{
  T d = l.a()*l.a()+l.b()*l.b();
  assert(d!=0); // line should not be the line at infinity
  return vgl_homg_point_2d<T>(l.b()*l.b()*p.x()-l.a()*l.b()*p.y()-l.a()*l.c(),
                              l.a()*l.a()*p.y()-l.a()*l.b()*p.x()-l.b()*l.c(),
                              d);
}

template <class T>
vgl_point_3d<T> vgl_closest_point(vgl_plane_3d<T> const& l,
                                  vgl_point_3d<T> const& p)
{
  // The planes b(x-x0)=a(y-y0) and c(x-x0)=a(z-z0) are orthogonal
  // to ax+by+cz+d=0 and go through the point (x0,y0,z0).
  // Hence take the intersection of these three planes:
  T d = l.a()*l.a()+l.b()*l.b()+l.c()*l.c();
  return vgl_point_3d<T>(((l.b()*l.b()+l.c()*l.c())*p.x()-l.a()*l.b()*p.y()-l.a()*l.c()*p.z()-l.a()*l.d())/d,
                         ((l.a()*l.a()+l.c()*l.c())*p.y()-l.a()*l.b()*p.x()-l.b()*l.c()*p.z()-l.b()*l.d())/d,
                            ((l.a()*l.a()+l.b()*l.b())*p.z()-l.a()*l.c()*p.x()-l.b()*l.c()*p.y()-l.c()*l.d())/d);
}

template <class T>
vgl_homg_point_3d<T> vgl_closest_point(vgl_homg_plane_3d<T> const& l,
                                       vgl_homg_point_3d<T> const& p)
{
  return vgl_homg_point_3d<T>((l.b()*l.b()+l.c()*l.c())*p.x()-l.a()*l.b()*p.y()-l.a()*l.c()*p.z()-l.a()*l.d(),
                              (l.a()*l.a()+l.c()*l.c())*p.y()-l.a()*l.b()*p.x()-l.b()*l.c()*p.z()-l.b()*l.d(),
                              (l.a()*l.a()+l.b()*l.b())*p.z()-l.a()*l.c()*p.x()-l.b()*l.c()*p.y()-l.c()*l.d(),
                              l.a()*l.a()+l.b()*l.b()+l.c()*l.c());
}

template <class T>
vgl_point_2d<T> vgl_closest_point(vgl_polygon<T> const& poly,
                                  vgl_point_2d<T> const& point,
                                  bool closed)
{
  T x=point.x(), y=point.y();
  T dd = vgl_distance_to_linesegment(poly[0][0].x(),poly[0][0].y(), poly[0][1].x(),poly[0][1].y(), x,y);
  int si = 0, di = 0;
  for ( int s=0; s < poly.num_sheets(); ++s )
  {
    unsigned int n = poly[s].size();
    assert( n > 1 );
    for (unsigned i=0; i<n-1; ++i)
    {
      T nd = vgl_distance_to_linesegment(poly[s][i].x(),poly[s][i].y(), poly[s][i+1].x(),poly[s][i+1].y(), x,y);
      if (nd<dd) { dd=nd; di=i; si=s; }
    }
    if (closed)
    {
      T nd = vgl_distance_to_linesegment(poly[s][0].x(),poly[s][0].y(), poly[s][n-1].x(),poly[s][n-1].y(), x,y);
      if (nd<dd) { dd=nd; di=-1; si=s; }
    }
  }
  T ret_x, ret_y;
  unsigned int n = poly[si].size();
  if (di == -1)
    vgl_closest_point_to_linesegment(ret_x,ret_y, poly[si][0].x(),poly[si][0].y(), poly[si][n-1].x(),poly[si][n-1].y(), x,y);
  else
    vgl_closest_point_to_linesegment(ret_x,ret_y, poly[si][di].x(),poly[si][di].y(), poly[si][di+1].x(),poly[si][di+1].y(), x,y);
  return vgl_point_2d<T>(T(ret_x), T(ret_y));
}

template <class T>
vcl_pair<vgl_homg_point_3d<T>, vgl_homg_point_3d<T> >
vgl_closest_points(vgl_homg_line_3d_2_points<T> const& line1,
                   vgl_homg_line_3d_2_points<T> const& line2)
{
  vcl_pair<vgl_homg_point_3d<T>, vgl_homg_point_3d<T> > ret;
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
    vgl_homg_point_3d<T> p21 = line1.point_infinite();
    vgl_homg_point_3d<T> p43 = line2.point_infinite();
    vgl_vector_3d<T> p13 = line1.point_finite()-line2.point_finite();

    T d1343 = p13.x() * p43.x() + p13.y() * p43.y() + p13.z() * p43.z();
    T d4321 = p43.x() * p21.x() + p43.y() * p21.y() + p43.z() * p21.z();
    T d1321 = p13.x() * p21.x() + p13.y() * p21.y() + p13.z() * p21.z();
    T d4343 = p43.x() * p43.x() + p43.y() * p43.y() + p43.z() * p43.z();
    T d2121 = p21.x() * p21.x() + p21.y() * p21.y() + p21.z() * p21.z();

    T denom = d2121 * d4343 - d4321 * d4321;
    T numer = d1343 * d4321 - d1321 * d4343;

    // avoid divisions:
    //T mua = numer / denom;
    //T mub = (d1343 + d4321 * mua) / d4343;
    T mu_n = d1343 * denom + d4321 * numer;
    T mu_d = d4343 * denom;

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

template <class T>
vgl_homg_point_3d<T> vgl_closest_point(vgl_homg_line_3d_2_points<T> const& l,
                                       vgl_homg_point_3d<T> const& p)
{
  // Invalid case: the given point is at infinity:
  if (p.w() == 0) return l.point_infinite();
  // The plane through p and orthogonal to l is a(x-px)+b(y-py)+c(z-pz)=0
  // where (a,b,c,0) is the point at infinity of l.
  T a = l.point_infinite().x(), b = l.point_infinite().y(), c = l.point_infinite().z();
  // The closest point is then the intersection of this plane with the line l.
  // This point equals l.point_finite + lambda * l.direction, with lambda:
  vgl_homg_point_3d<T> const& q = l.point_finite();
  vgl_vector_3d<T> v = p-q;
  T lambda = (a*v.x()+b*v.y()+c*v.z())/(a*a+b*b+c*c);
  return vgl_homg_point_3d<T>(q.x()+lambda*a*q.w(), q.y()+lambda*b*q.w(), q.z()+lambda*c*q.w(), q.w());
}

#undef VGL_CLOSEST_POINT_INSTANTIATE
#define VGL_CLOSEST_POINT_INSTANTIATE(T) \
template void vgl_closest_point_to_linesegment(T&,T&,T,T,T,T,T,T); \
template void vgl_closest_point_to_non_closed_polygon(T&,T&,T const[],T const[],unsigned int,T,T); \
template void vgl_closest_point_to_closed_polygon(T&,T&,T const[],T const[],unsigned int,T,T); \
template vgl_point_2d<T > vgl_closest_point_origin(vgl_line_2d<T >const&); \
template vgl_homg_point_2d<T > vgl_closest_point_origin(vgl_homg_line_2d<T >const& l); \
template vgl_point_2d<T > vgl_closest_point(vgl_line_2d<T >const&,vgl_point_2d<T >const&); \
template vgl_point_2d<T > vgl_closest_point(vgl_point_2d<T >const&,vgl_line_2d<T >const&); \
template vgl_homg_point_2d<T > vgl_closest_point(vgl_homg_line_2d<T >const&,vgl_homg_point_2d<T >const&); \
template vgl_homg_point_2d<T > vgl_closest_point(vgl_homg_point_2d<T >const&,vgl_homg_line_2d<T >const&); \
template vgl_point_3d<T > vgl_closest_point(vgl_plane_3d<T >const&,vgl_point_3d<T >const&); \
template vgl_point_3d<T > vgl_closest_point(vgl_point_3d<T >const&,vgl_plane_3d<T >const&); \
template vgl_homg_point_3d<T > vgl_closest_point(vgl_homg_plane_3d<T >const&,vgl_homg_point_3d<T >const&); \
template vgl_homg_point_3d<T > vgl_closest_point(vgl_homg_point_3d<T >const&,vgl_homg_plane_3d<T >const&); \
template vgl_point_2d<T > vgl_closest_point(vgl_polygon<T >const&,vgl_point_2d<T >const&,bool); \
template vcl_pair<vgl_homg_point_3d<T >,vgl_homg_point_3d<T > > \
         vgl_closest_points(vgl_homg_line_3d_2_points<T >const&,vgl_homg_line_3d_2_points<T >const&); \
template vgl_homg_point_3d<T > vgl_closest_point(vgl_homg_line_3d_2_points<T >const&,vgl_homg_point_3d<T >const&); \
template vgl_homg_point_3d<T > vgl_closest_point(vgl_homg_point_3d<T >const&,vgl_homg_line_3d_2_points<T >const&)

#endif // vgl_closest_point_txx_
