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
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/vgl_polygon.h>
#include <vcl_cassert.h>

template <class T>
static inline T square(T x) { return x*x; }


// Consider numbers smaller than this to be zero
const double SMALL_DOUBLE = 1e-12;


// Borland has trouble deducing template types when parameters have
// type const T instead of T. This occurs in
// vgl_distance2_to_linesegment. The workaround is to make the T const
// parameter a T parameter.
//
#ifdef VCL_BORLAND_55
# define DIST_SQR_TO_LINE_SEG_2D( T, x1, y1, x2, y2, x, y ) \
     vgl_distance2_to_linesegment(T(x1), T(y1), T(x2), T(y2), T(x), T(y) );
# define DIST_SQR_TO_LINE_SEG_3D( T, x1, y1, z1, x2, y2, z2, x, y, z ) \
     vgl_distance2_to_linesegment(T(x1), T(y1), T(z1), T(x2), T(y2), T(z2), T(x), T(y), T(z) );
#else
# define DIST_SQR_TO_LINE_SEG_2D( T, x1, y1, x2, y2, x, y ) \
     vgl_distance2_to_linesegment(x1, y1, x2, y2, x, y );
# define DIST_SQR_TO_LINE_SEG_3D( T, x1, y1, z1, x2, y2, z2, x, y, z ) \
     vgl_distance2_to_linesegment(x1, y1, z1, x2, y2, z2, x, y, z );
#endif


template <class T>
void vgl_closest_point_to_linesegment(T& ret_x, T& ret_y,
                                      T x1, T y1,
                                      T x2, T y2,
                                      T x0, T y0)
{
  // squared distance between endpoints:
  T ddh = square(x2-x1) + square(y2-y1);

  // squared distance to endpoints:
  T dd1 = square(x0-x1) + square(y0-y1);
  T dd2 = square(x0-x2) + square(y0-y2);

  // if closest to the start point:
  if (dd2 > ddh + dd1) { ret_x=x1; ret_y=y1; return; }

  // if closest to the end point :
  if (dd1 > ddh + dd2) { ret_x=x2; ret_y=y2; return; }

  // line through (x0,y0) and perpendicular to the given line is
  // the line with equation (x-x0)(x2-x1)+(y-y0)(y2-y1)=0.
  // Then it just remains to intersect these two lines:
  T dx = x2-x1;
  T dy = y2-y1;
  double c = dx*dx+dy*dy;
  ret_x = T((dx*dx*x0+dy*dy*x1-dx*dy*(y1-y0))/c); // possible rounding error!
  ret_y = T((dx*dx*y1+dy*dy*y0-dx*dy*(x1-x0))/c);
}

template <class T>
void vgl_closest_point_to_linesegment(T& ret_x, T& ret_y, T& ret_z,
                                      T x1, T y1, T z1,
                                      T x2, T y2, T z2,
                                      T x, T y, T z)
{
  // squared distance between endpoints:
  T ddh = square(x2-x1) + square(y2-y1) + square(z2-z1);

  // squared distance to endpoints :
  T dd1 = square(x-x1) + square(y-y1) + square(z-z1);
  T dd2 = square(x-x2) + square(y-y2) + square(z-z2);

  // if closest to the start point:
  if (dd2 > ddh + dd1) { ret_x=x1; ret_y=y1; ret_z=z1; return; }

  // if closest to the end point :
  if (dd1 > ddh + dd2) { ret_x=x2; ret_y=y2; ret_z=z2; return; }

  // plane through (x,y,z) and orthogonal to the line is a(X-x)+b(Y-y)+c(Z-z)=0
  // where (a,b,c) is the direction of the line.
  T a = x2-x1, b = y2-y1, c = z2-z1;
  // The closest point is then the intersection of this plane with the line.
  // This point equals (x1,y1,z1) + lambda * (a,b,c), with this lambda:
  double lambda = (a*(x-x1)+b*(y-y1)+c*(z-z1))/double(a*a+b*b+c*c);
  ret_x = x1+T(lambda*a); ret_y = y1+T(lambda*b); ret_z = z1+T(lambda*c);
}

