// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_sqt_cell_geometry_hxx
#define rgtl_sqt_cell_geometry_hxx

#include <iostream>
#include <cmath>
#include "rgtl_sqt_cell_geometry.h"

#include "rgtl_sqt_cell_bounds.h"
#include "rgtl_sqt_space.hxx"

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_matrix_fixed.hxx>
#include <vnl/vnl_vector_fixed.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#define RGTL_SQT_CELL_GEOMETRY_CONE_SPECIALIZE_3

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
rgtl_sqt_cell_geometry<D, Face>
::rgtl_sqt_cell_geometry(): location_(Face)
{
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
rgtl_sqt_cell_geometry<D, Face>
::rgtl_sqt_cell_geometry(cell_location_type const& cell): location_(cell)
{
  // Get the cell bounds.
  rgtl_sqt_cell_bounds<D> bounds(cell);

  // Compute the bounding plane normals.
  for (unsigned int j=0; j < D-1; ++j)
  {
    double u0 = bounds.origin(j);
    double u1 = u0 + bounds.size();
    space::isoplane_normal(j, u0, this->lower_[j]);
    space::isoplane_normal(j, u1, this->upper_[j]);
  }

  // Compute the edge ray directions.
  // Precompute the lower and upper components for each axis.
  double dLower[D-1];
  double dUpper[D-1];
  for (unsigned int j=0; j < D-1; ++j)
  {
    double uLower = bounds.origin(j);
    double uUpper = uLower + bounds.size();
    dLower[j] = std::tan(uLower);
    dUpper[j] = std::tan(uUpper);
  }

  // Now distribute the components into each edge.
  for (unsigned int i=0; i < (1<<(D-1)); ++i)
  {
    for (unsigned int a=0; a < space::face_axis; ++a)
    {
      unsigned int j = a;
      if ((i>>j)&1)
      {
        this->edges_[i][a] = dUpper[j];
      }
      else
      {
        this->edges_[i][a] = dLower[j];
      }
    }
    this->edges_[i][space::face_axis] = (space::face_side? +1:-1);
    for (unsigned int a=space::face_axis+1; a < D; ++a)
    {
      unsigned int j = a-1;
      if ((i>>j)&1)
      {
        this->edges_[i][a] = dUpper[j];
      }
      else
      {
        this->edges_[i][a] = dLower[j];
      }
    }
  }

  // Compute the bounding cone axis and opening angle.
  this->compute_cone();
}

//----------------------------------------------------------------------------
template <unsigned int D>
struct rgtl_sqt_cell_geometry_cone
{
  // TODO: This template can be specialized for 2 dimensions to speed
  // up computation by skipping the SVD.
  static void compute(double edges[1<<(D-1)][D], unsigned int ei0,
                      double axis[D], double& angle)
  {
    // The edge closest to the face axis direction is always contained
    // in the bounding cone surface.
    vnl_vector_fixed<double, D> e0;
    e0.copy_in(edges[ei0]);
    e0.normalize();

    // The other edges contained in the bounding cone surface are the
    // D-1 neighbors along each other axis.  We treat the edge
    // directions as points on the unit sphere.  The normal to the
    // plane containing these points is the cone axis.  The plane cuts
    // the unit sphere and the cone on the same circle.
    vnl_matrix_fixed<double, D, D> m;
    for (unsigned int j=0; j < D-1; ++j)
    {
      unsigned int ei1 = ei0 ^ (1<<j);
      vnl_vector_fixed<double, D> e1;
      e1.copy_in(edges[ei1]);
      e1.normalize();
      m.set_column(j, e1-e0);
    }
    m.set_column(D-1, 0.0);

    // The column space of the matrix provides a basis for a D-1
    // dimensional subspace embedded in D dimensions.  The
    // left-nullspace of the matrix is described by a single vector
    // and is the desired plane normal and cone axis.
    vnl_svd<double> svd(m.as_ref());
    vnl_vector<double> nv = svd.left_nullvector();
    //nv.normalize(); // should already be normalized.

    // Orient the normal ray to point in the same direction as the
    // edges.
    double dot = dot_product(e0, nv);
    if (dot < 0)
    {
      nv = -nv;
      dot = -dot;
    }

    // Return the axis and opening angle.
    nv.copy_out(axis);
    angle = std::acos(dot);
  }
};

#ifdef RGTL_SQT_CELL_GEOMETRY_CONE_SPECIALIZE_3
//----------------------------------------------------------------------------
// Specialize cone axis computation for 3 dimensions to speed up
// computation by using a cross-product instead of a full SVD.
template <>
struct rgtl_sqt_cell_geometry_cone<3>
{
  static void compute(double edges[1<<(3-1)][3], unsigned int ei0,
                      double axis[3], double& angle)
  {
    // The edge closest to the face axis direction is always contained
    // in the bounding cone surface.
    double e0[3] = {edges[ei0][0], edges[ei0][1], edges[ei0][2]};
    normalize(e0);

    // The other edges contained in the bounding cone surface are the
    // D-1 neighbors along each other axis.  We treat the edge
    // directions as points on the unit sphere.  The normal to the
    // plane containing these points is the cone axis.  The plane cuts
    // the unit sphere and the cone on the same circle.
    unsigned int ei1 = ei0 ^ (1<<0);
    unsigned int ei2 = ei0 ^ (1<<1);
    double e1[3] = {edges[ei1][0], edges[ei1][1], edges[ei1][2]};
    double e2[3] = {edges[ei2][0], edges[ei2][1], edges[ei2][2]};
    normalize(e1);
    normalize(e2);
    double a[3] = {e1[0]-e0[0], e1[1]-e0[1], e1[2]-e0[2]};
    double b[3] = {e2[0]-e0[0], e2[1]-e0[1], e2[2]-e0[2]};

    // Compute the cone axis direction.
    cross(a, b, axis);
    normalize(axis);

    // Orient the normal ray to point in the same direction as the edges.
    double d = dot(e0, axis);
    if (d < 0)
    {
      axis[0] = -axis[0];
      axis[1] = -axis[1];
      axis[2] = -axis[2];
      d = -d;
    }

    // Compute the cone opening angle.
    angle = std::acos(d);
  }

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
};
#endif

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_cell_geometry<D, Face>
::compute_cone()
{
  // The bounding cone is determined by the edge direction closest
  // to the face axis direction and the D-1 neighbors along each
  // other axis.
  unsigned int ei0 = 0;
  if (this->location_.level() >= 1)
    {
    for (unsigned int j=0; j < D-1; ++j)
      {
      if (!(this->location_.index(j) >> (this->location_.level()-1)))
        {
        ei0 |= (1<<j);
        }
      }
    }

  // Dispatch the actual computation by dimension.
  rgtl_sqt_cell_geometry_cone<D>::compute(this->edges_, ei0,
                                          this->axis_, this->angle_);
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
typename rgtl_sqt_cell_geometry<D, Face>::cell_location_type const&
rgtl_sqt_cell_geometry<D, Face>
::location() const
{
  return this->location_;
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_cell_geometry<D, Face>
::get_cone(float axis[D], float& angle) const
{
  for (unsigned int k=0; k < D; ++k)
    {
    axis[k] = static_cast<float>(this->axis_[k]);
    }
  angle = static_cast<float>(this->angle_);
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_cell_geometry<D, Face>
::get_cone(double axis[D], double& angle) const
{
  for (unsigned int k=0; k < D; ++k)
    {
    axis[k] = this->axis_[k];
    }
  angle = this->angle_;
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
double const*
rgtl_sqt_cell_geometry<D, Face>
::get_cone_axis() const
{
  return this->axis_;
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
double
rgtl_sqt_cell_geometry<D, Face>
::get_cone_angle() const
{
  return this->angle_;
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_cell_geometry<D, Face>
::get_wedge_edge(unsigned int edge_index, double edge[D]) const
{
  double const* e = this->get_wedge_edge(edge_index);
  for (unsigned int a=0; a < D; ++a)
    {
    edge[a] = e[a];
    }
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
double const*
rgtl_sqt_cell_geometry<D, Face>
::get_wedge_edge(unsigned int edge_index) const
{
  assert(edge_index < (1<<(D-1)));
  return this->edges_[edge_index];
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_cell_geometry<D, Face>
::get_center_planes(double center_normals[D-1][D]) const
{
  // The center planes are located at the origin of the last child.
  cell_location_type upper_child =
    this->location_.get_child(child_index_type((1<<(D-1))-1));
  rgtl_sqt_cell_bounds<D> upper(upper_child);
  for (unsigned int j=0; j < D-1; ++j)
    {
    space::isoplane_normal(j, upper.origin(j), center_normals[j]);
    }
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_cell_geometry<D, Face>
::get_children(rgtl_sqt_cell_geometry children[(1<<(D-1))]) const
{
  double center_normals[D-1][D];
  this->get_center_planes(center_normals);
  this->get_children(children, center_normals);
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_cell_geometry<D, Face>
::get_children(rgtl_sqt_cell_geometry children[(1<<(D-1))],
               double const center_normals[D-1][D]) const
{
  for (unsigned int i=0; i < (1<<(D-1)); ++i)
    {
    // Compute the child location.
    children[i].location_ = this->location_.get_child(child_index_type(i));

    // The child lower and upper bounding planes come from our bounding
    // planes and the center plane normals.
    for (unsigned int j=0; j < D-1; ++j)
      {
      if ((i>>j)&1)
        {
        // This is an upper child along axis j.  The lower bounding
        // plane comes from the center and the upper from the parent.
        for (unsigned int k=0; k < D; ++k)
          {
          children[i].lower_[j][k] = center_normals[j][k];
          children[i].upper_[j][k] = this->upper_[j][k];
          }
        }
      else
        {
        // This is a lower child along axis j.  The lower bounding
        // plane comes from the parent and the upper from the center.
        for (unsigned int k=0; k < D; ++k)
          {
          children[i].lower_[j][k] = this->lower_[j][k];
          children[i].upper_[j][k] = center_normals[j][k];
          }
        }
      }
    }

  // Compute the wedge edge ray directions for each child.  There are
  // a total of 3^(D-1) directions to be stored, and 2^(D-1) of them
  // come from the parent edge ray directions.  We first precompute
  // the direction components along each axis.
  cell_location_type upper_child =
    this->location_.get_child(child_index_type((1<<(D-1))-1));
  rgtl_sqt_cell_bounds<D> upper(upper_child);
  double dLower[D-1];
  double dMiddle[D-1];
  double dUpper[D-1];
  for (unsigned int a=0; a < space::face_axis; ++a)
    {
    unsigned int j = a;
    dLower[j] = this->edges_[0][a];
    dMiddle[j] = std::tan(upper.origin(j));
    dUpper[j] = this->edges_[(1<<(D-1))-1][a];
    }
  for (unsigned int a=space::face_axis+1; a < D; ++a)
    {
    unsigned int j = a-1;
    dLower[j] = this->edges_[0][a];
    dMiddle[j] = std::tan(upper.origin(j));
    dUpper[j] = this->edges_[(1<<(D-1))-1][a];
    }

  // Distribute the components into all edges of each child.
  for (unsigned int ci=0; ci < (1<<(D-1)); ++ci)
    {
    // Distribute the components into each edge.
    for (unsigned int i=0; i < (1<<(D-1)); ++i)
      {
      double* edge = children[ci].edges_[i];
      for (unsigned int a=0; a < space::face_axis; ++a)
        {
        unsigned int j = a;
        if ((ci>>j)&1)
          {
          // This is an upper cell for axis j.
          // Edge components are middle..upper.
          if ((i>>j)&1)
            {
            edge[a] = dUpper[j];
            }
          else
            {
            edge[a] = dMiddle[j];
            }
          }
        else
          {
          // This is a lower cell for axis j.
          // Edge components are lower..middle.
          if ((i>>j)&1)
            {
            edge[a] = dMiddle[j];
            }
          else
            {
            edge[a] = dLower[j];
            }
          }
        }
      edge[space::face_axis] = (space::face_side? +1:-1);
      for (unsigned int a=space::face_axis+1; a < D; ++a)
        {
        unsigned int j = a-1;
        if ((ci>>j)&1)
          {
          // This is an upper cell for axis j.
          // Edge components are middle..upper.
          if ((i>>j)&1)
            {
            edge[a] = dUpper[j];
            }
          else
            {
            edge[a] = dMiddle[j];
            }
          }
        else
          {
          // This is a lower cell for axis j.
          // Edge components are lower..middle.
          if ((i>>j)&1)
            {
            edge[a] = dMiddle[j];
            }
          else
            {
            edge[a] = dLower[j];
            }
          }
        }
      }
    }

  // Compute the bounding cone axis and opening angle for each child.
  for (unsigned int i=0; i < (1<<(D-1)); ++i)
    {
    children[i].compute_cone();
    }
}

//----------------------------------------------------------------------------
#define RGTL_SQT_CELL_GEOMETRY_INSTANTIATE(D, Face) \
  template class rgtl_sqt_cell_geometry< D , Face >

#endif
