#ifndef rgtl_octree_data_fixed_h
#define rgtl_octree_data_fixed_h
//:
// \file
// \brief Represent an octree
// \author Brad King
// \date February 2007
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "rgtl_compact_tree_data_fixed.h"
#include "rgtl_serialize_access.h"
#include "rgtl_serialize_base.h"

template <unsigned int D> class rgtl_octree_cell_location;

template <unsigned int D,
          typename LeafDataType = void,
          typename NodeDataType = void>
class rgtl_octree_data_fixed:
  public rgtl_compact_tree_data_fixed<D, LeafDataType, NodeDataType>
{
 public:
  typedef rgtl_compact_tree_data_fixed<D, LeafDataType, NodeDataType> derived;
  typedef typename derived::cell_index_type cell_index_type;
  typedef typename derived::node_index_type node_index_type;
  typedef typename derived::child_index_type child_index_type;
  typedef typename derived::leaf_data_type leaf_data_type;
  typedef typename derived::node_data_type node_data_type;
  enum { dimension = derived::dimension };
  typedef rgtl_octree_cell_location<dimension> cell_location_type;
  leaf_data_type const* set_leaf_data(cell_index_type cell_index,
                                      leaf_data_type const* leaf_data);
  leaf_data_type const* set_leaf_data(cell_location_type const& cell,
                                      leaf_data_type const* leaf_data);
  node_data_type const* set_node_data(cell_index_type cell_index,
                                      node_data_type const* node_data);
  node_data_type const* set_node_data(cell_location_type const& cell,
                                      node_data_type const* node_data);
  leaf_data_type const* get_leaf_data(cell_index_type cell_index) const;
  leaf_data_type const* get_leaf_data(cell_location_type const& cell) const;
  node_data_type const* get_node_data(cell_index_type cell_index) const;
  node_data_type const* get_node_data(cell_location_type const& cell) const;

  //: Get the top-most search starting cell containing a given cell.
  //  Normally this is always the root cell.  However this octree
  //  representation supports storage of the siblings of the root
  //  cell.
  void get_start_cell(cell_location_type const& dest_cell,
                      cell_location_type& start_cell,
                      cell_index_type& start_index) const;

  cell_index_type location_to_index(cell_location_type const& cell) const;
  bool get_neighbor(cell_location_type const& cell,
                    unsigned int face,
                    cell_location_type& neighbor,
                    cell_index_type& nidx) const;
  void find_neighbor(cell_location_type const& dest_cell,
                     cell_location_type const& cur_cell,
                     cell_index_type cidx,
                     cell_location_type& out_cell,
                     cell_index_type& out_cidx) const;

  //: Get the index of the child of the a current cell containing a destination cell.
  child_index_type
  get_child_containing(cell_location_type const& dest_cell,
                       cell_location_type const& cur_cell) const;
 private:
  leaf_data_type const* set_leaf_data(cell_location_type const& cell_dest,
                                      leaf_data_type const* leaf_data,
                                      cell_location_type const& cell_current,
                                      cell_index_type cidx);
  node_data_type const* set_node_data(cell_location_type const& cell_dest,
                                      node_data_type const* node_data,
                                      cell_location_type const& cell_current,
                                      cell_index_type cidx);
  cell_index_type location_to_index(cell_location_type const& dest_cell,
                                    cell_location_type const& cur_cell,
                                    cell_index_type cidx) const;

  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr& rgtl_serialize_base<derived>(*this);
  }
};

#endif // rgtl_octree_data_fixed_h
