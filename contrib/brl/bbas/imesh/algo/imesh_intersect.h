// This is brl/bbas/imesh/algo/imesh_intersect.h
#ifndef imesh_intersect_h_
#define imesh_intersect_h_
//:
// \file
// \brief Functions for mesh intersections
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date May 9, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <iostream>
#include <limits>
#include <imesh/imesh_mesh.h>
#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Intersect the ray from point p with direction d and the triangle defined by a,b,c
//  \returns true if intersection occurs
//  \param dist is the distance to the triangle (returned by reference)
//  \param u and \param v are the barycentric coordinates of the intersection
//  Barycentric coordinates are u and v such that (1-u-v)*a + u*b + v*c = p+dist*d
bool imesh_intersect_triangle(const vgl_point_3d<double>& p,
                              const vgl_vector_3d<double>& d,
                              const vgl_point_3d<double>& a,
                              const vgl_point_3d<double>& b,
                              const vgl_point_3d<double>& c,
                              double& dist,
                              double& u, double& v);


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
                              double& u, double& v);


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
                                       double& u, double& v);

//: Intersect the ray from point p with direction d and the triangulated mesh
//  \returns the face index of the closest intersecting triangle
//  \param dist is the distance to the triangle (returned by reference)
//  \param u and \param v (optional) are the barycentric coordinates of the intersection
int imesh_intersect_min_dist(const vgl_point_3d<double>& p,
                             const vgl_vector_3d<double>& d,
                             const imesh_mesh& mesh,
                             double& dist, double* u=nullptr, double* v=nullptr);


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
                             double& u, double& v);


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
                             double& u, double& v);


//: Find the closest point on the triangle a,b,c to point p
//  \param dist is the distance to the triangle (returned by reference)
vgl_point_3d<double>
imesh_triangle_closest_point(const vgl_point_3d<double>& p,
                             const vgl_point_3d<double>& a,
                             const vgl_point_3d<double>& b,
                             const vgl_point_3d<double>& c,
                             double& dist);


//: Find the closest point on the triangulated mesh to point p
//  \returns the face index of the closest triangle (one of them if on an edge or vertex)
//  \param cp is the closest point on the mesh (returned by reference)
//  \param u and \param v (optional) are the barycentric coordinates of the closest point
int imesh_closest_point(const vgl_point_3d<double>& p,
                        const imesh_mesh& mesh,
                        vgl_point_3d<double>& cp,
                        double* u=nullptr, double* v=nullptr);


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
                         double& u, double& v);


//: Find the closest intersection point along the vector (du,dv)
//  Both the vector (du,dv) and the intersection point are in barycentric coordinates
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
                         const double& eps = std::numeric_limits<double>::epsilon());

#endif // imesh_intersect_h_
