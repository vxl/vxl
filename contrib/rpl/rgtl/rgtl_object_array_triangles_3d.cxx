//:
// \file
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <map>
#include <iostream>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include "rgtl_object_array_triangles_3d.h"

#include "rgtl_object_closest_polygon_3d.h"
#include "rgtl_separating_axis.h"
#include "rgtl_serialize_access.h"
#include "rgtl_serialize_base.h"
#include "rgtl_serialize_stl_vector.h"
#include "rgtl_serialize_vnl_vector_fixed.h"
#include "rgtl_serialize_vnl_double_3.h"
#include "rgtl_serialize_split.h"
#include "rgtl_serialize_istream.h"
#include "rgtl_serialize_ostream.h"

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_cross.h>


#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//----------------------------------------------------------------------------
// Represent the geometry of a single triangle for use by closest point
// computation.
class rgtl_oat3_geometry
{
 public:
  rgtl_oat3_geometry(rgtl_object_array_triangles_3d const* self,
                     int const point_ids[3])
  {
    self->get_point(point_ids[0], this->verts[0]);
    self->get_point(point_ids[1], this->verts[1]);
    self->get_point(point_ids[2], this->verts[2]);
    this->compute();
  }

  // Return the number of vertices.
  unsigned int get_number_of_vertices() const { return 3; }

  // Return the vertex at the given index.
  double const* get_vertex(unsigned int i) const
  {
    return this->verts[i];
  }

  // Return the tangent direction pointing along the right-handed
  // traversal of the given edge.
  double const* get_edge_tangent(unsigned int i) const
  {
    return this->edge_tangents[i];
  }

  // Return the direction normal to the edge and contained in the face
  // plane.  Orient the normal to point toward the face interior.
  double const* get_edge_normal(unsigned int i) const
  {
    return this->edge_normals[i];
  }

  // Return the direction normal to the polygon face.  A right-handed
  // walk corresponds to the finger curl when the thumb points along
  // this normal.
  double const* get_face_normal() const
  {
    return this->face_normal;
  }

 private:
  void compute()
  {
    for (int cur=0; cur < 3; ++cur)
    {
      int next = (cur+1)%3;
      for (int a=0; a < 3; ++a)
      {
        this->edge_tangents[cur][a] =
          this->verts[next][a] - this->verts[cur][a];
      }
    }
    this->cross_left(this->edge_tangents[0], this->edge_tangents[2],
                     this->face_normal);
    for (int i=0; i < 3; ++i)
    {
      this->cross_right(this->face_normal, this->edge_tangents[i],
                        this->edge_normals[i]);
    }
  }

  static void cross_right(double const u[3], double const v[3], double w[3])
  {
    w[0] = u[1] * v[2] - u[2] * v[1];
    w[1] = u[2] * v[0] - u[0] * v[2];
    w[2] = u[0] * v[1] - u[1] * v[0];
  }
  static void cross_left(double const u[3], double const v[3], double w[3])
  {
    w[0] = u[2] * v[1] - u[1] * v[2];
    w[1] = u[0] * v[2] - u[2] * v[0];
    w[2] = u[1] * v[0] - u[0] * v[1];
  }
  double verts[3][3];
  double edge_tangents[3][3];
  double edge_normals[3][3];
  double face_normal[3];
};

//----------------------------------------------------------------------------
// Private implementation of class rgtl_object_array_triangles_3d.
class rgtl_object_array_triangles_3d::pimpl
{
 public:
  // Construct for a given number of triangles.
  pimpl(int ntris)
  : triangle_computed_(ntris, 0),
    triangle_axes_(ntris),
    triangle_centers_(ntris),
    triangle_radii_(ntris),
    triangle_closest_features_(ntris)
  {
    have_pseudonormals = false;
  }

