// This is core/vgl/algo/vgl_homg_operators_3d.txx
#ifndef vgl_homg_operators_3d_txx_
#define vgl_homg_operators_3d_txx_
//:
// \file

#include "vgl_homg_operators_3d.h"

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cmath.h> // for vcl_sqrt(), vcl_acos()
#include <vcl_cassert.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_svd.h>

#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_homg_point_2d.h>

//-----------------------------------------------------------------------------

//: Return the angle between the (oriented) lines (in radians)
//
template <class Type>
double vgl_homg_operators_3d<Type>::angle_between_oriented_lines(const vgl_homg_line_3d& l1,
                                                                 const vgl_homg_line_3d& l2)
{
  vgl_homg_point_3d<Type> const& dir1 = l1.point_infinite();
  vgl_homg_point_3d<Type> const& dir2 = l2.point_infinite();
  double n = dir1.x()*dir1.x()+dir1.y()*dir1.y()+dir1.z()*dir1.z();
  n       *= dir2.x()*dir2.x()+dir2.y()*dir2.y()+dir2.z()*dir2.z();
  // dot product of unit direction vectors:
  n = (dir1.x()*dir2.x()+dir1.y()*dir2.y()+dir1.z()*dir2.z())/vcl_sqrt(n);
  return vcl_acos(n);
}


//-----------------------------------------------------------------------------

//: Return the squared distance between the points
//
template <class Type>
Type vgl_homg_operators_3d<Type>::distance_squared(const vgl_homg_point_3d<Type>& point1,
                                                   const vgl_homg_point_3d<Type>& point2)
{
  Type mag = 0;
  Type d;

  d = point1.x()/point1.w() - point2.x()/point2.w();
  mag += d*d;

  d = point1.y()/point1.w() - point2.y()/point2.w();
  mag += d*d;

  d = point1.z()/point1.w() - point2.z()/point2.w();
  mag += d*d;

  return mag;
}

//-----------------------------------------------------------------------------

//: Return the Eucidean distance between the points
//
template <class Type>
Type vgl_homg_operators_3d<Type>::distance(const vgl_homg_point_3d<Type>&point1,
                                           const vgl_homg_point_3d<Type>&point2)
{
  return vcl_sqrt(vgl_homg_operators_3d<Type>::distance_squared(point1,point2));
}

//-----------------------------------------------------------------------------

//: Return the intersection point of the line and plane
//
template <class Type>
vgl_homg_point_3d<Type> vgl_homg_operators_3d<Type>::intersect_line_and_plane(
                                  const vgl_homg_line_3d &line,
                                  const vgl_homg_plane_3d<Type>& plane)
{
  //
  // use P.(S + lambda D) = 0 to find lambda, and hence a point on the plane.

  // TODO should have methods for DoubleVector from a point

  const vnl_vector<Type> x1 = get_vector(line.point_finite());
  const vnl_vector<Type> x2 = get_vector(line.point_infinite());
  const vnl_vector<Type>  p = get_vector(plane);

  // FIXME: this works for double and smaller, but not complex. it might happen.

  double numerator = -dot_product(x1, p);  // find out if dot_product is ok
  double denominator = dot_product(x2, p);

  // Scale for conditioning
  double scale;
  if ( numerator + denominator != 0 )
    scale = 1.0/(numerator + denominator);
  else
    scale = 1.0/numerator;
  numerator *= scale;
  denominator *= scale;

  vnl_vector<Type> r = x1 * Type(denominator) + x2 * Type(numerator);
  return vgl_homg_point_3d<Type>(r[0], r[1], r[2], r[3]);
}

//-----------------------------------------------------------------------------
//
// Compute the intersection point of the lines, or the mid-point
// of the common perpendicular if the lines are skew
//
#if 0 // linker error better than run-time error.
template <class Type>
vgl_homg_point_3d<Type>
vgl_homg_operators_3d<Type>::lines_to_point(const vgl_homg_line_3d& ,
                                            const vgl_homg_line_3d& )
{
  vcl_cerr << "Warning: vgl_homg_operators_3d<Type>::lines_to_point() not yet implemented\n";
  return vgl_homg_point_3d<Type>();
}
#endif

//-----------------------------------------------------------------------------
//
// - Compute the best fit intersection point of the lines
//
#if 0 // linker error better than run-time error.
template <class Type>
vgl_homg_point_3d<Type>
vgl_homg_operators_3d<Type>::lines_to_point(const vcl_vector<vgl_homg_line_3d >& )
{
  vcl_cerr << "Warning: vgl_homg_operators_3d<Type>::lines_to_point() not yet implemented\n";
  return vgl_homg_point_3d<Type>();
}
#endif

