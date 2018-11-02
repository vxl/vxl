// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_octree_data_fixed_hxx
#define rgtl_octree_data_fixed_hxx

#include "rgtl_octree_data_fixed.h"
#include "rgtl_octree_cell_location.h"
#include "rgtl_compact_tree.hxx"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
typename rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>::leaf_data_type const*
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::set_leaf_data(cell_index_type cell_index,
                leaf_data_type const* leaf_data)
{
  return this->derived::set_leaf_data(cell_index, leaf_data);
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
typename rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>::leaf_data_type const*
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::set_leaf_data(cell_location_type const& cell,
                leaf_data_type const* leaf_data)
{
  return this->set_leaf_data(cell, leaf_data,
                             cell_location_type(), cell_index_type());
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
typename rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>::node_data_type const*
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::set_node_data(cell_index_type cell_index,
                node_data_type const* node_data)
{
  return this->derived::set_node_data(cell_index, node_data);
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
typename rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>::node_data_type const*
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::set_node_data(cell_location_type const& cell,
                node_data_type const* node_data)
{
  return this->set_node_data(cell, node_data,
                             cell_location_type(), cell_index_type());
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
typename rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>::leaf_data_type const*
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::get_leaf_data(cell_index_type cell_index) const
{
  return this->derived::get_leaf_data(cell_index);
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
typename rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>::leaf_data_type const*
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::get_leaf_data(cell_location_type const& cell) const
{
  if (cell_index_type c = this->location_to_index(cell))
  {
    if (!this->has_children(c))
    {
      return this->get_leaf_data(c);
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
typename rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>::node_data_type const*
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::get_node_data(cell_index_type cell_index) const
{
  return this->derived::get_node_data(cell_index);
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
typename rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>::node_data_type const*
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::get_node_data(cell_location_type const& cell) const
{
  if (cell_index_type c = this->location_to_index(cell))
  {
    if (this->has_children(c))
    {
      return this->get_node_data(c);
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
void
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::get_start_cell(cell_location_type const& dest_cell,
                 cell_location_type& start_cell,
                 cell_index_type& start_index) const
{
  // The compact tree representation supports indexing the siblings of
  // the root cell.  Choose the child of the root's parent that
  // contains the requested cell.
  child_index_type root_sibling(0);
  start_cell.level() = 0;
  for (unsigned int a=0; a < D; ++a)
  {
    unsigned int i = dest_cell.index(a) >> dest_cell.level();
    assert(i <= 1 && "cannot access outside parent of root");
    start_cell[a] = i;
    root_sibling |= i<<a;
  }
  start_index = cell_index_type(node_index_type(0), root_sibling);
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
typename rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>::cell_index_type
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::location_to_index(cell_location_type const& cell) const
{
  cell_location_type start_cell;
  cell_index_type start_index;
  this->get_start_cell(cell, start_cell, start_index);
  return this->location_to_index(cell, start_cell, start_index);
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
typename rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>::leaf_data_type const*
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::set_leaf_data(cell_location_type const& cell_dest,
                leaf_data_type const* leaf_data,
                cell_location_type const& cell_current,
                cell_index_type cidx)
{
  if (cell_current.level() == cell_dest.level())
  {
    // Set the data for this leaf.
    return this->set_leaf_data(cidx, leaf_data);
  }

  if (!this->has_children(cidx))
  {
    if (leaf_data)
    {
      // Divide the cell so a child can store the data.
      this->subdivide(cidx);
    }
    else
    {
      // The destination leaf does not exist in the tree and there are
      // no data to store.
      return 0;
    }
  }

  // The cell is divided.  Recurse into the child containing the
  // destination leaf.
  child_index_type child = this->get_child_containing(cell_dest, cell_current);
  if (leaf_data_type const* ld =
      this->set_leaf_data(cell_dest, leaf_data,
                          cell_current.get_child(child),
                          this->get_child(cidx, child)))
  {
    // Data are contained in the child.  We cannot collapse this cell.
    return ld;
  }

  // Collapse the cell if possible.
  if (this->can_collapse(cidx))
  {
    this->collapse(cidx);
  }

  // No data were stored.
  return 0;
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
typename rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>::node_data_type const*
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::set_node_data(cell_location_type const& cell_dest,
                node_data_type const* node_data,
                cell_location_type const& cell_current,
                cell_index_type cidx)
{
  // Make sure this is a node.
  if (!this->has_children(cidx))
  {
    if (node_data)
    {
      // Divide the cell so it or a child can store the data.
      this->subdivide(cidx);
    }
    else
    {
      // The destination node does not exist in the tree and there are
      // no data to store.
      return 0;
    }
  }

  if (cell_current.level() == cell_dest.level())
  {
    // Set the data for this node.
    if (node_data_type const* nd = this->set_node_data(cidx, node_data))
    {
      // Data were stored.  We cannot collapse this cell.
      return nd;
    }
  }
  else
  {
    // Recurse into the child containing the destination node.
    child_index_type child =
      this->get_child_containing(cell_dest, cell_current);
    if (node_data_type const* nd =
        this->set_node_data(cell_dest, node_data,
                            cell_current.get_child(child),
                            this->get_child(cidx, child)))
    {
      // Data are contained in the child.  We cannot collapse this cell.
      return nd;
    }
  }

  // Collapse the cell if possible.
  if (this->can_collapse(cidx))
  {
    this->collapse(cidx);
  }

  // No data were stored.
  return 0;
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
typename rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>::child_index_type
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::get_child_containing(cell_location_type const& dest_cell,
                       cell_location_type const& cur_cell) const
{
  int bit_index = (dest_cell.level() - cur_cell.level() - 1);
  child_index_type child;
  for (unsigned int a=0; a < D; ++a)
  {
    child |= (((dest_cell.index(a) >> bit_index) & 1) << a);
  }
  return child;
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
typename rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>::cell_index_type
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::location_to_index(cell_location_type const& dest_cell,
                    cell_location_type const& cur_cell,
                    cell_index_type cidx) const
{
  if (cur_cell.level() == dest_cell.level())
  {
    // We have reached the destination cell.  Return its index.
    return cidx;
  }
  else if (this->has_children(cidx))
  {
    // The cell is divided.  Recurse into the child containing the
    // destination cell.
    child_index_type child =
      this->get_child_containing(dest_cell, cur_cell);
    return this->location_to_index(dest_cell, cur_cell.get_child(child),
                                   this->get_child(cidx, child));
  }
  else
  {
    // The cell does not exist.
    return cell_index_type::invalid();
  }
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
bool
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::get_neighbor(cell_location_type const& cell,
               unsigned int face,
               cell_location_type& neighbor,
               cell_index_type& nidx) const
{
  // Check the direction in which to search for a neighbor.
  unsigned int axis = face >> 1;
  unsigned int side = face & 1;
  if (axis >= D)
  {
    return false;
  }

  // Compute the logical index of the desired neighbor and check its
  // bounds.
  cell_location_type goal = cell;
  if (side == 0)
  {
    // Move to the neighbor that is lower along this axis.
    if (goal.index(axis) == 0)
    {
      return false;
    }
    --goal.index(axis);
  }
  else
  {
    // Move to the neighbor that is higher along this axis.
    ++goal.index(axis);
    if (goal.index(axis) >= static_cast<unsigned int>(1<<goal.level()))
    {
      return false;
    }
  }

  // Search for the neighbor cell or the leaf containing it.
  this->find_neighbor(goal, cell_location_type(), cell_index_type(),
                      neighbor, nidx);
  return true;
}

//----------------------------------------------------------------------------
template <unsigned int D, class LeafDataType, class NodeDataType>
void
rgtl_octree_data_fixed<D, LeafDataType, NodeDataType>
::find_neighbor(cell_location_type const& dest_cell,
                cell_location_type const& cur_cell,
                cell_index_type cidx,
                cell_location_type& out_cell,
                cell_index_type& out_cidx) const
{
  // TODO: This could be guided with an initial cell index.  The
  // search would first compute the level of the deepest cell
  // containing both the initial cell and the goal.  It would then
  // walk up to the level and back down if this were less than half
  // way to the root cell.
  if (cur_cell.level() < dest_cell.level() && this->has_children(cidx))
  {
    // The cell is divided.  Recurse into the child containing the
    // destination cell.
    int bit_index = (dest_cell.level() - cur_cell.level() - 1);
    child_index_type child(0);
    for (unsigned int a=0; a < D; ++a)
    {
      child |= (((dest_cell.index(a) >> bit_index) & 1) << a);
    }
    this->find_neighbor(dest_cell, cur_cell.get_child(child),
                        this->get_child(cidx, child),
                        out_cell, out_cidx);
  }
  else
  {
    // We have found the desired neighbor cell.
    out_cell = cur_cell;
    out_cidx = cidx;
  }
}

#endif
