/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_sqt_cell_location_h
#define rgtl_sqt_cell_location_h

//:
// \file
// \brief Represent the logical index of a spherical quad-tree cell.
// \author Brad King
// \date February 2007

#include <iostream>
#include <iosfwd>
#include "rgtl_octree_cell_location.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Represent the logical index of a spherical quad-tree cell.
//
// A spherical quad-tree contains D*2 quad-trees, one for each face of
// the circumscribing cube.  The location of a cell can be uniquely
// described by the index of the face whose quad-tree contains the
// cell and the location of the cell within its quad-tree.
template <unsigned int D>
class rgtl_sqt_cell_location: public rgtl_octree_cell_location<D-1>
{
public:
  //: The type from which this class derives.
  typedef rgtl_octree_cell_location<D-1> derived;

  //: Type-safe index of a child.
  typedef typename derived::child_index_type child_index_type;

  //: Construct with the root quad-tree cell on a given face.
  rgtl_sqt_cell_location(unsigned int face);

  //: Get the location of a child.
  rgtl_sqt_cell_location get_child(child_index_type child_index) const;

  //: Get the index of the circumscribing cube face containing the cell.
  unsigned int face() const { return this->face_; }

protected:
  //: Constructor used internally.
  rgtl_sqt_cell_location(derived const& d, unsigned int face);

private:
  //: Store the index of the face containing the cell.
  unsigned int face_;
};

//: Define a total ordering to logical spherical quad-tree locations.
template <unsigned int D>
bool operator<(rgtl_sqt_cell_location<D> const& l,
               rgtl_sqt_cell_location<D> const& r);
template <unsigned int D>
bool operator==(rgtl_sqt_cell_location<D> const& l,
                rgtl_sqt_cell_location<D> const& r);

//: Print a logical cell location in a human-readable form.
template <unsigned int D>
std::ostream& operator<<(std::ostream& os,
                        rgtl_sqt_cell_location<D> const& cell);

#endif
