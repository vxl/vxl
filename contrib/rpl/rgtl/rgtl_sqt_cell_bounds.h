/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_sqt_cell_bounds_h
#define rgtl_sqt_cell_bounds_h

//:
// \file
// \brief Represent the bounding box of a cell on one face of a SQT.
// \author Brad King
// \date March 2007

#include "rgtl_octree_cell_bounds.h"

template <unsigned int D> class rgtl_sqt_cell_location;

//: Represent the bounding box of a cell on one face of a SQT.
//
// The spherical quad-tree root cell on each face always has the same
// bounds since the cells are defined on a parameter space.  This
// class encapsulates this knowledge and provides bounds computation
// without requiring the user to provide root cell bounds.
template <unsigned int D>
class rgtl_sqt_cell_bounds: public rgtl_octree_cell_bounds<D-1>
{
public:
  //: The type from which this class derives.
  typedef rgtl_octree_cell_bounds<D-1> derived;

  //: Construct the bounds for a logical cell location.
  rgtl_sqt_cell_bounds(rgtl_sqt_cell_location<D> const& cell);

private:
  static derived root_bounds();
};

#endif
