// This is brl/bbas/imesh/algo/imesh_intersect.cxx
#include <iostream>
#include <limits>
#include "imesh_intersect.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vgl/vgl_triangle_3d.h>
#include <vgl/vgl_distance.h>


namespace {

//: compute barycentric coordinates (u,v) assuming that \a p is in the plane of triangle (a,b,c)
void compute_barycentric(const vgl_point_3d<double>& p,
                         const vgl_point_3d<double>& a,
                         const vgl_point_3d<double>& b,
                         const vgl_point_3d<double>& c,
                         double& u, double& v)
{
  vgl_vector_3d<double> vp(p-a);
  vgl_vector_3d<double> vu(b-a);
  vgl_vector_3d<double> vv(c-a);
  vgl_vector_3d<double> n(cross_product(vu,vv));
  vgl_vector_3d<double> nxu(cross_product(n,vu));
  vgl_vector_3d<double> nxv(cross_product(n,vv));
  u = dot_product(vp,nxv)/dot_product(vu,nxv);
  v = dot_product(vp,nxu)/dot_product(vv,nxu);
}
// end of namespace
};


//: Intersect the ray from point p with direction d and the triangle defined by a,b,c
//  \returns true if intersection occurs
//  \param dist is the distance to the triangle (returned by reference)
//  \param u and \param v are the barycentric coordinates of the intersection
bool imesh_intersect_triangle(const vgl_point_3d<double>& p,
                              const vgl_vector_3d<double>& d,
                              const vgl_point_3d<double>& a,
                              const vgl_point_3d<double>& b,
                              const vgl_point_3d<double>& c,
                              double& dist,
                              double& u, double& v)
{
  vgl_vector_3d<double> n(cross_product(b-a,c-a));
  return imesh_intersect_triangle(p,d,a,b,c,n,dist,u,v);
}


//: Intersect the ray from point p with direction d and the triangle defined by a,b,c
//  The un-normalized normal vector (b-a)x(c-a) is precomputed and also passed in
//  \returns true if intersection occurs
//  \param dist is the distance to the triangle (returned by reference)
//  \param u and \param v are the barycentric coordinates of the intersection
bool imesh_intersect_triangle(const vgl_point_3d<double>& p,
                              const vgl_vector_3d<double>& d,
                              const vgl_point_3d<double>& a,
                              const vgl_point_3d<double>& b,
                              const vgl_point_3d<double>& c,
                              const vgl_vector_3d<double>& n,
                              double& dist,
                              double& u, double& v)
{
  double denom = -dot_product(d,n);
  if (denom <= 0) // back facing triangles
    return false;

  vgl_vector_3d<double> ap(p-a);
  vgl_vector_3d<double> t(cross_product(d,ap));
  v = dot_product(b-p,t);
  if (v < 0.0 || v>denom)
    return false;

  u = -dot_product(c-p,t);
  if (u < 0.0 || u+v > denom)
    return false;

  dist = dot_product(ap,n);
  if (dist < 0.0)
    return false;

  u /= denom;
  v /= denom;
  dist /= denom;

  return true;
}


//: Intersect the ray from point p with direction d and the triangle defined by a,b,c
//  The un-normalized normal vector (b-a)x(c-a) is precomputed and also passed in
//  \returns true if intersection occurs and the new dist is less than the old distance (but > 0)
//  \param dist is the distance to the triangle (returned by reference)
//  \param u and \param v are the barycentric coordinates of the intersection
bool imesh_intersect_triangle_min_dist(const vgl_point_3d<double>& p,
                                       const vgl_vector_3d<double>& d,
                                       const vgl_point_3d<double>& a,
                                       const vgl_point_3d<double>& b,
                                       const vgl_point_3d<double>& c,
                                       const vgl_vector_3d<double>& n,
                                       double& dist,
                                       double& u, double& v)
{
  double denom = -dot_product(d,n);
  if (denom <= 0) // back facing triangles
    return false;

  vgl_vector_3d<double> ap(p-a);
  double new_dist = dot_product(ap,n)/denom;
  if (new_dist < 0.0 || new_dist > dist)
    return false;

  vgl_vector_3d<double> t(cross_product(d,ap));
  v = dot_product(b-p,t);
  if (v < 0.0 || v>denom)
    return false;

  u = -dot_product(c-p,t);
  if (u < 0.0 || u+v > denom)
    return false;

  dist = new_dist;
  u /= denom;
  v /= denom;

  return true;
}