//-----------------------------------------------------------------------------

//: Return the squared perpendicular distance between the line and point
//
template <class Type>
double
vgl_homg_operators_3d<Type>::perp_dist_squared(const vgl_homg_line_3d& l,
                                               const vgl_homg_point_3d<Type>& p)
{
  vgl_homg_point_3d<Type> q = vgl_homg_operators_3d<Type>::perp_projection(l, p); // foot point
  return vgl_homg_operators_3d<Type>::distance_squared(p,q);
}

//-----------------------------------------------------------------------------

//: Return the line which is perpendicular to l and passes through p.
//
template <class Type>
typename vgl_homg_operators_3d<Type>::vgl_homg_line_3d
vgl_homg_operators_3d<Type>::perp_line_through_point(const vgl_homg_line_3d& l,
                                                     const vgl_homg_point_3d<Type>& p)
{
  if (p.ideal())
  {
    // this only works if p is not on l; and since the implementation below
    // only works when p is a finite point, use this one when p is at infinity.
    vgl_homg_point_3d<Type> perp_dirn = vgl_homg_operators_3d<Type>::perp_projection(l,p);
    if (get_vector(p)==get_vector(perp_dirn))
      vcl_cerr << "Warning: perp_line_through_point() makes no sense if the point is the infinity point of the line\n";
    return vgl_homg_line_3d(p, perp_dirn);
  }
  else // by Brendan McCane
  {
    // OK this is a better version because it works even if the point
    // is on the line. It does this simply by creating a direction
    // perpendicular to the current line and then creating a new
    // line with the passed in pt and the perpendicular direction.
    vgl_homg_point_3d<Type> dirn = l.point_infinite();
    vgl_homg_point_3d<Type> perp_dirn(Type(1)/dirn.x(), (-dirn.z()-1)/dirn.y(), Type(1), Type(0));
    // should put an assert in here making sure that the dot product
    // is zero (or close to), but I don't know how to do that with
    // templates eg complex<1e-8 probably doesn't make sense.
    return vgl_homg_line_3d(p, perp_dirn);
  }
}


//-----------------------------------------------------------------------------

//: Compute the perpendicular projection point of p onto l.
//
template <class Type>
vgl_homg_point_3d<Type>
vgl_homg_operators_3d<Type>::perp_projection(const vgl_homg_line_3d& l,
                                             const vgl_homg_point_3d<Type>& p)
{
  vgl_homg_point_3d<Type> const& q = l.point_finite();
  Type a[3]  = { q.x()/q.w(), q.y()/q.w(), q.z()/q.w() };
  Type b[3]  = { p.x()/p.w()-a[0], p.y()/p.w()-a[1], p.z()/p.w()-a[2] };

  vgl_homg_point_3d<Type> const& i = l.point_infinite();
  Type dp = i.x()*i.x()+i.y()*i.y()+i.z()*i.z();
  dp = (b[0]*i.x() + b[1]*i.y() + b[2]*i.z()) / dp;

  return vgl_homg_point_3d<Type>(a[0]+dp*i.x(), a[1]+dp*i.y(), a[2]+dp*i.z());
}


//-----------------------------------------------------------------------------

//: Dihedral angle (of intersection) of 2 planes
//
template <class Type>
Type vgl_homg_operators_3d<Type>::plane_plane_angle(const vgl_homg_plane_3d<Type>& plane1,
                                                    const vgl_homg_plane_3d<Type>& plane2)
{
  double cosang = dot_product(plane1.normal(), plane2.normal());

  return vcl_acos(cosang);
}

//-----------------------------------------------------------------------------

//: Return the intersection line of the planes
//
template <class Type>
typename vgl_homg_operators_3d<Type>::vgl_homg_line_3d
vgl_homg_operators_3d<Type>::planes_to_line(const vgl_homg_plane_3d<Type>& plane1,
                                            const vgl_homg_plane_3d<Type>& plane2)
{
  // TODO need equivalent of get_vector
  vnl_matrix<Type> M(2,4);
  M.set_row(0, get_vector(plane1));
  M.set_row(1, get_vector(plane2));
  vnl_svd<Type> svd(M);
  M = svd.nullspace(2);
  vnl_vector<Type> r = M.get_column(0);
  vgl_homg_point_3d<Type> p1(r[0], r[1], r[2], r[3]);
  r = M.get_column(1);
  vgl_homg_point_3d<Type> p2(r[0], r[1], r[2], r[3]);
  return vgl_homg_line_3d(p1, p2);
}


