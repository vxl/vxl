// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_octree_cell_geometry_hxx
#define rgtl_octree_cell_geometry_hxx

#include <iostream>
#include <cmath>
#include "rgtl_octree_cell_geometry.h"

#include "rgtl_octree_cell_bounds.h"
#include "rgtl_sqt_space.hxx"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define RGTL_OCTREE_CELL_GEOMETRY_CONE_SPECIALIZE_3

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_cell_geometry<D>
::rgtl_octree_cell_geometry(): location_()
{
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_cell_geometry<D>
::rgtl_octree_cell_geometry(cell_location_type const& cell,
                            cell_bounds_type const& root_bounds):
  location_(cell)
{
  // Compute the bounds of this cell.
  cell_bounds_type cell_bounds;
  cell_bounds.compute_bounds(root_bounds, cell);

  // Compute the lower and upper bounds for each axis.
  for (unsigned int a=0; a < D; ++a)
  {
    this->lower_[a] = cell_bounds.origin(a);
    this->upper_[a] = this->lower_[a] + cell_bounds.size();
  }

  // Compute the cell bounding sphere.
  double half_size = cell_bounds.size()/2;
  for (unsigned int a=0; a < D; ++a)
  {
    this->center_[a] = this->lower_[a] + half_size;
  }
  this->radius_ = std::sqrt(D*half_size*half_size);

  // Compute the corner locations.
  this->compute_corners();
}

//----------------------------------------------------------------------------
template <unsigned int D>
typename rgtl_octree_cell_geometry<D>::cell_location_type const&
rgtl_octree_cell_geometry<D>
::location() const
{
  return this->location_;
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_cell_geometry<D>
::get_sphere(double center[D], double& radius) const
{
  for (unsigned int k=0; k < D; ++k)
  {
    center[k] = this->center_[k];
  }
  radius = this->radius_;
}

//----------------------------------------------------------------------------
template <unsigned int D>
double const* rgtl_octree_cell_geometry<D>::get_sphere_center() const
{
  return this->center_;
}

//----------------------------------------------------------------------------
template <unsigned int D>
double rgtl_octree_cell_geometry<D>::get_sphere_radius() const
{
  return this->radius_;
}

//----------------------------------------------------------------------------
template <unsigned int D>
double const* rgtl_octree_cell_geometry<D>::get_lower() const
{
  return this->lower_;
}

//----------------------------------------------------------------------------
template <unsigned int D>
double const* rgtl_octree_cell_geometry<D>::get_upper() const
{
  return this->upper_;
}

//----------------------------------------------------------------------------
template <unsigned int D>
double const* rgtl_octree_cell_geometry<D>::get_corner(unsigned int i) const
{
  return this->corners_[i];
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_cell_geometry<D>
::get_children(rgtl_octree_cell_geometry children[1<<D]) const
{
  for (unsigned int i=0; i < (1<<D); ++i)
  {
    // Compute the child location.
    children[i].location_ = this->location_.get_child(child_index_type(i));

    // Compute the child bounding sphere.
    for (unsigned int a=0; a < D; ++a)
    {
      if ((i>>a)&1)
      {
        children[i].center_[a] = (this->center_[a]+this->upper_[a])/2;
      }
      else
      {
        children[i].center_[a] = (this->lower_[a]+this->center_[a])/2;
      }
    }
    children[i].radius_ = this->radius_ / 2;

    // Compute the child bounding plane positions.
    for (unsigned int a=0; a < D; ++a)
    {
      if ((i>>a)&1)
      {
        children[i].lower_[a] = this->center_[a];
        children[i].upper_[a] = this->upper_[a];
      }
      else
      {
        children[i].lower_[a] = this->lower_[a];
        children[i].upper_[a] = this->center_[a];
      }
    }

    // Compute the child corners.
    children[i].compute_corners();
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
void rgtl_octree_cell_geometry<D>::compute_corners()
{
  for (unsigned int i=0; i < (1<<D); ++i)
  {
    double* corner = this->corners_[i];
    for (unsigned int a=0; a < D; ++a)
    {
      if ((i>>a)&1)
      {
        corner[a] = this->upper_[a];
      }
      else
      {
        corner[a] = this->lower_[a];
      }
    }
  }
}

//----------------------------------------------------------------------------
#define RGTL_OCTREE_CELL_GEOMETRY_INSTANTIATE(D) \
  template class rgtl_octree_cell_geometry< D >

#endif