  // Allocate the structures below for a new triangle.
  void add_triangle()
  {
    this->set_number_of_triangles(triangle_axes_.size()+1);
  }
  void set_number_of_triangles(std::size_t n)
  {
    this->triangle_computed_.resize(n, 0);
    this->triangle_axes_.resize(n);
    this->triangle_centers_.resize(n);
    this->triangle_radii_.resize(n);
    this->triangle_closest_features_.resize(n);
  }
  void reserve_triangles(std::size_t n)
  {
    this->triangle_computed_.reserve(n);
    this->triangle_axes_.reserve(n);
    this->triangle_centers_.reserve(n);
    this->triangle_radii_.reserve(n);
    this->triangle_closest_features_.reserve(n);
  }

  // Store the flags for whether triangle axes have been computed.
  std::vector<char> triangle_computed_;

  // Store the axes that can separate each triangle from an axis-aligned
  // box.
  typedef rgtl_separating_axis<3, double> axis_type;
  struct triangle_axes_type { axis_type axes[13]; };
  std::vector<triangle_axes_type> triangle_axes_;

  // Store the minimum bounding sphere for each triangle.
  std::vector<vnl_double_3> triangle_centers_;
  std::vector<double> triangle_radii_;

  // Store the edge normal index for each triangle's edges.
  typedef vnl_vector_fixed<int, 3> triangle_edge_type;
  std::vector<triangle_edge_type> triangle_edges_ids_;

  // The helper class used to compute closest points.
  typedef rgtl_object_closest_polygon_3d<rgtl_oat3_geometry>
  closest_finder_type;

  // Store last closest feature for each triangle.
  struct closest_feature_type
  {
    closest_finder_type::feature_kind_type kind;
    int index;
  };
  std::vector<closest_feature_type> triangle_closest_features_;

  // Store whether or not normals have been computed.
  bool have_pseudonormals;

  // Store an angle-weighted pseudo-normal for each vertex.
  std::vector<vnl_double_3> vertex_normals_;

  // Store an angle-weighted pseudo-normal for each edge.
  std::vector<vnl_double_3> edge_normals_;

  // Store an angle-weighted pseudo-normal for each face.
  std::vector<vnl_double_3> face_normals_;

  // Map key for an edge.
  struct edge_key
  {
    int lower;
    int upper;
    edge_key(int a, int b)
    {
      if (a <= b) { lower = a; upper = b; }
      else { lower = b; upper = a; }
    }
    bool operator<(const edge_key& that) const
    {
      return this->lower < that.lower ||
             (this->lower == that.lower &&
              this->upper <  that.upper);
    }
  };

  // Math utilities.
  static double dot(double const u[3], double const v[3])
  {
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
  }
  static void cross(double const u[3], double const v[3], double w[3])
  {
    w[0] = u[1] * v[2] - u[2] * v[1];
    w[1] = u[2] * v[0] - u[0] * v[2];
    w[2] = u[0] * v[1] - u[1] * v[0];
  }
  static double normalize(double n[3])
  {
    double mag = std::sqrt(dot(n,n));
    if (mag > 0)
    {
      double mag_inv = 1/mag;
      n[0] *= mag_inv;
      n[1] *= mag_inv;
      n[2] *= mag_inv;
    }
    return mag;
  }
  static double square(double x) { return x*x; }

  // Compute the minimum bounding sphere containing a triangle.
  bool compute_minimum_ball(double const x0[3],
                            double const x1[3],
                            double const x2[3],
                            double c0[3],
                            double& radius_squared);

  // Check whether the given sphere intersects the bounding sphere
  // of a triangle.
  bool balls_intersect(unsigned int id,
                       double const center[3], double radius) const;

  // Compute the angle-weighted pseudo-normals.
  void compute_normals(rgtl_object_array_triangles_3d* self);
};

//----------------------------------------------------------------------------
rgtl_object_array_triangles_3d
::rgtl_object_array_triangles_3d(int npts, int ntris):
  derived(npts), triangles_(ntris), pimpl_(new pimpl(ntris))
{
}

//----------------------------------------------------------------------------
rgtl_object_array_triangles_3d
::~rgtl_object_array_triangles_3d()
{
  delete this->pimpl_;
}

