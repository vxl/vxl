/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_octree_dual_element_h
#define rgtl_octree_dual_element_h

//:
// \file
// \brief Octree dual grid element representation.
// \author Brad King
// \date March 2007

#include "rgtl_config.h"
#include "rgtl_meta.h"
#include "rgtl_static_assert.h"

#include "rgtl_octree_dual_location.h"
#include "rgtl_octree_dual_index.h"

//: Represent an octree dual mesh element location and index.
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
struct rgtl_octree_dual_element
{
  typedef rgtl_octree_dual_location<D, d> dual_location_type;
  typedef rgtl_octree_dual_index<D, d> dual_index_type;
  typedef typename dual_location_type::cell_location_type cell_location_type;
  typedef typename dual_index_type::cell_index_type cell_index_type;

  //: The number of primal grid cells needed (2^d).
  RGTL_STATIC_CONST(unsigned int, num_primal = 1<<d);

  //: Access the primal grid cell logical locations.
  cell_location_type& primal_location(unsigned int i)
    {
    return this->location_[i];
    }
  cell_location_type const& primal_location(unsigned int i) const
    {
    return this->location_[i];
    }

  //: Access the primal grid cell storage indices.
  cell_index_type& primal_index(unsigned int i)
    {
    return this->index_[i];
    }
  cell_index_type primal_index(unsigned int i) const
    {
    return this->index_[i];
    }

private:
  dual_location_type location_;
  dual_index_type index_;
};

//: Octree dual mesh element location and index with orientation.
//
// This is identical to rgtl_octree_dual_element except that the extra
// template argument 'm' provides information about the orientation of
// the dual element.  The value of 'm' has D bits of which d are set.
// The bits set indicate the axes spanned by the dual element.
template <unsigned int D, unsigned int d, unsigned int m>
struct rgtl_octree_dual_element_oriented: public rgtl_octree_dual_element<D,d>
{
private:
#if 0
  // For some reason MSVC instantiates this with m=0 for overload
  // resolution when passing to a function template even when the real
  // type is m!=0.  The instantiation is not selected and the proper
  // overload is selected anyway.  This check should be placed in all
  // function templates defined over m just to be sure.
  RGTL_STATIC_ASSERT(rgtl::meta::bits_c<m>::value == d);
#endif
};

#endif
