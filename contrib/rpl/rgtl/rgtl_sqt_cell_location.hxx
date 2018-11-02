// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_sqt_cell_location_hxx
#define rgtl_sqt_cell_location_hxx

#include <iostream>
#include "rgtl_sqt_cell_location.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_sqt_cell_location<D>
::rgtl_sqt_cell_location(unsigned int face): derived(), face_(face)
{
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_sqt_cell_location<D>
::rgtl_sqt_cell_location(derived const& d, unsigned int face):
  derived(d), face_(face)
{
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_sqt_cell_location<D>
rgtl_sqt_cell_location<D>::get_child(child_index_type child_index) const
{
  return rgtl_sqt_cell_location(this->derived::get_child(child_index),
                                this->face_);
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool operator<(rgtl_sqt_cell_location<D> const& l,
               rgtl_sqt_cell_location<D> const& r)
{
  typedef typename rgtl_sqt_cell_location<D>::derived const& super;

  // Order by face first.
  if(l.face() < r.face()) { return true; }
  else if(l.face() > r.face()) { return false; }

  // Use the cell ordering within a face.
  return static_cast<super>(l) < static_cast<super>(r);
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool operator==(rgtl_sqt_cell_location<D> const& l,
                rgtl_sqt_cell_location<D> const& r)
{
  typedef typename rgtl_sqt_cell_location<D>::derived const& super;
  if(l.face() != r.face()) { return false; }
  return static_cast<super>(l) == static_cast<super>(r);
}

//----------------------------------------------------------------------------
template <unsigned int D>
std::ostream& operator<<(std::ostream& os, rgtl_sqt_cell_location<D> const& cell)
{
  // Decompose the face index into axis and side.
  unsigned int axis = cell.face()>>1;
  unsigned int side = cell.face()&1;

  // Print the face axis.
  switch(axis)
    {
    case 0: os << "X"; break;
    case 1: os << "Y"; break;
    case 2: os << "Z"; break;
    default: os << "A" << axis; break;
    }

  // Print the face side.
  os << (side? "+" : "-");

  // Print the quad-tree cell location.
  typedef typename rgtl_sqt_cell_location<D>::derived const& super;
  return os << static_cast<super>(cell);
}

//----------------------------------------------------------------------------
#define RGTL_SQT_CELL_LOCATION_INSTANTIATE(D) \
  template class rgtl_sqt_cell_location< D >; \
  template bool operator<(rgtl_sqt_cell_location< D > const&, \
                          rgtl_sqt_cell_location< D > const&); \
  template bool operator==(rgtl_sqt_cell_location< D > const&, \
                           rgtl_sqt_cell_location< D > const&); \
  template std::ostream& operator<<(std::ostream&, \
                                   rgtl_sqt_cell_location< D > const&)

#endif