//----------------------------------------------------------------------------
void
rgtl_object_array_triangles_3d::get_triangle(int id, int point_ids[3]) const
{
  this->triangles_[id].copy_out(point_ids);
}

//----------------------------------------------------------------------------
void
rgtl_object_array_triangles_3d::set_triangle(int id, int const point_ids[3])
{
  this->triangles_[id].copy_in(point_ids);
}

//----------------------------------------------------------------------------
int rgtl_object_array_triangles_3d::add_triangle(int const point_ids[3])
{
  triangle_type t(point_ids);
  int id = this->number_of_objects();
  this->triangles_.push_back(t);
  this->pimpl_->add_triangle();
  return id;
}

//----------------------------------------------------------------------------
void rgtl_object_array_triangles_3d::reserve_triangles(unsigned int n)
{
  this->triangles_.reserve(n);
  this->pimpl_->reserve_triangles(n);
}

//----------------------------------------------------------------------------
int rgtl_object_array_triangles_3d::number_of_objects() const
{
  return static_cast<int>(this->triangles_.size());
}

//----------------------------------------------------------------------------
bool
rgtl_object_array_triangles_3d
::object_intersects_object(int, int) const
{
  return false;
}

//----------------------------------------------------------------------------
bool
rgtl_object_array_triangles_3d
::object_intersects_box(int id,
                        double const center[3],
                        double const radius,
                        double const lower[3],
                        double const upper[3],
                        double const corners[8][3]) const
{
  this->compute_axes(id);

  // If the box bounding sphere does not intersect the triangle
  // bounding sphere we have a fast-rejection.
  if (!this->pimpl_->balls_intersect(id, center, radius))
  {
    return false;
  }

  // If any vertex is in the box we have a fast-accept.
  triangle_type const& t = this->triangles_[id];
  for (unsigned int i=0; i < 3; ++i)
  {
    if (this->derived::object_intersects_box(t[i], center, radius,
                                            lower, upper, corners))
    {
      return true;
    }
  }

  // Get the separating axes for this triangle and the box.
  pimpl::triangle_axes_type const& ta = this->pimpl_->triangle_axes_[id];

  // Project the box onto each axis to check for separation from the
  // triangle.
  for (unsigned int j=0; j < 13; ++j)
  {
    pimpl::axis_type ba(ta.axes[j].axis());
    for (unsigned int k=0; k < 8; ++k)
    {
      ba.update(corners[k]);
    }
    if (pimpl::axis_type::projection_type::disjoint(ba.projection(),
                                                    ta.axes[j].projection()))
    {
      return false;
    }
  }

  // No separating axis was found.  The triangle does intersect
  // the box.
  return true;
}

//----------------------------------------------------------------------------
bool
rgtl_object_array_triangles_3d
::object_closest_point(int id,
                       double const x[3],
                       double y[3],
                       double bound_squared) const
{
  this->compute_axes(id);

  // If the triangle bounding ball does not intersect the query ball
  // skip computing the closest point.
  if (bound_squared >= 0 &&
      !this->pimpl_->balls_intersect(id, x, std::sqrt(bound_squared)))
  {
    return false;
  }

  // Get the triangle geometry.
  triangle_type const& t = this->triangles_[id];
  rgtl_oat3_geometry g(this, t.data_block());

  // We will record the closest feature on the triangle.
  pimpl::closest_feature_type& closest_feature =
    this->pimpl_->triangle_closest_features_[id];

  // Find the closest point.
  pimpl::closest_finder_type finder(g);
  finder.compute_closest_point(x, y,
                               closest_feature.kind,
                               closest_feature.index);
  return true;
}

