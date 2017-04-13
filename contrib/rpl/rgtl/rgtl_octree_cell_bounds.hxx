// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_octree_cell_bounds_hxx
#define rgtl_octree_cell_bounds_hxx

#include "rgtl_octree_cell_bounds.h"

#include "rgtl_octree_cell_location.h"

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_cell_bounds<D>::rgtl_octree_cell_bounds():
  origin_(0.0), size_(1)
{
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_cell_bounds<D>::rgtl_octree_cell_bounds(double const o[D],
                                                    double s):
  origin_(o), size_(s)
{
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_cell_bounds<D>::rgtl_octree_cell_bounds(point_type const& o,
                                                    double s):
  origin_(o), size_(s)
{
}

//----------------------------------------------------------------------------
template <unsigned int D>
void rgtl_octree_cell_bounds<D>::set_bounds(double const o[D], double s)
{
  this->origin_.copy_in(o);
  this->size_ = s;
}

//----------------------------------------------------------------------------
template <unsigned int D>
void rgtl_octree_cell_bounds<D>::set_bounds(point_type const& o, double s)
{
  this->origin_ = o;
  this->size_ = s;
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool rgtl_octree_cell_bounds<D>::contains(double const p[D]) const
{
  for(unsigned int a=0; a < D; ++a)
    {
    if(p[a] < this->origin_[a] ||
       p[a] > (this->origin_[a]+this->size_))
      {
      return false;
      }
    }
  return true;
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool rgtl_octree_cell_bounds<D>::contains(point_type const& p) const
{
  return this->contains(p.data_block());
}

//----------------------------------------------------------------------------
template <unsigned int D>
void rgtl_octree_cell_bounds<D>::compute_bounds(double const (&bds)[D][2],
                                                double factor)
{
  this->compute_bounds(&bds[0][0], factor);
}

//----------------------------------------------------------------------------
template <unsigned int D>
void rgtl_octree_cell_bounds<D>::compute_bounds(double const bds[D*2],
                                                double factor)
{
  // Find the maximum size along any axis.
  double max_size = 0;
  for(unsigned int i=0; i < D; ++i)
    {
    double sz = bds[2*i+1]-bds[2*i];
    if(sz > max_size)
      {
      max_size = sz;
      }
    }
  if(max_size <= 0)
    {
    max_size = 1;
    }

  // Construct a bounding cube around the entire input bounds.
  this->size_ = max_size*factor;
  for(unsigned int i=0; i < D; ++i)
    {
    this->origin_[i] = bds[2*i] - (this->size_-(bds[2*i+1]-bds[2*i]))/2;
    }
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_cell_bounds<D>
::compute_bounds(rgtl_octree_cell_bounds<D> const& root_bounds,
                 rgtl_octree_cell_location<D> const& cell)
{
  this->size_ = root_bounds.size() / (1<<cell.level());
  for(unsigned int i=0; i < D; ++i)
    {
    this->origin_[i] = (root_bounds.origin(i) + cell.index(i)*this->size_);
    }
}

#undef RGTL_OCTREE_CELL_BOUNDS_INSTANTIATE
#define RGTL_OCTREE_CELL_BOUNDS_INSTANTIATE( D ) \
  template class rgtl_octree_cell_bounds< D >

#endif