template <class T>
int vgl_closest_point_to_non_closed_polygon(T& ret_x, T& ret_y,
                                            T const px[], T const py[], unsigned int n,
                                            T x, T y)
{
  assert(n>1);
  double dd = DIST_SQR_TO_LINE_SEG_2D(T,px[0],py[0], px[1],py[1], x,y);
  int di = 0;
  for (unsigned i=1; i+1<n; ++i)
  {
    double nd = DIST_SQR_TO_LINE_SEG_2D(T,px[i],py[i], px[i+1],py[i+1], x,y);
    if (nd<dd) { dd=nd; di=i; }
  }
  vgl_closest_point_to_linesegment(ret_x,ret_y, px[di],py[di], px[di+1],py[di+1], x,y);
  return di;
}

template <class T>
int vgl_closest_point_to_non_closed_polygon(T& ret_x, T& ret_y, T& ret_z,
                                            T const px[], T const py[], T const pz[], unsigned int n,
                                            T x, T y, T z)
{
  assert(n>1);
  double dd = DIST_SQR_TO_LINE_SEG_3D(T,px[0],py[0],pz[0], px[1],py[1],pz[1], x,y,z);
  int di = 0;
  for (unsigned i=1; i+1<n; ++i)
  {
    double nd = DIST_SQR_TO_LINE_SEG_3D(T,px[i],py[i],pz[i], px[i+1],py[i+1],pz[i+1], x,y,z);
    if (nd<dd) { dd=nd; di=i; }
  }
  vgl_closest_point_to_linesegment(ret_x,ret_y,ret_z, px[di],py[di],pz[di],
                                   px[di+1],py[di+1],pz[di+1], x,y,z);
  return di;
}

template <class T>
int vgl_closest_point_to_closed_polygon(T& ret_x, T& ret_y,
                                        T const px[], T const py[], unsigned int n,
                                        T x, T y)
{
  assert(n>1);
  double dd = DIST_SQR_TO_LINE_SEG_2D(T,px[0],py[0], px[n-1],py[n-1], x,y);
  int di = -1;
  for (unsigned i=0; i+1<n; ++i)
  {
    double nd = DIST_SQR_TO_LINE_SEG_2D(T,px[i],py[i], px[i+1],py[i+1], x,y);
    if (nd<dd) { dd=nd; di=i; }
  }
  if (di == -1) di+=n,
    vgl_closest_point_to_linesegment(ret_x,ret_y, px[0],py[0], px[n-1],py[n-1], x,y);
  else
    vgl_closest_point_to_linesegment(ret_x,ret_y, px[di],py[di], px[di+1],py[di+1], x,y);
  return di;
}

