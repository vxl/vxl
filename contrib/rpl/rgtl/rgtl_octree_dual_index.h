/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_octree_dual_index_h
#define rgtl_octree_dual_index_h

//:
// \file
// \brief Index of an octree dual mesh element.
// \author Brad King
// \date April 2007

#include "rgtl_compact_tree_index.h"

//: Represent an octree dual mesh element index.
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
class rgtl_octree_dual_index
{
public:
  //: The octree cell index type.
  typedef rgtl_compact_tree_index<D> cell_index_type;

  //: Access the primal grid cell indices.
  cell_index_type& operator[](unsigned int i)
    {
    return this->primal_[i];
    }
  cell_index_type operator[](unsigned int i) const
    {
    return this->primal_[i];
    }

private:
  // The octree index of each primal cell.
  cell_index_type primal_[(1<<d)];
};

#endif