//: Intersect the ray from point p with direction d and the triangulated mesh
//  \returns the face index of the closest intersecting triangle
//  \param dist is the distance to the triangle (returned by reference)
//  \param u and \param v (optional) are the barycentric coordinates of the intersection
int imesh_intersect_min_dist(const vgl_point_3d<double>& p,
                             const vgl_vector_3d<double>& d,
                             const imesh_mesh& mesh,
                             double& dist, double* u, double* v)
{
  double ut, vt; // temporary u and v

  assert(mesh.faces().regularity() == 3);
  const auto& faces
      = static_cast<const imesh_regular_face_array<3>&>(mesh.faces());

  const imesh_vertex_array<3>& verts = mesh.vertices<3>();

  assert(faces.has_normals());
  assert((cross_product(vgl_point_3d<double>(verts[faces[0][1]])-vgl_point_3d<double>(verts[faces[0][0]]),
                        vgl_point_3d<double>(verts[faces[0][2]])-vgl_point_3d<double>(verts[faces[0][0]]))
         -faces.normal(0)).length() < 1e-14);

  int isect = -1;
  dist = std::numeric_limits<double>::infinity();
  for (unsigned int i=0; i<faces.size(); ++i)
  {
    const imesh_regular_face<3>& f = faces[i];
    if (imesh_intersect_triangle_min_dist(p,d,verts[f[0]],verts[f[1]],verts[f[2]],
                                         faces.normal(i),dist,ut,vt))
    {
      isect = i;
      if (u) *u = ut;
      if (v) *v = vt;
    }
  }
  return isect;
}


//: Find the closest point on the triangle a,b,c to point p
//  The un-normalized normal vector (b-a)x(c-a) is precomputed and also passed in
//  \returns a code indicating that the closest point:
//  - 0 does not exist (should not occur)
//  - 1 is \a a
//  - 2 is \a b
//  - 3 is on the edge from \a a to \a b
//  - 4 is \a c
//  - 5 is on the edge from \a a to \a c
//  - 6 is on the edge from \a b to \a c
//  - 7 is on the face of the triangle
//  \param dist is the distance to the triangle (returned by reference)
//  \param u and \param v are the barycentric coordinates of the closest point
unsigned char
imesh_triangle_closest_point(const vgl_point_3d<double>& p,
                             const vgl_point_3d<double>& a,
                             const vgl_point_3d<double>& b,
                             const vgl_point_3d<double>& c,
                             const vgl_vector_3d<double>& n,
                             double& dist,
                             double& u, double& v)
{
  double denom = 1.0/dot_product(n,n);

  vgl_vector_3d<double> ap(p-a);
  vgl_vector_3d<double> bp(p-b);
  vgl_vector_3d<double> cp(p-c);

  vgl_vector_3d<double> t(cross_product(n,ap));
  v = dot_product(bp,t) * denom;
  u = -dot_product(cp,t) * denom;

  vgl_vector_3d<double> ab(b-a);
  vgl_vector_3d<double> bc(c-b);
  vgl_vector_3d<double> ca(a-c);

  double eps = std::numeric_limits<double>::epsilon();

  unsigned char state = 0;
  double uv;
  if (u <= eps) {
    double p_v = v - u * dot_product(ab,ca)/dot_product(ca,ca);
    if (p_v <= eps) {
      state = 1;
    }
    else if (p_v >= 1.0) {
      state = 4;
    }
    else {
      u = 0.0; v = p_v;
      dist = ((1-v)*ap + v*cp).length();
      return 5;
    }
  }
  if (v <= eps) {
    double p_u = u - v * dot_product(ca,ab)/dot_product(ab,ab);
    if (p_u <= eps) {
      state = 1;
    }
    else if (p_u >= 1.0) {
      state = 2;
    }
    else {
      u = p_u; v = 0.0;
      dist = ((1-u)*ap + u*bp).length();
      return 3;
    }
  }
  if ((uv = 1.0-u-v) <= eps) {
    double s = -dot_product(ca,bc)/dot_product(bc,bc);
    double p_u = u + uv * s;
    double p_v = v + uv * (1.0-s);
    if (p_v <= eps) {
      state = 2;
    }
    else if (p_u <= eps) {
      state = 4;
    }
    else {
      u=p_u; v=p_v;
      dist = (u*bp + v*cp).length();
      return 6;
    }
  }

  switch (state)
  {
    case 1:
      u=0.0; v=0.0;
      dist = ap.length();
      return 1;
    case 2:
      u=1.0; v=0.0;
      dist = bp.length();
      return 2;
    case 4:
      u=0.0; v=1.0;
      dist = cp.length();
      return 4;
    default:
      dist = std::abs(dot_product(ap,n) * std::sqrt(denom));
      return 7;
  }

  return 0;
}


//: Find the closest point on the triangle a,b,c to point p
//  \returns a code same as other version of this function
//  \param dist is the distance to the triangle (returned by reference)
//  \param u and \param v are the barycentric coordinates of the closest point
unsigned char
imesh_triangle_closest_point(const vgl_point_3d<double>& p,
                             const vgl_point_3d<double>& a,
                             const vgl_point_3d<double>& b,
                             const vgl_point_3d<double>& c,
                             double& dist,
                             double& u, double& v)
{
  vgl_vector_3d<double> n(cross_product(b-a,c-a));
  return imesh_triangle_closest_point(p,a,b,c,n,dist,u,v);
}


//: Find the closest point on the triangle a,b,c to point p
//  \param dist is the distance to the triangle (returned by reference)
vgl_point_3d<double>
imesh_triangle_closest_point(const vgl_point_3d<double>& p,
                             const vgl_point_3d<double>& a,
                             const vgl_point_3d<double>& b,
                             const vgl_point_3d<double>& c,
                             double& dist)
{
  double u,v;
  imesh_triangle_closest_point(p,a,b,c,dist,u,v);
  double t = 1-u-v;
  return {t*a.x() + u*b.x() + v*c.x(),
                              t*a.y() + u*b.y() + v*c.y(),
                              t*a.z() + u*b.z() + v*c.z()};
}

