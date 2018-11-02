// This is core/vgl/algo/vgl_intersection.hxx
#ifndef vgl_algo_intersection_hxx_
#define vgl_algo_intersection_hxx_
//:
// \file
// \author Gamze Tunali

#include "vgl_intersection.h"

#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_polygon.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_svd.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
vgl_point_3d<T> vgl_intersection(std::vector<vgl_plane_3d<T> > const& p)
{
  std::vector<vgl_homg_plane_3d<T> > planes;
  for (unsigned i=0; i<p.size(); ++i) {
    planes.push_back(vgl_homg_plane_3d<T> (p[i]));
  }

  return vgl_homg_operators_3d<T>::intersection(planes);
}


template <class T>
vgl_infinite_line_3d<T>
vgl_intersection(const std::list<vgl_plane_3d<T> >& planes)
{
  if (planes.size() == 0)
    return vgl_infinite_line_3d<T>(vgl_vector_2d<T>(0,0),vgl_vector_3d<T>(0,0,0));

  // form the matrix of plane normal monomials
  vnl_matrix<double> Q(3,3,0.0);
  vnl_vector<double> vd(3,0.0);
  unsigned n = planes.size();
  for (typename std::list<vgl_plane_3d<T> >::const_iterator pit = planes.begin();
       pit != planes.end(); ++pit)
  {
    double a = (*pit).a(), b = (*pit).b(), c = (*pit).c(),d = (*pit).d();
    Q[0][0] += a*a; Q[0][1] += a*b; Q[0][2] += a*c;
    Q[1][1] += b*b; Q[1][2] += b*c;
    Q[2][2] += c*c;
    vd[0]-=a*d; vd[1]-=b*d; vd[2]-=c*d;
  }
  Q[1][0]=  Q[0][1];   Q[2][0]= Q[0][2];   Q[2][1]=  Q[1][2];
  Q/=n;
  vd/=n;
  vnl_svd<double> svd(Q);

  // the direction of the resulting line
  vnl_vector<double> t = svd.nullvector();

  double tx = t[0], ty = t[1], tz = t[2];
  double atx = std::fabs(tx), aty = std::fabs(ty), atz = std::fabs(tz);
  // determine maximum component of t
  char component = 'x';
  if (aty>atx&&aty>atz)
    component = 'y';
  if (atz>atx&&atz>aty)
    component = 'z';
  vgl_point_3d<double> p0d;
  switch (component)
  {
    case 'x':
    {
      double det = Q[1][1]*Q[2][2] - Q[1][2]*Q[2][1];
      double neuy = vd[1]*Q[2][2]  - Q[1][2]*vd[2];
      double neuz = Q[1][1]*vd[2]  - vd[1]*Q[2][1];
      p0d.set(0.0, neuy/det, neuz/det);
      break;
    }
    case 'y':
    {
      double det = Q[0][0]*Q[2][2] - Q[0][2]*Q[2][0];
      double neux = vd[0]*Q[2][2]  - Q[0][2]*vd[2];
      double neuz = Q[0][0]*vd[2]  - vd[0]*Q[2][0];
      p0d.set(neux/det, 0.0, neuz/det);
      break;
    }
    case 'z':
    default:
    {
      double det = Q[0][0]*Q[1][1] - Q[0][1]*Q[1][0];
      double neux = vd[0]*Q[1][1]  - Q[0][1]*vd[1];
      double neuy = Q[0][0]*vd[1]  - vd[0]*Q[1][0];
      p0d.set(neux/det, neuy/det, 0.0);
      break;
    }
  }
  vgl_point_3d<T> pt(static_cast<T>(p0d.x()),
                     static_cast<T>(p0d.y()),
                     static_cast<T>(p0d.z()));

  vgl_vector_3d<T> tv(static_cast<T>(tx),
                      static_cast<T>(ty),
                      static_cast<T>(tz));

  return vgl_infinite_line_3d<T>(pt, tv);
}

