// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_object_array_points_hxx
#define rgtl_object_array_points_hxx

#include <iostream>
#include <limits>
#include "rgtl_object_array_points.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_object_array_points<D>::rgtl_object_array_points(int n): points_(n)
{
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_object_array_points<D>
::rgtl_object_array_points(std::vector<point_type> const& points):
  points_(points)
{
}

//----------------------------------------------------------------------------
template <unsigned int D>
void rgtl_object_array_points<D>::set_number_of_points(int n)
{
  this->points_.resize(n);
}

//----------------------------------------------------------------------------
template <unsigned int D>
int rgtl_object_array_points<D>::get_number_of_points() const
{
  return static_cast<int>(this->points_.size());
}

//----------------------------------------------------------------------------
template <unsigned int D>
void rgtl_object_array_points<D>::get_point(int id, double x[D]) const
{
  this->points_[id].copy_out(x);
}

//----------------------------------------------------------------------------
template <unsigned int D>
void rgtl_object_array_points<D>::set_point(int id, double const x[D])
{
  this->points_[id].copy_in(x);
}

//----------------------------------------------------------------------------
template <unsigned int D>
int rgtl_object_array_points<D>::add_point(double const x[D])
{
  point_type p(x);
  int id = this->number_of_objects();
  this->points_.push_back(p);
  return id;
}

//----------------------------------------------------------------------------
template <unsigned int D>
int rgtl_object_array_points<D>::number_of_objects() const
{
  return this->get_number_of_points();
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool
rgtl_object_array_points<D>
::object_intersects_object(int idA, int idB) const
{
  return this->points_[idA] == this->points_[idB];
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool
rgtl_object_array_points<D>
::object_intersects_box(int id,
                        double const [D],
                        double const,
                        double const lower[D],
                        double const upper[D],
                        double const [1<<D][D]) const
{
  point_type const& x = this->points_[id];
  for (unsigned int a=0; a < D; ++a)
  {
    // Use an asymmetric test so that points on the boundary between
    // adjacent boxes are given to only one box.
    if (x[a] < lower[a] || x[a] >= upper[a])
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool
rgtl_object_array_points<D>
::object_closest_point(int id,
                       double const[D],
                       double y[D],
                       double) const
{
  this->points_[id].copy_out(y);
  return true;
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool
rgtl_object_array_points<D>
::object_intersects_ray(int,
                        double const[D],
                        double const[D],
                        double [D],
                        double*) const
{
  return false;
}

//----------------------------------------------------------------------------
template <unsigned int D>
void rgtl_object_array_points<D>::compute_bounds(double bounds[D][2]) const
{
  // Initialize bounds to empty.
  for (unsigned int a=0; a < D; ++a)
  {
    bounds[a][0] = +std::numeric_limits<double>::max();
    bounds[a][1] = -std::numeric_limits<double>::max();
  }

  // Update the bounds for each point.
  int n = this->get_number_of_points();
  for (int i=0; i < n; ++i)
  {
    point_type const& p = this->points_[i];
    for (unsigned int a=0; a < D; ++a)
    {
      if (p[a] < bounds[a][0])
      {
        bounds[a][0] = p[a];
      }
      if (p[a] > bounds[a][1])
      {
        bounds[a][1] = p[a];
      }
    }
  }
}

#undef RGTL_OBJECT_ARRAY_POINTS_INSTANTIATE
#define RGTL_OBJECT_ARRAY_POINTS_INSTANTIATE( D ) \
  template class rgtl_object_array_points<D >

#endif
