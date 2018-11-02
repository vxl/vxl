#include <iostream>
#include <limits>
#include <vector>
#include "rgtl_sqt_object_array_triangles_3d.h"
//:
// \file
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "rgtl_object_array_triangles_3d.h"
#include "rgtl_sqt_space.h"
#include "rgtl_sqt_cell_bounds.h"
#include "rgtl_sqt_cell_geometry.h"
#include "rgtl_sqt_cell_location.h"

#include <vnl/vnl_float_3.h>
#include <vnl/vnl_double_3.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//----------------------------------------------------------------------------
class rgtl_soat3d_polygon
{
 public:
  enum { max_verts = 7 };
  rgtl_soat3d_polygon(): num_verts(0) {}
  unsigned int num_verts;
  float verts[max_verts][3];
  void insert_vertex(vnl_float_3& v)
  {
    assert("too many vertices in polygon" && (num_verts < max_verts));
    v.copy_out(this->verts[this->num_verts++]);
  }
  void insert_vertex(float const v[3])
  {
    assert("too many vertices in polygon" && (num_verts < max_verts));
    float* nv = this->verts[this->num_verts++];
    nv[0] = v[0];
    nv[1] = v[1];
    nv[2] = v[2];
  }
  void insert_vertex(double const v[3])
  {
    assert("too many vertices in polygon" && (num_verts < max_verts));
    float* nv = this->verts[this->num_verts++];
    nv[0] = static_cast<float>(v[0]);
    nv[1] = static_cast<float>(v[1]);
    nv[2] = static_cast<float>(v[2]);
  }
  void split(double n[3], rgtl_soat3d_polygon out[2]) const;
};

//----------------------------------------------------------------------------
void rgtl_soat3d_polygon::split(double n[3], rgtl_soat3d_polygon out[2]) const
{
  // Stop if the input is invalid.
  if (this->num_verts < 3)
  {
    out[0].num_verts = 0;
    out[1].num_verts = 0;
    return;
  }

  // Get the position of each vertex along the plane normal.
  double dots[max_verts];
  double min_dot = std::numeric_limits<float>::infinity();
  double max_dot = -std::numeric_limits<float>::infinity();
  int start = 0;
  for (unsigned int i=0; i < this->num_verts; ++i)
  {
    float const* v = this->verts[i];
    dots[i] = n[0]*v[0] + n[1]*v[1] + n[2]*v[2]; // inline the dot product
    if (dots[i] < min_dot)
    {
      min_dot = dots[i];
      start = i;
    }
    if (dots[i] > max_dot)
    {
      max_dot = dots[i];
    }
  }

  // If all the vertices are on one side of the plane just return the
  // whole polygon on that side.
  if (min_dot < 0 && max_dot <= 0)
  {
    out[0] = *this;
    out[1].num_verts = 0;
    return;
  }
  if (max_dot > 0 && min_dot >= 0)
  {
    out[0].num_verts = 0;
    out[1] = *this;
    return;
  }

  // Traverse the polygon and create points on each side.
  bool done = false;
  bool first = true;
  int last_i = 0;
  int last_side = 0;
  int current_side = 0;
  out[0].num_verts = 0;
  out[1].num_verts = 0;
  for (int i=start; !done; (last_i = i, i = (i+1)%this->num_verts,
                            last_side = current_side, first = false))
  {
    // Update our current side.  If a vertex appears exactly on the
    // plane do not switch sides.
    if (dots[i] < 0)
    {
      current_side = 0;
    }
    else if (dots[i] > 0)
    {
      current_side = 1;
    }

    // If we switched sides add the intermediate vertex on both sides.
    if (current_side != last_side)
    {
      double wB = std::fabs(dots[last_i]);
      if (wB > 0)
      {
        // Intersect the line segment just traversed with the plane.
        // We know the intersection is between the endpoints.
        double wA = std::fabs(dots[i]);
        vnl_double_3 pA(this->verts[last_i][0],
                        this->verts[last_i][1],
                        this->verts[last_i][2]);
        vnl_double_3 pB(this->verts[i][0],
                        this->verts[i][1],
                        this->verts[i][2]);
        vnl_double_3 pNew = (wA*pA + wB*pB) / (wA+wB);
        out[0].insert_vertex(pNew.data_block());
        out[1].insert_vertex(pNew.data_block());
      }
      else
      {
        // The last point was exactly on the plane and was already
        // added on the other side.  Add the point on this side.
        out[current_side].insert_vertex(this->verts[last_i]);
      }
    }

    // Store the vertex on the current side.
    if (i != start || first)
    {
      out[current_side].insert_vertex(this->verts[i]);
    }
    else
    {
      done = true;
    }
  }
}