template <class T>
bool
vgl_intersection(const std::list<vgl_plane_3d<T> >& planes, std::vector<T> ws,
                 vgl_infinite_line_3d<T>& line, T &residual)
{
  if (planes.size() < 2)
    return false;
  // form the matrix of plane normal monomials
  vnl_matrix<double> Q(3,3,0.0);
  vnl_vector<double> vd(3,0.0);
  unsigned cnt=0;
  T sum_ws=0;
  for (typename std::list<vgl_plane_3d<T> >::const_iterator pit = planes.begin();
       pit != planes.end(); ++pit)
  {
    double a = (*pit).a(), b = (*pit).b(), c = (*pit).c(), d = (*pit).d();
    T w=ws[cnt++];
    Q[0][0] += w*a*a;
    Q[0][1] += w*a*b; Q[1][1] += w*b*b;
    Q[0][2] += w*a*c; Q[1][2] += w*b*c; Q[2][2] += w*c*c;
    vd[0]-=w*a*d; vd[1]-=w*b*d; vd[2]-=w*c*d;
    sum_ws+=w;
  }
  Q[1][0]=  Q[0][1];   Q[2][0]= Q[0][2];   Q[2][1]=  Q[1][2];
  Q/=sum_ws;
  vd/=sum_ws;
  vnl_svd<double> svd(Q);
  // the direction of the resulting line
  vnl_vector<double> t = svd.nullvector();
  double tx = t[0], ty = t[1], tz = t[2];
  double atx = std::fabs(tx), aty = std::fabs(ty), atz = std::fabs(tz);
  // determine maximum component of t
  char component = 'x';
  if (aty>atx&&aty>atz)
    component = 'y';
  if (atz>atx&&atz>aty)
    component = 'z';
  vgl_point_3d<double> p0d;
  switch (component)
  {
    case 'x':
    {
      double det = Q[1][1]*Q[2][2] - Q[1][2]*Q[2][1];
      double neuy = vd[1]*Q[2][2]  - Q[1][2]*vd[2];
      double neuz = Q[1][1]*vd[2]  - vd[1]*Q[2][1];
      p0d.set(0.0, neuy/det, neuz/det);
      break;
    }
    case 'y':
    {
      double det = Q[0][0]*Q[2][2] - Q[0][2]*Q[2][0];
      double neux = vd[0]*Q[2][2]  - Q[0][2]*vd[2];
      double neuz = Q[0][0]*vd[2]  - vd[0]*Q[2][0];
      p0d.set(neux/det, 0.0, neuz/det);
      break;
    }
    case 'z':
    default:
    {
      double det = Q[0][0]*Q[1][1] - Q[0][1]*Q[1][0];
      double neux = vd[0]*Q[1][1]  - Q[0][1]*vd[1];
      double neuy = Q[0][0]*vd[1]  - vd[0]*Q[1][0];
      p0d.set(neux/det, neuy/det, 0.0);
      break;
    }
  }
  vgl_point_3d<T> pt(static_cast<T>(p0d.x()),
                     static_cast<T>(p0d.y()),
                     static_cast<T>(p0d.z()));

  vgl_vector_3d<T> tv(static_cast<T>(tx),
                      static_cast<T>(ty),
                      static_cast<T>(tz));
  residual=T(0);
  sum_ws=0;
  cnt=0;
  for (typename std::list<vgl_plane_3d<T> >::const_iterator pit = planes.begin();
       pit != planes.end(); ++pit)
  {
    double a = pit->normal().x(), b = pit->normal().y(), c = pit->normal().z();
    residual+=ws[cnt]*ws[cnt]*T(a*tx+b*ty+c*tz)*T(a*tx+b*ty+c*tz);
    sum_ws+=ws[cnt]*ws[cnt];
    cnt++;
  }

  if (cnt>0)
  {
    residual/=sum_ws;
    residual=std::sqrt(residual);
  }
  line=vgl_infinite_line_3d<T>(pt, tv);
  return true;
}


template <class T>
bool vgl_intersection(vgl_box_3d<T> const& b, std::list<vgl_point_3d<T> >& poly)
{
  // check if two bounding boxes intersect
  // find the bounding box of the polygon
  assert(poly.size() >= 3);

  vgl_box_3d<T> bb;
  typename std::list<vgl_point_3d<T> >::iterator it=poly.begin();
  for (; it != poly.end(); ++it)
    bb.add(*it);

  vgl_box_3d<T> inters = vgl_intersection(b, bb);
  if (inters.is_empty())
    return false;

  // check if the polygon corners inside the box
  for (it=poly.begin(); it != poly.end(); ++it) {
    if (b.contains(*it))
      return true;
  }

  it=poly.begin();
  // get the first 3 points to create a plane
  vgl_point_3d<T> p0=*it; ++it;
  vgl_point_3d<T> p1=*it; ++it;
  vgl_point_3d<T> p2=*it; ++it;
  // create a plane from polygon
  vgl_plane_3d<T> poly_plane(p0,p1,p2);
  if (!vgl_intersection<T>(b, poly_plane))
    return false;

  // now we do a 3D transformation of the polygon and the box center to the plane
  // where polygon resides, so that we can do 2D poly-point test
  vgl_vector_3d<T> n = poly_plane.normal();
  n=normalize(n);
  vgl_vector_3d<T> u(p1-p0);
  u=normalize(u);
  vgl_vector_3d<T> v = cross_product(n,u);

  vnl_matrix<T> M(3,3);
  M.put(0,0,u.x());
  M.put(1,0,u.y());
  M.put(2,0,u.z());
  M.put(0,1,v.x());
  M.put(1,1,v.y());
  M.put(2,1,v.z());
  M.put(0,2,n.x());
  M.put(1,2,n.y());
  M.put(2,2,n.z());

  vnl_matrix_inverse<T> R(M);

  // transform the polygon
  vgl_polygon<T> poly2d(1);  // with one sheet
  for (it=poly.begin(); it != poly.end(); ++it) {
    vgl_vector_3d<T> temp(*it-p0);
    vnl_matrix<T> tv(3,1);
    tv.put(0,0,temp.x());
    tv.put(1,0,temp.y());
    tv.put(2,0,temp.z());
    vnl_matrix<T> pi = R*tv;
    poly2d.push_back(pi.get(0,0), pi.get(1,0));
  }

  vgl_point_3d<T> c=b.centroid();
  vnl_matrix<T> tv(3,1);
  tv.put(0,0,c.x()-p0.x());
  tv.put(1,0,c.y()-p0.y());
  tv.put(2,0,c.z()-p0.z());
  vnl_matrix<T> ci(R*tv);
  return poly2d.contains(ci.get(0,0),ci.get(1,0));
}

#undef VGL_ALGO_INTERSECTION_INSTANTIATE
#define VGL_ALGO_INTERSECTION_INSTANTIATE(T) \
template vgl_point_3d<T > vgl_intersection(const std::vector<vgl_plane_3d<T > >&); \
template bool vgl_intersection(vgl_box_3d<T > const&, std::list<vgl_point_3d<T > >&); \
template vgl_infinite_line_3d<T > vgl_intersection(const std::list<vgl_plane_3d<T > >& planes); \
template bool vgl_intersection(const std::list<vgl_plane_3d<T > >& planes, std::vector<T > ws,vgl_infinite_line_3d<T >&,T& residual)

#endif // vgl_algo_intersection_hxx_