//-----------------------------------------------------------------------------
//
// - Compute the best-fit intersection line of the planes
//
#if 0 // linker error better than run-time error.
template <class Type>
vgl_homg_operators_3d<Type>::vgl_homg_line_3d
vgl_homg_operators_3d<Type>::planes_to_line(const vcl_vector<vgl_homg_plane_3d<Type> >&)
{
  vcl_cerr << "Warning: vgl_homg_operators_3d<Type>::planes_to_line() not yet implemented\n";
  return vgl_homg_line_3d<Type>();
}
#endif


//-----------------------------------------------------------------------------
//
// - Return the line through the points
//
#if 0 // linker error better than run-time error.
template <class Type>
vgl_homg_operators_3d<Type>::vgl_homg_line_3d
vgl_homg_operators_3d<Type>::points_to_line(const vgl_homg_point_3d<Type>&,
                                            const vgl_homg_point_3d<Type>&)
{
  vcl_cerr << "Warning: vgl_homg_operators_3d<Type>::points_to_line() not yet implemented\n";
  return vgl_homg_line_3d<Type>();
}
#endif

//-----------------------------------------------------------------------------
//
// - Compute the best-fit line through the points
//
#if 0 // linker error better than run-time error.
template <class Type>
vgl_homg_operators_3d<Type>::vgl_homg_line_3d
vgl_homg_operators_3d<Type>::points_to_line(const vcl_vector<vgl_homg_point_3d<Type> >&)
{
  vcl_cerr << "Warning: vgl_homg_operators_3d<Type>::points_to_line() not yet implemented\n";
  return vgl_homg_line_3d<Type>();
}
#endif

//-----------------------------------------------------------------------------
//
// - Return the plane through the points
//
#if 0 // linker error better than run-time error.
template <class Type>
vgl_homg_plane_3d<Type>
vgl_homg_operators_3d<Type>::points_to_plane(const vgl_homg_point_3d<Type>&,
                                             const vgl_homg_point_3d<Type>&,
                                             const vgl_homg_point_3d<Type>&)
{
  vcl_cerr << "Warning: vgl_homg_operators_3d<Type>::points_to_plane() not yet implemented\n";
  return vgl_homg_plane_3d<Type>();
}
#endif


//-----------------------------------------------------------------------------
//
// - Compute the best-fit plane through the points
//
#if 0 // linker error better than run-time error.
template <class Type>
vgl_homg_plane_3d<Type>
vgl_homg_operators_3d<Type>::points_to_plane(const vcl_vector<vgl_homg_point_3d<Type> >&)
{
  vcl_cerr << "Warning: vgl_homg_operators_3d<Type>::points_to_plane() not yet implemented\n";
  return vgl_homg_plane_3d<Type>();
}
#endif

//-----------------------------------------------------------------------------

//: Compute best-fit intersection of planes in a point.
//
template <class Type>
vgl_homg_point_3d<Type>
vgl_homg_operators_3d<Type>::intersection(const vgl_homg_plane_3d<Type>& p1,
                                          const vgl_homg_plane_3d<Type>& p2,
                                          const vgl_homg_plane_3d<Type>& p3)
{
  return vgl_homg_point_3d<Type>(
       p1.ny()*p2.nz()*p3.d()+p2.ny()*p3.nz()*p1.d()+p3.ny()*p1.nz()*p2.d()
      -p1.ny()*p3.nz()*p2.d()-p2.ny()*p1.nz()*p3.d()-p3.ny()*p2.nz()*p1.d(),
       p1.nz()*p2.d()*p3.nx()+p2.nz()*p3.d()*p1.nx()+p3.nz()*p1.d()*p2.nx()
      -p1.nz()*p3.d()*p2.nx()-p2.nz()*p1.d()*p3.nx()-p3.nz()*p2.d()*p1.nx(),
       p1.d()*p2.nx()*p3.ny()+p2.d()*p3.nx()*p1.ny()+p3.d()*p1.nx()*p2.ny()
      -p1.d()*p3.nx()*p2.ny()-p2.d()*p1.nx()*p3.ny()-p3.d()*p2.nx()*p1.ny(),
       p1.nx()*p2.ny()*p3.nz()+p2.nx()*p3.ny()*p1.nz()+p3.nx()*p1.ny()*p2.nz()
      -p1.nx()*p3.ny()*p2.nz()-p2.nx()*p1.ny()*p3.nz()-p3.nx()*p2.ny()*p1.nz());
}

