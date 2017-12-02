/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_octree_dual_location_h
#define rgtl_octree_dual_location_h

//:
// \file
// \brief Logical location of an octree dual mesh element.
// \author Brad King
// \date April 2007

#include "rgtl_octree_cell_location.h"

//: Represent an octree dual mesh element logical location.
//
// The first template argument specifies the dimension of the octree.
// The second template argument specifies the dimension of the element.
// Element dimension 0 is a dual point specified by one primal grid cell.
// Element dimension D is a dual cell.
//
// An octree dual mesh element is specified by the primal grid cells
// corresponding to its vertices.  At least d+1 and at most 2^d unique
// primal grid cells are necessary.  Here we store all 2^d cells in
// lexicographic order.  Some cells may be repeated but there will be
// at least d+1 unique cells.
template <unsigned int D, unsigned int d>
class rgtl_octree_dual_location
{
public:
  //: The logical octree cell location type.
  typedef rgtl_octree_cell_location<D> cell_location_type;

  //: Access the primal grid cell locations.
  cell_location_type& operator[](unsigned int i)
    {
    return this->primal_[i];
    }
  cell_location_type const& operator[](unsigned int i) const
    {
    return this->primal_[i];
    }

private:
  // The logical octree location of each primal cell.
  cell_location_type primal_[(1<<d)];
};

#endif