//----------------------------------------------------------------------------
void
rgtl_object_array_triangles_3d
::compute_axes(int id) const
{
  // Compute triangle information at most once.
  if (this->pimpl_->triangle_computed_[id])
  {
    return;
  }
  this->pimpl_->triangle_computed_[id] = 1;

  // Get references to the structures for the triangle of interest.
  triangle_type const& t = this->triangles_[id];
  pimpl::triangle_axes_type& ta = this->pimpl_->triangle_axes_[id];
  vnl_double_3& tc = this->pimpl_->triangle_centers_[id];
  double& tr = this->pimpl_->triangle_radii_[id];

  // Compute canonical directions.
  vnl_double_3 ijk[3];
  ijk[0] = vnl_double_3(1, 0, 0);
  ijk[1] = vnl_double_3(0, 1, 0);
  ijk[2] = vnl_double_3(0, 0, 1);

  // Get triangle vertices.
  vnl_double_3 verts[3];
  this->get_point(t[0], verts[0].data_block());
  this->get_point(t[1], verts[1].data_block());
  this->get_point(t[2], verts[2].data_block());

  // Compute the separating axis directions.
  typedef pimpl::axis_type axis_type;
  vnl_double_3 edges[3];
  edges[0] = verts[1] - verts[0];
  edges[1] = verts[2] - verts[1];
  edges[2] = verts[0] - verts[2];
  vnl_double_3 normal = -vnl_cross_3d(edges[0], edges[2]);
  ta.axes[0] = axis_type(normal);
  ta.axes[1] = axis_type(ijk[0]);
  ta.axes[2] = axis_type(ijk[1]);
  ta.axes[3] = axis_type(ijk[2]);
  for (unsigned int j=0; j < 3; ++j)
  {
    for (unsigned int k=0; k < 3; ++k)
    {
      ta.axes[k*3 + j + 4] = axis_type(vnl_cross_3d(ijk[k], edges[j]));
    }
  }

  // Project the triangle onto each axis.
  for (unsigned int j=0; j < 13; ++j)
  {
    ta.axes[j].update(verts[0]);
    ta.axes[j].update(verts[1]);
    ta.axes[j].update(verts[2]);
  }

  double radius_squared;
  this->pimpl_->compute_minimum_ball(verts[0].data_block(),
                                     verts[1].data_block(),
                                     verts[2].data_block(),
                                     tc.data_block(),
                                     radius_squared);
  tr = std::sqrt(radius_squared);
}