//: Find the closest point on the triangulated mesh to point p
//  \returns the face index of the closest triangle (one of them if on an edge or vertex)
//  \param cp is the closest point on the mesh (returned by reference)
//  \param u and \param v (optional) are the barycentric coordinates of the closest point
int imesh_closest_point(const vgl_point_3d<double>& p,
                        const imesh_mesh& mesh,
                        vgl_point_3d<double>& cp,
                        double* u, double* v)
{
  assert(mesh.faces().regularity() == 3);
  const auto& faces
      = static_cast<const imesh_regular_face_array<3>&>(mesh.faces());

  const imesh_vertex_array<3>& verts = mesh.vertices<3>();

  int isect = -1;
  double dist = std::numeric_limits<double>::infinity();
  for (unsigned int i=0; i<faces.size(); ++i)
  {
    const imesh_regular_face<3>& f = faces[i];
    vgl_point_3d<double> cpt = vgl_triangle_3d_closest_point(p,verts[f[0]],verts[f[1]],verts[f[2]]);
    double cp_dist = vgl_distance(cpt,p);
    if (cp_dist < dist)
    {
      isect = i;
      dist = cp_dist;
      cp = cpt;
      double ut, vt;
      compute_barycentric(cp,verts[f[0]],verts[f[1]],verts[f[2]],ut,vt);
      if (u) *u = ut;
      if (v) *v = vt;
    }
  }
  return isect;
}


//: Find the closest intersection point from p along d with triangle a,b,c
//  \returns a code indicating that the intersection point:
//  - 0 does not exist
//  - 1 is \a a
//  - 2 is \a b
//  - 3 is on the edge from \a a to \a b
//  - 4 is \a c
//  - 5 is on the edge from \a a to \a c
//  - 6 is on the edge from \a b to \a c
//  - 7 could not be computed (error)
//  \param u and \param v are the barycentric coordinates of the intersection
unsigned char
imesh_triangle_intersect(const vgl_point_2d<double>& p,
                         const vgl_vector_2d<double>& d,
                         const vgl_point_2d<double>& a,
                         const vgl_point_2d<double>& b,
                         const vgl_point_2d<double>& c,
                         double& u, double& v)
{
  vgl_vector_2d<double> v1(b-a), v2(c-a), vp(p-a);
  vgl_vector_2d<double> v1n(v1.y(), -v1.x());
  v1n /= dot_product(v1n,v2);
  vgl_vector_2d<double> v2n(v2.y(), -v2.x());
  v2n /= dot_product(v2n,v1);
  v = dot_product(v1n,vp);
  u = dot_product(v2n,vp);

  double dv = dot_product(d,v1n);
  double du = dot_product(d,v2n);

  return imesh_triangle_intersect(u,v,du,dv);
}


//: Find the closest intersection point in barycentric coordinates along the vector (du,dv) in barycentric coordinates
//  \returns a code indicating that the intersection point:
//  - 0 does not exist
//  - 1 is at corner (0,0)
//  - 2 is at corner (1,0)
//  - 3 is on the edge v=0
//  - 4 is at corner (0,1)
//  - 5 is on the edge from u=0
//  - 6 is on the edge from u+v=1
//  - 7 could not be computed (error)
//  \param u and \param v are updated to the coordinates of the intersection
unsigned char
imesh_triangle_intersect(double& u, double& v,
                         const double& du, const double& dv,
                         const double& eps)
{
  unsigned char state = 0;
  double best_t = std::numeric_limits<double>::infinity();
  if (du > 0.0)
  {
    double t = -u/du;
    double vi = v + t*dv;
    if (vi > eps && vi+eps < 1.0)
    {
      best_t = t;
      state = 5;
    }
    else if (std::abs(vi) <= eps)
    {
      best_t = t;
      state = 1;
    }
    else if (std::abs(1-vi) <= eps)
    {
      best_t = t;
      state = 4;
    }
  }
  if (dv > 0.0)
  {
    double t = -v/dv;
    double ui = u + t*du;
    if (ui > eps && ui+eps < 1.0)
    {
      best_t = t;
      state = 3;
    }
    else if (std::abs(ui) <= eps)
    {
      best_t = t;
      state = 1;
    }
    else if (std::abs(1-ui) <= eps)
    {
      best_t = t;
      state = 2;
    }
  }
  if (du+dv < 0.0)
  {
    double t = (1-u-v)/(du+dv);
    double ui = u + t*du;
    double vi = v + t*dv;
    if (ui > eps && vi > eps)
    {
      best_t = t;
      state = 6;
    }
    else if (std::abs(ui) <= eps)
    {
      best_t = t;
      state = 4;
    }
    else if (std::abs(vi) <= eps)
    {
      best_t = t;
      state = 2;
    }
  }

  u+=best_t*du;
  v+=best_t*dv;

  return state;
}