template <class Type>
vgl_homg_point_3d<Type>
vgl_homg_operators_3d<Type>::intersection(const vcl_vector<vgl_homg_plane_3d<Type> >& planes)
{
  int n = planes.size();
  vnl_matrix<Type> A(planes.size(), 4);

  for (int i =0; i < n; ++i) {
    A(i,0) = planes[i].nx();
    A(i,1) = planes[i].ny();
    A(i,2) = planes[i].nz();
    A(i,3) = planes[i].d();
  }

  vnl_svd<Type> svd(A);
  return vgl_homg_point_3d<Type>(svd.nullvector().begin());
}


template <class Type>
vnl_vector<Type> vgl_homg_operators_3d<Type>::get_vector(vgl_homg_point_3d<Type> const& p)
{
  // make a vnl_vector for the point p

  vnl_vector<Type> v(4);
  v.put(0,p.x());
  v.put(1,p.y());
  v.put(2,p.z());
  v.put(3,p.w());

  return v;
}

template <class Type>
vnl_vector<Type> vgl_homg_operators_3d<Type>::get_vector(vgl_homg_plane_3d<Type> const& p)
{
  // make a vnl_vector for the point p

  vnl_vector<Type> v(4);
  v.put(0,p.nx());
  v.put(1,p.ny());
  v.put(2,p.nz());
  v.put(3,p.d());

  return v;
}

template <class Type>
void vgl_homg_operators_3d<Type>::unitize(vgl_homg_point_3d<Type>& a)
{
  double norm = a.x()*a.x() + a.y()*a.y() + a.z()*a.z() + a.w()*a.w();

  if (norm == 0.0) {
    vcl_cerr << "vgl_homg_operators_3d<Type>::unitize() -- Zero length vector\n";
    return;
  }
  norm = 1.0/vcl_sqrt(norm);
  a.set(Type(a.x()*norm), Type(a.y()*norm), Type(a.z()*norm), Type(a.w()*norm));
}

//: Return the midpoint of the line joining two homogeneous points
//  When one of the points is at infinity, that point is returned.
//  When both points are at infinity, the invalid point (0,0,0,0) is returned.
template <class Type>
vgl_homg_point_3d<Type>
vgl_homg_operators_3d<Type>::midpoint(const vgl_homg_point_3d<Type>& p1,
                                      const vgl_homg_point_3d<Type>& p2)
{
  Type x = p1.x() * p2.w() + p2.x() * p1.w();
  Type y = p1.y() * p2.w() + p2.y() * p1.w();
  Type z = p1.z() * p2.w() + p2.z() * p1.w();
  Type w = p1.w() * p2.w() + p2.w() * p1.w();

  return vgl_homg_point_3d<Type>(x,y,z,w);
}

//: Intersect a set of 3D planes to find the least-square point of intersection.
// This finds the point $\bf x$ that minimizes $\|\tt L \bf x\|$, where $\tt L$
// is the matrix whose rows are the planes. The implementation uses vnl_svd
// to accumulate and compute the nullspace of $\tt L^\top \tt L$.
template <class Type>
vgl_homg_point_3d<Type>
vgl_homg_operators_3d<Type>::planes_to_point(const vcl_vector<vgl_homg_plane_3d<Type> >& planes)
{
  assert(planes.size() >= 3);

  vnl_vector<Type> mov = most_orthogonal_vector_svd(planes);
  return vgl_homg_point_3d<Type>(mov[0], mov[1], mov[2], mov[3]);
}

template <class Type>
double vgl_homg_operators_3d<Type>::cross_ratio(const vgl_homg_point_3d<Type>& a,
                                                const vgl_homg_point_3d<Type>& b,
                                                const vgl_homg_point_3d<Type>& c,
                                                const vgl_homg_point_3d<Type>& d)
{
  double x1 = a.x(), y1 = a.y(), z1 = a.z(), w1 = a.w();
  double x2 = b.x(), y2 = b.y(), z2 = b.z(), w2 = b.w();
  double x3 = c.x(), y3 = c.y(), z3 = c.z(), w3 = c.w();
  double x4 = d.x(), y4 = d.y(), z4 = d.z(), w4 = d.w();
  double x = x1 - x2; if (x<0) x = -x; // assuming a != b ;-)
  double y = y1 - y2; if (y<0) y = -y;
  double z = z1 - z2; if (z<0) z = -z;
  double n = (x>y && x>z) ? (x1*w3-x3*w1)*(x2*w4-x4*w2) :
             (y>z)        ? (y1*w3-y3*w1)*(y2*w4-y4*w2) :
                            (z1*w3-z3*w1)*(z2*w4-z4*w2);
  double m = (x>y && x>z) ? (x1*w4-x4*w1)*(x2*w3-x3*w2) :
             (y>z)        ? (y1*w4-y4*w1)*(y2*w3-y3*w2) :
                            (z1*w4-z4*w1)*(z2*w3-z3*w2);
  if (n == 0 && m == 0)
    vcl_cerr << "cross ratio not defined: three of the given points coincide\n";
  return n/m;
}