template <class T>
int vgl_closest_point_to_closed_polygon(T& ret_x, T& ret_y, T& ret_z,
                                        T const px[], T const py[], T const pz[], unsigned int n,
                                        T x, T y, T z)
{
  assert(n>1);
  double dd = DIST_SQR_TO_LINE_SEG_3D(T,px[0],py[0],pz[0], px[n-1],py[n-1],pz[n-1], x,y,z);
  int di = -1;
  for (unsigned i=0; i+1<n; ++i)
  {
    double nd = DIST_SQR_TO_LINE_SEG_3D(T,px[i],py[i],pz[i], px[i+1],py[i+1],pz[i+1], x,y,z);
    if (nd<dd) { dd=nd; di=i; }
  }
  if (di == -1) di+=n,
    vgl_closest_point_to_linesegment(ret_x,ret_y,ret_z, px[0],py[0],pz[0],
                                     px[n-1],py[n-1],pz[n-1], x,y,z);
  else
    vgl_closest_point_to_linesegment(ret_x,ret_y,ret_z, px[di],py[di],pz[di],
                                     px[di+1],py[di+1],pz[di+1], x,y,z);
  return di;
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
vgl_point_3d<T> vgl_closest_point_origin(vgl_plane_3d<T> const& pl)
{
  T d = pl.a()*pl.a()+pl.b()*pl.b()+pl.c()*pl.c();
  return vgl_point_3d<T>(-pl.a()*pl.d()/d, -pl.b()*pl.d()/d, -pl.c()*pl.d()/d);
}

template <class T>
vgl_homg_point_3d<T> vgl_closest_point_origin(vgl_homg_plane_3d<T> const& pl)
{
  return vgl_homg_point_3d<T>(pl.a()*pl.d(), pl.b()*pl.d(), pl.c()*pl.d(),
                              -pl.a()*pl.a()-pl.b()*pl.b()-pl.c()*pl.c());
}

template <class T>
vgl_homg_point_3d<T> vgl_closest_point_origin(vgl_homg_line_3d_2_points<T> const& l)
{
  vgl_homg_point_3d<T> const& q = l.point_finite();
  // The plane through the origin and orthogonal to l is ax+by+cz=0
  // where (a,b,c,0) is the point at infinity of l.
  T a = l.point_infinite().x(), b = l.point_infinite().y(), c = l.point_infinite().z(),
    d = a*a+b*b+c*c;
  // The closest point is then the intersection of this plane with the line l.
  // This point equals d * l.point_finite - lambda * l.direction, with lambda:
  T lambda = a*q.x()+b*q.y()+c*q.z();
  return vgl_homg_point_3d<T>(d*q.x()-lambda*a*q.w(), d*q.y()-lambda*b*q.w(), d*q.z()-lambda*c*q.w(), d*q.w());
}

template <class T>
vgl_point_3d<T> vgl_closest_point_origin(vgl_line_3d_2_points<T> const& l)
{
  vgl_point_3d<T> const& q = l.point1();
  // The plane through the origin and orthogonal to l is ax+by+cz=0
  // where (a,b,c) is the direction of l.
  T a = l.point2().x()-q.x(), b = l.point2().y()-q.y(), c = l.point2().z()-q.z(),
    d = a*a+b*b+c*c;
  // The closest point is then the intersection of this plane with the line l.
  // This point equals l.point1 - lambda * l.direction, with lambda:
  T lambda = (a*q.x()+b*q.y()+c*q.z())/d; // possible rounding error!
  return vgl_point_3d<T>(q.x()-lambda*a, q.y()-lambda*b, q.z()-lambda*c);
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
  double dd = DIST_SQR_TO_LINE_SEG_2D(T,poly[0][0].x(),poly[0][0].y(), poly[0][1].x(),poly[0][1].y(), x,y);
  int si = 0, di = 0;
  for ( unsigned int s=0; s < poly.num_sheets(); ++s )
  {
    unsigned int n = poly[s].size();
    assert( n > 1 );
    for (unsigned i=0; i+1<n; ++i)
    {
      double nd = DIST_SQR_TO_LINE_SEG_2D(T,poly[s][i].x(),poly[s][i].y(), poly[s][i+1].x(),poly[s][i+1].y(), x,y);
      if (nd<dd) { dd=nd; di=i; si=s; }
    }
    if (closed)
    {
      double nd = DIST_SQR_TO_LINE_SEG_2D(T,poly[s][0].x(),poly[s][0].y(), poly[s][n-1].x(),poly[s][n-1].y(), x,y);
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
  vgl_homg_point_3d<T> const& q = l.point_finite();
  vgl_vector_3d<T> v = p-q;
  // The plane through p and orthogonal to l is a(x-px)+b(y-py)+c(z-pz)=0
  // where (a,b,c,0) is the point at infinity of l.
  T a = l.point_infinite().x(), b = l.point_infinite().y(), c = l.point_infinite().z(),
    d = a*a+b*b+c*c;
  // The closest point is then the intersection of this plane with the line l.
  // This point equals d * l.point_finite + lambda * l.direction, with lambda:
  T lambda = a*v.x()+b*v.y()+c*v.z();
  return vgl_homg_point_3d<T>(d*q.x()+lambda*a*q.w(), d*q.y()+lambda*b*q.w(), d*q.z()+lambda*c*q.w(), d*q.w());
}

template <class T>
vgl_point_3d<T> vgl_closest_point(vgl_line_3d_2_points<T> const& l,
                                  vgl_point_3d<T> const& p)
{
  vgl_point_3d<T> const& q = l.point1();
  vgl_vector_3d<T> v = p-q;
  // The plane through p and orthogonal to l is a(x-px)+b(y-py)+c(z-pz)=0
  // where (a,b,c,0) is the direction of l.
  T a = l.point2().x()-q.x(), b = l.point2().y()-q.y(), c = l.point2().z()-q.z(),
    d = a*a+b*b+c*c;
  // The closest point is then the intersection of this plane with the line l.
  // This point equals l.point1 + lambda * l.direction, with lambda:
  T lambda = (a*v.x()+b*v.y()+c*v.z())/d; // possible rounding error!
  return vgl_point_3d<T>(q.x()+lambda*a, q.y()+lambda*b, q.z()+lambda*c);
}


//: Return the points of closest approach on 2 3D lines.
template <class T>
vcl_pair<vgl_point_3d<T>, vgl_point_3d<T> > 
vgl_closest_points(const vgl_line_3d_2_points<T>& l1, 
                   const vgl_line_3d_2_points<T>& l2,
                   bool* unique/*=0*/)
{
  vcl_pair<vgl_point_3d<T>, vgl_point_3d<T> > ret;
  
  // Get the parametric equation of each line
  // l1: p(s) = p1 + su;  u = p2 - p1
  // l2: q(t) = q1 + tv;  v = q2 - q1
  vgl_vector_3d<T> u = l1.direction();
  vgl_vector_3d<T> v = l2.direction();

  // Get a vector w from first point on line1 to first point on line2
  vgl_vector_3d<T> w = l1.point1() - l2.point1();

  double a = dot_product(u,u);
  double b = dot_product(u,v);
  double c = dot_product(v,v);
  double d = dot_product(u,w);
  double e = dot_product(v,w);

  // Calculate the parameters s,t for the closest point on each line
  double denom = a*c - b*b; // should always be non-negative
  assert(denom>=0.0);
  if (denom>SMALL_DOUBLE)
  {
    double s = (b*e - c*d) / denom;
    double t = (a*e - b*d) / denom;

    ret.first = l1.point_t(s);
    ret.second = l2.point_t(t);
    if (unique) *unique = true;
  }
  else
  {
    // Lines are parallel or collinear.
    // Arbitrarily, return the first point on line1 and the closest point on line2.
    double s = 0.0;
    double t = (b>c ? d/b : e/c);
    ret.first = l1.point_t(s);
    ret.second = l2.point_t(t);
    if (unique) *unique = false;
  }  

  return ret;
}


//: Return the points of closest approach on 2 3D line segments.
template <class T>
vcl_pair<vgl_point_3d<T>, vgl_point_3d<T> > 
vgl_closest_points(vgl_line_segment_3d<T> const& l1, 
                   vgl_line_segment_3d<T> const& l2,
                   bool* unique/*=0*/)
{
  vcl_pair<vgl_point_3d<T>, vgl_point_3d<T> > ret;
  
  // Get the parametric equation of each line
  // l1: p(s) = p1 + su;  u = p2 - p1
  // l2: q(t) = q1 + tv;  v = q2 - q1
  vgl_vector_3d<T> u = l1.direction();
  vgl_vector_3d<T> v = l2.direction();
  
  // Get a vector w from first point on line2 to first point on line1
  vgl_vector_3d<T> w = l1.point1() - l2.point1();
  
  double a = dot_product(u,u); assert(a>0.0);
  double b = dot_product(u,v); 
  double c = dot_product(v,v); assert(c>0.0);
  double d = dot_product(u,w); 
  double e = dot_product(v,w); 
  
  // Calculate the parameters s,t for the closest point on each infinite line
  double denom = a*c - b*b; // should always be non-negative
  assert(denom>=0.0);
    
  // Check whether the closest points on the infinite lines are also
  // on the finite line segments.
  // Consider the square [0,1][0,1] in the plane (s,t). 
  // Closest points (s,t) on the infinite lines may lie outside this square.
  // Closest points on line segments will then lie on the boundary of this square.
  // Hence, need to check either 1 or 2 edges of the square.
  double s_numer = 0.0; 
  double s_denom = denom;
  double t_numer = 0.0;
  double t_denom = denom;

  if (denom < SMALL_DOUBLE) 
  {   
    // Lines are parallel or collinear
    s_numer = 0.0; 
    s_denom = 1.0;
    t_numer = e;
    t_denom = c;
    if (unique) *unique = false; // assume this for now; check below.
  }
  else 
  {      
    // Calculate the closest points on the infinite lines
    s_numer = (b*e - c*d);
    t_numer = (a*e - b*d);
    if (s_numer < 0.0) 
    {       
      // If sc<0 then the s=0 edge is a candidate
      s_numer = 0.0;
      t_numer = e;
      t_denom = c;
    }
    else if (s_numer > s_denom) 
    {  
      // If sc>1 then the s=1 edge is a candidate
      s_numer = s_denom;
      t_numer = e + b;
      t_denom = c;
    }
    if (unique) *unique = true;
  }
  
  if (t_numer < 0.0) 
  {           
    // If tc<0 then the t=0 edge is a candidate
    t_numer = 0.0;
    
    // Recalculate sc for this edge
    if (-d < 0.0)
      s_numer = 0.0;
    else if (-d > a)
      s_numer = s_denom;
    else
    {
      s_numer = -d;
      s_denom = a;
    }
  }
  else if (t_numer > t_denom) 
  {      
    // If tc>1 then the t=1 edge is a candidate
    t_numer = t_denom;

    // Recalculate sc for this edge
    if ((-d + b) < 0.0)
      s_numer = 0.0;
    else if ((-d + b) > a)
      s_numer = s_denom;
    else 
    {
      s_numer = (-d + b);
      s_denom = a;
    }
  }
  
  // Now calculate the required values of (s,t)
  double s = (abs(s_numer) < SMALL_DOUBLE ? 0.0 : s_numer / s_denom);
  double t = (abs(t_numer) < SMALL_DOUBLE ? 0.0 : t_numer / t_denom);

  // Need to verify whether returned closest points are unique
  // in the case of parallel/collinear line segments
  if (unique && *unique==false)
  {
    if ((s==0.0 || s==1.0) && (t==0.0 || t==1.0))
      *unique = true;
  }

  ret.first = l1.point_t(s);
  ret.second = l2.point_t(t);

  return ret;
}


#undef DIST_SQR_TO_LINE_SEG_2D
#undef DIST_SQR_TO_LINE_SEG_3D


#undef VGL_CLOSEST_POINT_INSTANTIATE
#define VGL_CLOSEST_POINT_INSTANTIATE(T) \
template void vgl_closest_point_to_linesegment(T&,T&,T,T,T,T,T,T); \
template void vgl_closest_point_to_linesegment(T&,T&,T&,T,T,T,T,T,T,T,T,T); \
template int vgl_closest_point_to_non_closed_polygon(T&,T&,T const[],T const[],unsigned int,T,T); \
template int vgl_closest_point_to_non_closed_polygon(T&,T&,T&,T const[],T const[],T const[],unsigned int,T,T,T); \
template int vgl_closest_point_to_closed_polygon(T&,T&,T const[],T const[],unsigned int,T,T); \
template int vgl_closest_point_to_closed_polygon(T&,T&,T&,T const[],T const[],T const[],unsigned int,T,T,T); \
template vgl_point_2d<T > vgl_closest_point_origin(vgl_line_2d<T >const& l); \
template vgl_point_3d<T > vgl_closest_point_origin(vgl_plane_3d<T > const& pl); \
template vgl_point_3d<T > vgl_closest_point_origin(vgl_line_3d_2_points<T > const& l); \
template vgl_homg_point_2d<T > vgl_closest_point_origin(vgl_homg_line_2d<T >const& l); \
template vgl_homg_point_3d<T > vgl_closest_point_origin(vgl_homg_plane_3d<T > const& pl); \
template vgl_homg_point_3d<T > vgl_closest_point_origin(vgl_homg_line_3d_2_points<T > const& l); \
template vgl_point_2d<T > vgl_closest_point(vgl_line_2d<T >const&,vgl_point_2d<T >const&); \
template vgl_point_2d<T > vgl_closest_point(vgl_point_2d<T >const&,vgl_line_2d<T >const&); \
template vgl_point_3d<T > vgl_closest_point(vgl_line_3d_2_points<T >const&,vgl_point_3d<T >const&); \
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
template vgl_homg_point_3d<T > vgl_closest_point(vgl_homg_point_3d<T >const&,vgl_homg_line_3d_2_points<T >const&); \
template vcl_pair<vgl_point_3d<T >,vgl_point_3d<T > > \
         vgl_closest_points(vgl_line_3d_2_points<T >const&, vgl_line_3d_2_points<T >const&, bool*); \
template vcl_pair<vgl_point_3d<T >,vgl_point_3d<T > > \
         vgl_closest_points(vgl_line_segment_3d<T >const&, vgl_line_segment_3d<T >const&, bool*); \

#endif // vgl_closest_point_txx_