//----------------------------------------------------------------------------
template <unsigned int Face>
class rgtl_sqt_object_set_triangles_3d:
  public rgtl_sqt_object_set_face<3, Face>
{
 public:
  typedef rgtl_object_array_triangles_3d triangle_array_type;
  typedef rgtl_sqt_object_set_face<3, Face> derived;
  typedef typename derived::cell_location_type cell_location_type;
  typedef typename derived::cell_geometry_type cell_geometry_type;
  typedef typename derived::sqt_object_set_face_ptr sqt_object_set_face_ptr;
  typedef rgtl_sqt_cell_bounds<3> cell_bounds_type;

  // The spherical parameter space for this face.
  typedef rgtl_sqt_space<3, Face> space;

  // Construct an empty polygon set into which polygons will be inserted.
  rgtl_sqt_object_set_triangles_3d() {}

  // Construct a new polygon set initialized with the given triangles.
  rgtl_sqt_object_set_triangles_3d(double const origin[3],
                                   triangle_array_type const& triangles);

  // Implement the rgtl_sqt_object_set_face API for the polygon set.
  virtual int number_of_objects() const
  { return int(this->vertex_counts_.size()); }
  virtual int original_id(int id) const
  { return this->original_indices_[id]; }
  virtual void split(cell_geometry_type const& cell_geometry,
                     sqt_object_set_face_ptr children[1<<(3-1)]);
  virtual void get_depth_range(float const direction[3],
                               float& depth_min, float& depth_max) const;
  virtual void get_depth_range(double const direction[3],
                               double& depth_min, double& depth_max) const;

 private:
  // A compact representation for the polygons.
  std::vector<unsigned char> vertex_counts_;
  std::vector<vnl_float_3> vertex_points_;
  std::vector<int> original_indices_;
};

//----------------------------------------------------------------------------
template <unsigned int Face>
rgtl_sqt_object_set_triangles_3d<Face>::
rgtl_sqt_object_set_triangles_3d(double const origin[3],
                                 triangle_array_type const& triangles)
{
  // Get the root cell bounding plane normals.
  cell_location_type root(Face);
  cell_bounds_type root_bounds(root);
  vnl_double_3 normals[4];
  for (unsigned int i=0; i < 4; ++i)
  {
    double u = root_bounds.origin(i>>1);
    if (i&1)
    {
      u += root_bounds.size();
    }
    double n[3];
    space::isoplane_normal(i>>1, u, n);
    // Orient the normals to face the inside of the root wedge.
    if (i&1)
    {
      normals[i][0] = -n[0];
      normals[i][1] = -n[1];
      normals[i][2] = -n[2];
    }
    else
    {
      normals[i][0] = n[0];
      normals[i][1] = n[1];
      normals[i][2] = n[2];
    }
  }

  // Convert each input triangle to an initial polygon.
  for (int i=0; i < triangles.number_of_objects(); ++i)
  {
    // Get the current triangle.
    int ids[3];
    triangles.get_triangle(i, ids);

    // Create an initial triangular polygon.
    rgtl_soat3d_polygon p;
    p.num_verts = 3;
    for (unsigned int j=0; j < 3; ++j)
    {
      double pd[3];
      triangles.get_point(ids[j], pd);
      p.verts[j][0] = static_cast<float>(pd[0] - origin[0]);
      p.verts[j][1] = static_cast<float>(pd[1] - origin[1]);
      p.verts[j][2] = static_cast<float>(pd[2] - origin[2]);
    }

    // Clip the polygon against each bounding plane.
    for (unsigned int j = 0; j < 4; ++j)
    {
      rgtl_soat3d_polygon split[2];
      p.split(normals[j].data_block(), split);
      p = split[1];
    }

    // Store the polygon if anything is left.
    if (p.num_verts >= 3)
    {
      this->vertex_counts_.push_back(p.num_verts);
      for (unsigned int j=0; j < p.num_verts; ++j)
      {
        vnl_float_3 nv(p.verts[j][0],
                       p.verts[j][1],
                       p.verts[j][2]);
        this->vertex_points_.push_back(nv);
      }
      this->original_indices_.push_back(i);
    }
  }
}

