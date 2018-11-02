// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_octree_cell_location_hxx
#define rgtl_octree_cell_location_hxx

#include <iostream>
#include "rgtl_octree_cell_location.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_cell_location<D>::rgtl_octree_cell_location(): level_(0)
{
  // Initialize to the root cell.
  for(unsigned int i=0; i < D; ++i)
    {
    index_[i] = 0;
    }
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_cell_location<D>
::get_parent(rgtl_octree_cell_location& parent) const
{
  // The parent is one level shallower.
  parent.level_ = level_ - 1;

  // The least-significant-bit in the index for each axis is dropped.
  for(unsigned int j=0; j < D; ++j)
    {
    parent.index_[j] = index_[j] >> 1;
    }
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_cell_location<D>
rgtl_octree_cell_location<D>::get_parent() const
{
  rgtl_octree_cell_location parent;
  get_parent(parent);
  return parent;
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_cell_location<D>::get_child(child_index_type child_index,
                                        rgtl_octree_cell_location& child) const
{
  // The child is one level deeper.
  child.level_ = level_ + 1;

  // The least-significant-bit in the index for each axis comes from
  // the child index.  The rest come from the parent location.
  for(unsigned int j=0; j < D; ++j)
    {
    child.index_[j] = (index_[j] << 1) | ((child_index >> j) & 1);
    }
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_cell_location<D>
rgtl_octree_cell_location<D>::get_child(child_index_type child_index) const
{
  rgtl_octree_cell_location child;
  get_child(child_index, child);
  return child;
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool operator<(rgtl_octree_cell_location<D> const& l,
               rgtl_octree_cell_location<D> const& r)
{
  // Order by level first.
  if(l.level() < r.level()) { return true; }
  else if(l.level() > r.level()) { return false; }

  // Within a level order along each axis.
  for(unsigned int i=0; i < D; ++i)
    {
    if(l[i] < r[i]) { return true; }
    else if(l[i] > r[i]) { return false; }
    }

  return false;
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool operator>(rgtl_octree_cell_location<D> const& l,
               rgtl_octree_cell_location<D> const& r)
{
  // Order by level first.
  if(l.level() > r.level()) { return true; }
  else if(l.level() < r.level()) { return false; }

  // Within a level order along each axis.
  for(unsigned int i=0; i < D; ++i)
    {
    if(l[i] > r[i]) { return true; }
    else if(l[i] < r[i]) { return false; }
    }

  return false;
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool operator==(rgtl_octree_cell_location<D> const& l,
                rgtl_octree_cell_location<D> const& r)
{
  if(l.level() != r.level()) { return false; }
  for(unsigned int i=0; i < D; ++i)
    {
    if(l[i] != r[i]) { return false; }
    }
  return true;
}

//----------------------------------------------------------------------------
template <unsigned int D>
std::ostream& operator<<(std::ostream& os,
                        rgtl_octree_cell_location<D> const& cell)
{
  os << "[" << cell.level() << ":" << cell[0];
  for(unsigned int i=1; i < D; ++i)
    {
    os << "," << cell[i];
    }
  os << "]";
  return os;
}

#undef RGTL_OCTREE_CELL_LOCATION_INSTANTIATE
#define RGTL_OCTREE_CELL_LOCATION_INSTANTIATE( D ) \
  template class rgtl_octree_cell_location< D >; \
  template bool operator<(rgtl_octree_cell_location< D > const& l, \
                          rgtl_octree_cell_location< D > const& r); \
  template bool operator==(rgtl_octree_cell_location< D > const& l, \
                           rgtl_octree_cell_location< D > const& r); \
  template std::ostream& operator<<(std::ostream& os, \
                                   rgtl_octree_cell_location< D > const& cell)

#endif
