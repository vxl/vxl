#ifndef rgtl_octree_cell_location_h
#define rgtl_octree_cell_location_h
//:
// \file
// \brief Represent the logical index of an octree cell in D dimensions.
// \author Brad King
// \date February 2007
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <iosfwd>
#include "rgtl_compact_tree_index.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Represent the logical index of an octree cell in D dimensions.
//
// Each subdivision level in an octree can be indexed as a regular
// grid of size 2^L along each axis where L is the level of
// subdivision.  An octree cell can be completely indexed by the level
// of subdivision and the regular grid index within that level.  The
// subdivision level index is a single integer.  The regular grid
// index within the subdivision level is a set of D integers.
template <unsigned int D>
class rgtl_octree_cell_location
{
 public:
  //: Constructor initializes to root cell.
  rgtl_octree_cell_location();

  typedef rgtl_child_index_type child_index_type;

  //: Get the subdivision level of the cell.  The root cell is level 0.
  int& level() { return level_; }
  int level() const { return level_; }

  //: Get the regular grid index at the cell's subdivision level.
  unsigned int* index() { return index_; }
  unsigned int const* index() const { return index_; }
  unsigned int& index(unsigned int i) { return index_[i]; }
  unsigned int const& index(unsigned int i) const { return index_[i]; }
  unsigned int& operator[](unsigned int j) { return index_[j]; }
  unsigned int const& operator[](unsigned int j) const { return index_[j]; }

  //: Get the cell location of the parent cell.
  void get_parent(rgtl_octree_cell_location& parent) const;
  rgtl_octree_cell_location<D> get_parent() const;

  //: Get the cell location of a child cell.
  //  The child index must be in the range 0..(2^D-1).
  void get_child(child_index_type child_index,
                 rgtl_octree_cell_location& child) const;
  rgtl_octree_cell_location<D> get_child(child_index_type child_index) const;

 private:
  //: The depth of the level of this cell in the tree.
  //  Level zero is the root cell.  Negative levels are invalid indices.
  int level_;

  //: The index of the cell in the uniform grid defined at this level.
  unsigned int index_[D];
};

//: Define a total ordering to logical octree locations.
template <unsigned int D>
bool operator<(rgtl_octree_cell_location<D> const& l,
               rgtl_octree_cell_location<D> const& r);
template <unsigned int D>
bool operator==(rgtl_octree_cell_location<D> const& l,
                rgtl_octree_cell_location<D> const& r);
template <unsigned int D>
bool operator>(rgtl_octree_cell_location<D> const& l,
               rgtl_octree_cell_location<D> const& r);

//: Print a logical cell location in a human-readable form.
template <unsigned int D>
std::ostream& operator<<(std::ostream& os,
                        rgtl_octree_cell_location<D> const& cell);

#endif // rgtl_octree_cell_location_h