//----------------------------------------------------------------------------
template <unsigned int Face>
void
rgtl_sqt_object_set_triangles_3d<Face>
::split(cell_geometry_type const& cell_geometry,
        sqt_object_set_face_ptr children[1<<(3-1)])
{
  // Get the cell center plane normals with which to split the polygons.
  double center_normals[2][3];
  cell_geometry.get_center_planes(center_normals);

  // Allocate an output polygon set for each child.
  std::unique_ptr<rgtl_sqt_object_set_triangles_3d> out[4];
  for (unsigned int i=0; i < 4; ++i)
  {
    // Create this polygon set.
    out[i].reset(new rgtl_sqt_object_set_triangles_3d);

    // Reserve space for the polygon representation.
    out[i]->vertex_counts_.reserve(this->vertex_counts_.size());
    out[i]->vertex_points_.reserve(this->vertex_points_.size());
    out[i]->original_indices_.reserve(this->original_indices_.size());
  }

  // Split each polygon separately and store it in the children.
  unsigned int vertex_base = 0;
  for (unsigned int i=0; i < vertex_counts_.size(); ++i)
  {
    // Get the current polygon.
    rgtl_soat3d_polygon p;
    p.num_verts = this->vertex_counts_[i];
    for (unsigned int j=0; j < p.num_verts; ++j)
    {
      this->vertex_points_[vertex_base + j].copy_out(p.verts[j]);
    }

    // Split the polygon with the cell center planes.
    rgtl_soat3d_polygon split_v[2];
    p.split(center_normals[1], split_v);
    rgtl_soat3d_polygon pout[4];
    split_v[0].split(center_normals[0], pout+0);
    split_v[1].split(center_normals[0], pout+2);

    // Store the remaining polygon pieces for each child.
    for (unsigned int j=0; j < 4; ++j)
    {
      if (pout[j].num_verts >= 3)
      {
        out[j]->vertex_counts_.push_back(pout[j].num_verts);
        for (unsigned int k=0; k < pout[j].num_verts; ++k)
        {
          vnl_float_3 nv(pout[j].verts[k][0],
                         pout[j].verts[k][1],
                         pout[j].verts[k][2]);
          out[j]->vertex_points_.push_back(nv);
        }
        out[j]->original_indices_.push_back(this->original_indices_[i]);
      }
    }

    // Move on to the next input polygon.
    vertex_base += this->vertex_counts_[i];
  }

  // Hand the output sets to the caller.
  for (unsigned int i=0; i < 4; ++i)
  {
    children[i].reset(out[i].release());
  }
}

//----------------------------------------------------------------------------
template <unsigned int Face>
void
rgtl_sqt_object_set_triangles_3d<Face>
::get_depth_range(float const direction[3],
                  float& depth_min, float& depth_max) const
{
  depth_min = std::numeric_limits<float>::infinity();
  depth_max = 0;
  for (unsigned int i=0; i < this->vertex_points_.size(); ++i)
  {
    float const* p = this->vertex_points_[i].data_block();
    float d = 0;
    for (unsigned int a=0; a < 3; ++a)
    {
      d += p[a]*direction[a];
    }
    if (d < depth_min)
    {
      depth_min = d;
    }
    if (d > depth_max)
    {
      depth_max = d;
    }
  }
}

//----------------------------------------------------------------------------
template <unsigned int Face>
void
rgtl_sqt_object_set_triangles_3d<Face>
::get_depth_range(double const direction[3],
                  double& depth_min, double& depth_max) const
{
  depth_min = std::numeric_limits<double>::infinity();
  depth_max = 0;
  for (unsigned int i=0; i < this->vertex_points_.size(); ++i)
  {
    float const* p = this->vertex_points_[i].data_block();
    double d = 0;
    for (unsigned int a=0; a < 3; ++a)
    {
      d += p[a]*direction[a];
    }
    if (d < depth_min)
    {
      depth_min = d;
    }
    if (d > depth_max)
    {
      depth_max = d;
    }
  }
}

//----------------------------------------------------------------------------
rgtl_sqt_object_array_triangles_3d::
rgtl_sqt_object_array_triangles_3d(object_array_type const& oa): original_(oa)
{
}

//----------------------------------------------------------------------------
rgtl_sqt_object_array_triangles_3d::sqt_object_set_ptr
rgtl_sqt_object_array_triangles_3d::new_set(double const origin[3],
                                            unsigned int face) const
{
  // Create a polygon set representation of the triangles for the
  // requested face relative to the given origin.
  sqt_object_set_ptr sos;
  switch (face)
  {
    case 0: sos.reset(new rgtl_sqt_object_set_triangles_3d<0>(origin, this->original_)); break;
    case 1: sos.reset(new rgtl_sqt_object_set_triangles_3d<1>(origin, this->original_)); break;
    case 2: sos.reset(new rgtl_sqt_object_set_triangles_3d<2>(origin, this->original_)); break;
    case 3: sos.reset(new rgtl_sqt_object_set_triangles_3d<3>(origin, this->original_)); break;
    case 4: sos.reset(new rgtl_sqt_object_set_triangles_3d<4>(origin, this->original_)); break;
    case 5: sos.reset(new rgtl_sqt_object_set_triangles_3d<5>(origin, this->original_)); break;
    default: assert(!"no such face"); break;
  }
  return sos;
}

//----------------------------------------------------------------------------
rgtl_sqt_object_array_triangles_3d::derived::object_array_type const&
rgtl_sqt_object_array_triangles_3d::original() const
{
  return this->original_;
}