//----------------------------------------------------------------------------
bool
rgtl_object_array_triangles_3d
::object_intersects_ray(int id,
                        double const origin[3],
                        double const direction[3],
                        double in_y[3], double* in_s) const
{
  // Get the triangle geometry.
  triangle_type const& t = this->triangles_[id];
  rgtl_oat3_geometry g(this, t.data_block());

  // First compute the intersection with the triangle plane.
  double const* n = g.get_face_normal();
  double denominator = pimpl::dot(direction, n);
  if (std::fabs(denominator) > 0)
  {
    double local_s;
    double local_y[3];
    double* y = in_y? in_y : local_y;
    double& s = in_s? *in_s : local_s;
    double const* v0 = g.get_vertex(0);
    double v[3] = {v0[0]-origin[0], v0[1]-origin[1], v0[2]-origin[2]};
    s = pimpl::dot(v, n) / denominator;

    // The ray is only one sided.
    if (s < 0)
    {
      return false;
    }

    // Compute the point of intersection on the plane.
    for (int a=0; a < 3; ++a)
    {
      y[a] = origin[a] + s*direction[a];
    }

    // Test if the point of intersection is on the inside side of
    // every edge.
    for (int i=0; i < 3; ++i)
    {
      double const* vi = g.get_vertex(i);
      double const* ni = g.get_edge_normal(i);
      double ui[3] = {y[0]-vi[0], y[1]-vi[1], y[2]-vi[2]};
      if (pimpl::dot(ui, ni) < 0)
      {
        return false;
      }
    }

    // The intersection is inside the triangle.
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------------------
void
rgtl_object_array_triangles_3d
::compute_normals()
{
  this->pimpl_->compute_normals(this);
}

//----------------------------------------------------------------------------
void
rgtl_object_array_triangles_3d
::get_closest_point_normal(int id, double n[3]) const
{
  assert(this->pimpl_->have_pseudonormals);

  // Use the angle-weighted pseudo-normal of the closest feature.
  pimpl::closest_feature_type const& feature =
    this->pimpl_->triangle_closest_features_[id];
  if (feature.kind == pimpl::closest_finder_type::feature_vertex)
  {
    // The query point is closest to a triangle vertex.
    int v = this->triangles_[id][feature.index];
    this->pimpl_->vertex_normals_[v].copy_out(n);
  }
  else if (feature.kind == pimpl::closest_finder_type::feature_edge)
  {
    // The query point is closest to a triangle edge.
    int e = this->pimpl_->triangle_edges_ids_[id][feature.index];
    this->pimpl_->edge_normals_[e].copy_out(n);
  }
  else // feature.kind == pimpl::closest_finder_type::feature_face
  {
    // The query point is closest to a triangle face.
    this->pimpl_->face_normals_[id].copy_out(n);
  }
}

//----------------------------------------------------------------------------
//: Compute the center, c0, of the smallest possible sphere touching the three points.
// This center will be in the plane of the triangle and in the bisecting planes
// of the edges of the triangle.
// \verbatim
//                  |
//                  |m02
//         x0 O-----o----O x2
//             \    |
//              \   O c0
//         m01___\_/
//              _/\.
//             /   \.
//                  O  x1
// \endverbatim
bool
rgtl_object_array_triangles_3d::pimpl
::compute_minimum_ball(double const x0[3],
                       double const x1[3],
                       double const x2[3],
                       double c0[3],
                       double& radius_squared)
{
  // Normals to two of the bisecting planes.
  double n01[3] = {x1[0]-x0[0], x1[1]-x0[1], x1[2]-x0[2]};
  double n02[3] = {x2[0]-x0[0], x2[1]-x0[1], x2[2]-x0[2]};

  // The normal to the plane of the triangle.
  double n[3];
  cross(n01, n02, n); // TODO: Check for numerical problems.

  // Points in the two bisecting planes.
  double m01[3] = {(x0[0]+x1[0])/2, (x0[1]+x1[1])/2, (x0[2]+x1[2])/2};
  double m02[3] = {(x0[0]+x2[0])/2, (x0[1]+x2[1])/2, (x0[2]+x2[2])/2};

  // Compute the intersection of the two bisecting planes and the
  // triangle's plane to find c0.  Solve the linear system
  //   ( n01^T )      ( n01^T*m01 )
  //   ( n02^T ) c0 = ( n02^T*m02 )
  //   ( n^T   )      ( n^T*x0    )
  vnl_double_3x3 A;
  A.set_row(0, n01);
  A.set_row(1, n02);
  A.set_row(2, n);
  double b[3] = {dot(n01, m01), dot(n02, m02), dot(n, x0)};
  vnl_svd<double> svd(A.as_ref());
  svd.solve(b, c0); // TODO: Check for numerical problems.

  // Return the radius of the smallest ball touching the points.
  radius_squared = (this->square(x0[0]-c0[0])+
                    this->square(x0[1]-c0[1])+
                    this->square(x0[2]-c0[2]));
  return true;
}

//----------------------------------------------------------------------------
bool
rgtl_object_array_triangles_3d::pimpl
::balls_intersect(unsigned int id,
                  double const center[3], double radius) const
{
  double const* tc = this->triangle_centers_[id].data_block();
  double v[3] = {tc[0]-center[0], tc[1]-center[1], tc[2]-center[2]};
  double m = std::sqrt(dot(v,v));
  double r = radius + this->triangle_radii_[id];
  return m <= r;
}

//----------------------------------------------------------------------------
void
rgtl_object_array_triangles_3d::pimpl
::compute_normals(rgtl_object_array_triangles_3d* self)
{
  // Number of vertices and triangles.
  int num_verts = self->derived::number_of_objects();
  int num_triangles = self->number_of_objects();

  // Initialize the vertex normals.
  this->vertex_normals_.resize(num_verts, vnl_double_3(0,0,0));

  // Allocate the face normals.
  this->face_normals_.resize(num_triangles);

  // Allocate the triangle edge links.
  this->triangle_edges_ids_.resize(num_triangles);

  // Reset the edge information.
  this->edge_normals_.clear();

  // Map from edge endpoint ids to edge index.
  typedef std::map<edge_key, int> edge_map_type;
  edge_map_type edge_map;

  // Compute normal information using every triangle.
  for (int index=0; index < num_triangles; ++index)
  {
    // Get the triangle point ids.
    int point_ids[3];
    self->get_triangle(index, point_ids);

    // Get the geometry of this triangle.
    rgtl_oat3_geometry g(self, point_ids);

    // Store the face normal now.
    this->face_normals_[index].copy_in(g.get_face_normal());
    this->face_normals_[index].normalize();

    // Contribute to each vertex and edge normal.
    for (int j=0; j < 3; ++j)
    {
      // Compute the angle incident to this vertex.
      double const* pne = g.get_edge_tangent(j);
      double const* ppe = g.get_edge_tangent((j+2)%3);
      double ne[3] = {pne[0], pne[1], pne[2]};
      double pe[3] = {-ppe[0], -ppe[1], -ppe[2]};
      normalize(pe);
      normalize(ne);
      double angle = std::acos(dot(pe, ne));

      // Contribute the triangle normal to this vertex normal weighted
      // by the incident angle.  We do not need to accumulate the
      // total weight separately because the final normal will be
      // scaled to unit length anyway.
      vnl_double_3 const& fn = this->face_normals_[index];
      vnl_double_3& wn = this->vertex_normals_[point_ids[j]];
      wn += angle * fn;

      // Contribute the triangle normal to this edge.
      edge_key ek(point_ids[j], point_ids[(j+1)%3]);
      edge_map_type::iterator emi = edge_map.find(ek);
      if (emi == edge_map.end())
      {
        int ei = static_cast<int>(this->edge_normals_.size());
        this->edge_normals_.push_back(fn);
        emi = edge_map.insert(edge_map_type::value_type(ek, ei)).first;
      }
      else
      {
        vnl_double_3& en = this->edge_normals_[emi->second];
        en += fn;
        en.normalize();
      }

      // Store the edge id in the triangle.
      this->triangle_edges_ids_[index][j] = emi->second;
    }
  }

  // Scale the computed normals to unit length.
  for (int i=0; i < num_verts; ++i)
  {
    this->vertex_normals_[i].normalize();
  }

  // We now have pseudo-normals.
  have_pseudonormals = true;
}

//----------------------------------------------------------------------------
template <class Serializer>
void rgtl_object_array_triangles_3d::serialize_save(Serializer& sr)
{
  // Store whether or not we have computed pseudonormals.
  sr << this->pimpl_->have_pseudonormals;
}

//----------------------------------------------------------------------------
template <class Serializer>
void rgtl_object_array_triangles_3d::serialize_load(Serializer& sr)
{
  // Load whether or not we have computed pseudonormals.
  bool have_pseudonormals;
  sr >> have_pseudonormals;

  // Allocate triangle intersection info.
  std::size_t n = this->triangles_.size();
  this->pimpl_->set_number_of_triangles(n);

  // Compute the pseudonormals now if they were already computed when
  // the instance was serialized.
  if (have_pseudonormals)
  {
    this->compute_normals();
  }
}

//----------------------------------------------------------------------------
template <class Serializer>
void rgtl_object_array_triangles_3d::serialize(Serializer& sr)
{
  // Serialize the points and triangles.
  sr & rgtl_serialize_base<derived>(*this);
  sr & this->triangles_;

  // Serialize minimal information for internal state.
  rgtl_serialize_split(sr, *this);
}

#define RGTL_OAT_SERIALIZE_INSTANTIATE(T) \
  template void rgtl_object_array_triangles_3d::serialize< T >(T&)
RGTL_OAT_SERIALIZE_INSTANTIATE(rgtl_serialize_istream);
RGTL_OAT_SERIALIZE_INSTANTIATE(rgtl_serialize_ostream);
