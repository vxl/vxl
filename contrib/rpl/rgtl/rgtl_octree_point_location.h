#ifndef rgtl_octree_point_location_h
#define rgtl_octree_point_location_h
//:
// \file
// \brief Represent the logical index of an octree corner point.
// \author Brad King
// \date March 2007
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <unsigned int D> class rgtl_octree_cell_location;

//: Represent the logical index of an octree corner point in D dimensions.
//
// Each subdivision level in an octree can be indexed as a regular
// grid of points of size 2^L+1 along each axis where L is the level
// of subdivision.  An octree corner point can be completely indexed
// by the level of subdivision and the regular grid index within that
// level.  The subdivision level index is a single integer.  The
// regular grid index within the subdivision level is a set of D
// integers.  Note that a point indexed at a given level has
// additional valid indices at higher levels of subdivision.
template <unsigned int D>
class rgtl_octree_point_location
{
 public:
  //: Type representing cell locations.
  typedef rgtl_octree_cell_location<D> cell_location_type;

  //: Constructor initializes to the origin of the root cell.
  rgtl_octree_point_location();

  //: Construct to a corner of a cell.
  rgtl_octree_point_location(cell_location_type const& cell,
                             unsigned int corner=0);

  //: Get the subdivision level in which the index is represented.
  //  The root cell is level 0.
  int level() const { return level_; }

  //: Get the regular grid index at the current subdivision level.
  unsigned int* index() { return index_; }
  unsigned int const* index() const { return index_; }
  unsigned int& index(unsigned int i) { return index_[i]; }
  unsigned int const& index(unsigned int i) const { return index_[i]; }
  unsigned int& operator[](unsigned int j) { return index_[j]; }
  unsigned int const& operator[](unsigned int j) const { return index_[j]; }

 private:
  //: The depth of the level of this point in the tree.
  //  Level zero is the root cell.  Negative levels are invalid indices.
  int level_;

  //: The index of the point in the uniform grid defined at this level.
  unsigned int index_[D];
};

//: Define a total ordering to logical octree locations.
template <unsigned int D>
bool operator<(rgtl_octree_point_location<D> const& l,
               rgtl_octree_point_location<D> const& r);

//: Print a logical point location in a human-readable form.
template <unsigned int D>
std::ostream& operator<<(std::ostream& os,
                        rgtl_octree_point_location<D> const& point);

#endif // rgtl_octree_point_location_h
