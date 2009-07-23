// This is core/vgl/algo/vgl_intersection.txx
#ifndef vgl_algo_intersection_txx_
#define vgl_algo_intersection_txx_
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
#include <vnl/algo/vnl_matrix_inverse.h>

#include <vcl_cassert.h>

template <class T>
vgl_point_3d<T> vgl_intersection(vcl_vector<vgl_plane_3d<T> > const& p)
{
  vcl_vector<vgl_homg_plane_3d<T> > planes;
  for (unsigned i=0; i<p.size(); ++i) {
    planes.push_back(vgl_homg_plane_3d<T> (p[i]));
  }

  return vgl_homg_operators_3d<T>::intersection(planes);
}

template <class T>
bool vgl_intersection(vgl_box_3d<T> const& b, vcl_list<vgl_point_3d<T> >& poly)
{
  // check if two bounding boxes intersect
  // find the bounding box of the polygon
  assert(poly.size() >= 3);

  vgl_box_3d<T> bb;
  typename vcl_list<vgl_point_3d<T> >::iterator it=poly.begin();
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

  // find the box corners
  vcl_vector<vgl_point_3d<T> > corners;
  corners.push_back(b.min_point());
  corners.push_back(vgl_point_3d<T> (b.min_x()+b.width(), b.min_y(), b.min_z()));
  corners.push_back(vgl_point_3d<T> (b.min_x()+b.width(), b.min_y()+b.height(), b.min_z()));
  corners.push_back(vgl_point_3d<T> (b.min_x(), b.min_y()+b.height(), b.min_z()));
  corners.push_back(vgl_point_3d<T> (b.min_x(), b.min_y(), b.max_z()));
  corners.push_back(vgl_point_3d<T> (b.min_x()+b.width(), b.min_y(), b.max_z()));
  corners.push_back(b.max_point());
  corners.push_back(vgl_point_3d<T> (b.min_x(), b.min_y()+b.height(), b.max_z()));

  // find the signed distance from the box corners to the plane
  int pos=0, neg=0;
  for (unsigned c=0; c<corners.size(); ++c) {
    vgl_point_3d<T> corner=corners[c];
    double d=(poly_plane.a()*corner.x());
    d+=(poly_plane.b()*corner.y());
    d+=(poly_plane.c()*corner.z());
    d+=poly_plane.d();
    if (d > 0)
      ++pos;
    else if (d < 0)
      ++neg;
  }
  if (neg==8 || pos==8) // completely out of ploygon plane
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
template vgl_point_3d<T > vgl_intersection(const vcl_vector<vgl_plane_3d<T > >&); \
template bool vgl_intersection(vgl_box_3d<T > const&, vcl_list<vgl_point_3d<T > >&)
#endif // vgl_algo_intersection_txx_
