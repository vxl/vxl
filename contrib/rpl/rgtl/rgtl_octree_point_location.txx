// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_octree_point_location_txx
#define rgtl_octree_point_location_txx

#include "rgtl_octree_point_location.hxx"
#include "rgtl_octree_cell_location.hxx"

#include <vcl_iostream.h>

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_point_location<D>
::rgtl_octree_point_location():
  level_(0)
{
  // Initialize to the root cell origin.
  for(unsigned int i=0; i < D; ++i)
    {
    index_[i] = 0;
    }
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_point_location<D>
::rgtl_octree_point_location(cell_location_type const& cell,
                             unsigned int corner):
  level_(cell.level())
{
  for(unsigned int a=0; a < D; ++a)
    {
    index_[a] = cell.index(a) + ((corner >> a) & 1);
    }
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool operator<(rgtl_octree_point_location<D> const& l,
               rgtl_octree_point_location<D> const& r)
{
  // Compare indices on deepest of two levels.
  if(l.level() > r.level())
    {
    // Within a level order along each axis.
    unsigned int rshift = l.level() - r.level();
    for(unsigned int a=0; a < D; ++a)
      {
      unsigned int lindex = l.index(a);
      unsigned int rindex = (r.index(a) << rshift);
      if(lindex < rindex) { return true; }
      else if(lindex > rindex) { return false; }
      }
    }
  else if(r.level() > l.level())
    {
    // Within a level order along each axis.
    unsigned int lshift = r.level() - l.level();
    for(unsigned int a=0; a < D; ++a)
      {
      unsigned int lindex = (l.index(a) << lshift);
      unsigned int rindex = r.index(a);
      if(lindex < rindex) { return true; }
      else if(lindex > rindex) { return false; }
      }
    }
  else
    {
    // Within a level order along each axis.
    for(unsigned int a=0; a < D; ++a)
      {
      unsigned int lindex = l.index(a);
      unsigned int rindex = r.index(a);
      if(lindex < rindex) { return true; }
      else if(lindex > rindex) { return false; }
      }
    }

  return false;
}

//----------------------------------------------------------------------------
template <unsigned int D>
vcl_ostream& operator<<(vcl_ostream& os,
                        rgtl_octree_point_location<D> const& point)
{
  os << "[" << point.level() << ":" << point.index(0);
  for(unsigned int i=1; i < D; ++i)
    {
    os << "," << point.index(i);
    }
  os << "]";
  return os;
}

#undef RGTL_OCTREE_POINT_LOCATION_INSTANTIATE
#define RGTL_OCTREE_POINT_LOCATION_INSTANTIATE( D ) \
  template class rgtl_octree_point_location< D >; \
  template bool operator<(rgtl_octree_point_location< D > const& l, \
                          rgtl_octree_point_location< D > const& r); \
  template vcl_ostream& operator<<(vcl_ostream& os, \
                                 rgtl_octree_point_location< D > const& point)

#endif
