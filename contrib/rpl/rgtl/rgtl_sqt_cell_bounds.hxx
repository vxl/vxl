// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_sqt_cell_bounds_hxx
#define rgtl_sqt_cell_bounds_hxx

#include "rgtl_sqt_cell_bounds.h"

#include "rgtl_sqt_cell_location.h"

#include <vnl/vnl_math.h>

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_sqt_cell_bounds<D>
::rgtl_sqt_cell_bounds(rgtl_sqt_cell_location<D> const& cell)
{
  static derived root = root_bounds();
  this->derived::compute_bounds(root, cell);
}

//----------------------------------------------------------------------------
// All spherical quad-trees have the same root cell bounds on every
// face.  This is because the tree is defined over a parameter space.
template <unsigned int D>
typename rgtl_sqt_cell_bounds<D>::derived
rgtl_sqt_cell_bounds<D>::root_bounds()
{
  double origin[D-1];
  for(unsigned int i=0; i < D-1; ++i)
    {
    origin[i] = -vnl_math::pi/4;
    }
  double size = vnl_math::pi/2;
  return derived(origin, size);
};

//----------------------------------------------------------------------------
#define RGTL_SQT_CELL_BOUNDS_INSTANTIATE(D) \
  template class rgtl_sqt_cell_bounds< D >

#endif