template <class Type>
double vgl_homg_operators_3d<Type>::cross_ratio(const vgl_homg_plane_3d<Type>& a,
                                                const vgl_homg_plane_3d<Type>& b,
                                                const vgl_homg_plane_3d<Type>& c,
                                                const vgl_homg_plane_3d<Type>& d)
{
  double x1 = a.a(), y1 = a.b(), z1 = a.c(), w1 = a.d();
  double x2 = b.a(), y2 = b.b(), z2 = b.c(), w2 = b.d();
  double x3 = c.a(), y3 = c.b(), z3 = c.c(), w3 = c.d();
  double x4 = d.a(), y4 = d.b(), z4 = d.c(), w4 = d.d();
  double x = x1 - x2; if (x<0) x = -x; // assuming a != b ;-)
  double y = y1 - y2; if (y<0) y = -y;
  double z = z1 - z2; if (z<0) z = -z;
  double n = (x>y && x>z) ? (x1*w3-x3*w1)*(x2*w4-x4*w2) :
             (y>z)        ? (y1*w3-y3*w1)*(y2*w4-y4*w2) :
                            (z1*w3-z3*w1)*(z2*w4-z4*w2);
  double m = (x>y && x>z) ? (x1*w4-x4*w1)*(x2*w3-x3*w2) :
             (y>z)        ? (y1*w4-y4*w1)*(y2*w3-y3*w2) :
                            (z1*w4-z4*w1)*(z2*w3-z3*w2);
  if (n == 0 && m == 0)
    vcl_cerr << "cross ratio not defined: three of the given planes coincide\n";
  return n/m;
}

//: Conjugate point of three given collinear points.
// If cross ratio cr is given (default: -1), the generalized conjugate point
// returned is such that ((x1,x2;x3,answer)) = cr.
template <class T>
vgl_homg_point_3d<T>
vgl_homg_operators_3d<T>::conjugate(const vgl_homg_point_3d<T>& a,
                                    const vgl_homg_point_3d<T>& b,
                                    const vgl_homg_point_3d<T>& c,
                                    double cr)
// Default for cr is -1.
{
  T x1 = a.x(), y1 = a.y(), z1 = a.z(), w1 = a.w();
  T x2 = b.x(), y2 = b.y(), z2 = b.z(), w2 = b.w();
  T x3 = c.x(), y3 = c.y(), z3 = c.z(), w3 = c.w();
  T kx = x1*w3 - x3*w1, mx = x2*w3 - x3*w2, nx = T(kx*w2-cr*mx*w1);
  T ky = y1*w3 - y3*w1, my = y2*w3 - y3*w2, ny = T(ky*w2-cr*my*w1);
  T kz = z1*w3 - z3*w1, mz = z2*w3 - z3*w2, nz = T(kz*w2-cr*mz*w1);
  return vgl_homg_point_3d<T>(T(x2*kx-cr*x1*mx)*ny*nz,T(y2*ky-cr*y1*my)*nx*nz,T(z2*kz-cr*z1*mz)*nx*ny,nx*ny*nz);
}

template <class T>
double
vgl_homg_operators_3d<T>::perp_dist_squared(const vgl_homg_point_3d<T>& point,
                                            const vgl_homg_plane_3d<T>& plane)
{
  if ((plane.a()==0 && plane.b()== 0 && plane.c()== 0) || point.w()==0) {
    vcl_cerr << "vgl_homg_operators_3d<T>::perp_dist_squared() -- plane or point at infinity\n";
    return 1e38;
  }

#define dot(p,q) p.a()*q.x()+p.b()*q.y()+p.c()*q.z()+p.d()*q.w()
  double numerator = dot(plane,point) / point.w();
#undef dot
  if (numerator == 0) return 0.0;
  double denominator = plane.a()*plane.a() + plane.b()*plane.b() + plane.c()*plane.c();
  return numerator * numerator / denominator;
}

template <class T>
vnl_vector<T>
vgl_homg_operators_3d<T>::most_orthogonal_vector_svd(const vcl_vector<vgl_homg_plane_3d<T> >& planes)
{
  vnl_matrix<T> D(planes.size(), 4);

  typename vcl_vector<vgl_homg_plane_3d<T> >::const_iterator i = planes.begin();
  for (unsigned j = 0; i != planes.end(); ++i,++j)
    D.set_row(j, get_vector(*i));

  vnl_svd<T> svd(D);
  return svd.nullvector();
}

//: Homographic transformation of a 3D point through a 4x4 projective transformation matrix
template <class T>
vgl_homg_point_3d<T> operator*(vnl_matrix_fixed<T,4,4> const& m,
                               vgl_homg_point_3d<T> const& p)
{
  return vgl_homg_point_3d<T>(m(0,0)*p.x()+m(0,1)*p.y()+m(0,2)*p.z()+m(0,3)*p.w(),
                              m(1,0)*p.x()+m(1,1)*p.y()+m(1,2)*p.z()+m(1,3)*p.w(),
                              m(2,0)*p.x()+m(2,1)*p.y()+m(2,2)*p.z()+m(2,3)*p.w(),
                              m(3,0)*p.x()+m(3,1)*p.y()+m(3,2)*p.z()+m(3,3)*p.w());
}

//: Homographic transformation of a 3D plane through a 4x4 projective transformation matrix
template <class T>
vgl_homg_plane_3d<T> operator*(vnl_matrix_fixed<T,4,4> const& m,
                               vgl_homg_plane_3d<T> const& p)
{
  return vgl_homg_plane_3d<T>(m(0,0)*p.a()+m(0,1)*p.b()+m(0,2)*p.c()+m(0,3)*p.d(),
                              m(1,0)*p.a()+m(1,1)*p.b()+m(1,2)*p.c()+m(1,3)*p.d(),
                              m(2,0)*p.a()+m(2,1)*p.b()+m(2,2)*p.c()+m(2,3)*p.d(),
                              m(3,0)*p.a()+m(3,1)*p.b()+m(3,2)*p.c()+m(3,3)*p.d());
}

//: Project a 3D point to 2D through a 3x4 projective transformation matrix
template <class T>
vgl_homg_point_2d<T> operator*(vnl_matrix_fixed<T,3,4> const& m,
                               vgl_homg_point_3d<T> const& p)
{
  return vgl_homg_point_2d<T>(m(0,0)*p.x()+m(0,1)*p.y()+m(0,2)*p.z()+m(0,3)*p.w(),
                              m(1,0)*p.x()+m(1,1)*p.y()+m(1,2)*p.z()+m(1,3)*p.w(),
                              m(2,0)*p.x()+m(2,1)*p.y()+m(2,2)*p.z()+m(2,3)*p.w());
}

//: Backproject a 2D line through a 4x3 projective transformation matrix
template <class T>
vgl_homg_plane_3d<T> operator*(vnl_matrix_fixed<T,4,3> const& m,
                               vgl_homg_line_2d<T> const& l)
{
  return vgl_homg_plane_3d<T>(m(0,0)*l.a()+m(0,1)*l.b()+m(0,2)*l.c(),
                              m(1,0)*l.a()+m(1,1)*l.b()+m(1,2)*l.c(),
                              m(2,0)*l.a()+m(2,1)*l.b()+m(2,2)*l.c(),
                              m(3,0)*l.a()+m(3,1)*l.b()+m(3,2)*l.c());
}

#undef VGL_HOMG_OPERATORS_3D_INSTANTIATE
#define VGL_HOMG_OPERATORS_3D_INSTANTIATE(T) \
  template class vgl_homg_operators_3d<T >; \
  template vgl_homg_point_3d<T > operator*(vnl_matrix_fixed<T,4,4> const&,\
                                           vgl_homg_point_3d<T > const&); \
  template vgl_homg_plane_3d<T > operator*(vnl_matrix_fixed<T,4,4> const&,\
                                           vgl_homg_plane_3d<T > const&); \
  template vgl_homg_point_2d<T > operator*(vnl_matrix_fixed<T,3,4> const&,\
                                           vgl_homg_point_3d<T > const&); \
  template vgl_homg_plane_3d<T > operator*(vnl_matrix_fixed<T,4,3> const&,\
                                           vgl_homg_line_2d<T > const&)

#endif // vgl_homg_operators_3d_txx_
