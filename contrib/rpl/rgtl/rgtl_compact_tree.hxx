// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_compact_tree_hxx
#define rgtl_compact_tree_hxx

#include "rgtl_compact_tree.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
rgtl_compact_tree<D, Policy1, Policy2>
::rgtl_compact_tree():
  node_owners_(1, cell_index_type::invalid()),
  child_links_(1),
  leaf_data_owners_(0)
{
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
bool
rgtl_compact_tree<D, Policy1, Policy2>
::has_children(cell_index_type c) const
{
  link const& l = child_link(c);
  return !l && l.index();
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
typename rgtl_compact_tree<D, Policy1, Policy2>::cell_index_type
rgtl_compact_tree<D, Policy1, Policy2>
::get_child(cell_index_type c, child_index_type child_index) const
{
  // The given cell index consists of the node array index of its
  // parent and the corresponding child number.  The child link entry
  // in the parent for the cell contains the node array index of the
  // cell itself.  Combined with the desired child number this gives
  // the cell index of the child.
  assert(has_children(c));
  return cell_index_type(node_index_type(child_link(c).index()),
                         child_index);
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
typename rgtl_compact_tree<D, Policy1, Policy2>::cell_index_type
rgtl_compact_tree<D, Policy1, Policy2>
::get_parent(cell_index_type c) const
{
  // The owner entry of a node is the index of itself, consisting of
  // the node array index of its parent and the child number
  // corresponding to the node.  Therefore the owner entry of our
  // parent is its own cell index.
  return node_owner(c.parent());
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
typename rgtl_compact_tree<D, Policy1, Policy2>::link
rgtl_compact_tree<D, Policy1, Policy2>
::leaf_data_get(cell_index_type c) const
{
  assert(!has_children(c));
  return leaf_data_link(c);
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
typename rgtl_compact_tree<D, Policy1, Policy2>::link
rgtl_compact_tree<D, Policy1, Policy2>
::leaf_data_insert(cell_index_type c)
{
  assert(!has_children(c));
  if (link& cl = child_link(c))
    {
    // The leaf data entry already exists.
    return cl;
    }
  else
    {
    // Create a link to the new data location.
    link l(leaf_data_count(), true);

    // Tell the owner of the new data about its location.
    leaf_data_link(c) = l;

    // Tell the new data about its owner.
    leaf_data_owners_.push_back(c);

    // Create the new leaf data value entry according to the leaf
    // data value policy.
    this->leaf_data_value_create();

    return l;
    }
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
void
rgtl_compact_tree<D, Policy1, Policy2>
::leaf_data_erase(cell_index_type c)
{
  assert(!has_children(c));

  // Erase the leaf data entry if it exists.  If it is not the last
  // entry move the last entry into its place and then erase the
  // last entry.
  if (link& cl = child_link(c))
    {
    // Get the index of the leaf data entry.
    leaf_data_index_type ldi(cl.index());

    // The original owner of leaf data at this index no longer owns data.
    leaf_data_link(leaf_data_owner(ldi)) = link(0, false);

    // Move the last data entry to fill in the hole.
    if (ldi < leaf_data_count()-1)
      {
      // Tell the new location of the moved data about its owner.
      leaf_data_index_type last(leaf_data_count()-1);
      leaf_data_owner(ldi) = leaf_data_owner(last);

      // Tell the owner of the moved data about its new location.
      leaf_data_link(leaf_data_owner(ldi)) = link(ldi, true);
      }

    // Deallocate the owner portion of the unused leaf data entry.
    leaf_data_owners_.erase(leaf_data_owners_.end()-1,
                            leaf_data_owners_.end());

    // Remove the old leaf data value entry according to the leaf
    // data value policy.
    this->leaf_data_value_destroy(ldi);
    }
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
typename rgtl_compact_tree<D, Policy1, Policy2>::link
rgtl_compact_tree<D, Policy1, Policy2>
::node_data_get(cell_index_type c) const
{
  assert(has_children(c));

  // Invoke the internal implementation of this method according to
  // the node data policy given.
  return this->node_data_get_impl(node_index_type(child_link(c).index()));
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
typename rgtl_compact_tree<D, Policy1, Policy2>::link
rgtl_compact_tree<D, Policy1, Policy2>
::node_data_insert(cell_index_type c)
{
  assert(has_children(c));

  // Invoke the internal implementation of this method according to
  // the node data policy given.
  return this->node_data_insert_impl(node_index_type(child_link(c).index()));
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
void
rgtl_compact_tree<D, Policy1, Policy2>
::node_data_erase(cell_index_type c)
{
  assert(has_children(c));

  // Invoke the internal implementation of this method according to
  // the node data policy given.
  this->node_data_erase_impl(node_index_type(child_link(c).index()));
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
void
rgtl_compact_tree<D, Policy1, Policy2>
::subdivide(cell_index_type c)
{
  assert(can_subdivide(c));

  // Link the new parent to its children.
  child_link(c) = link(node_count(), false);

  // link the new children to their parent.
  node_owners_.push_back(c);

  // Create the entry for the children.
  child_links_.push_back(child_links_entry());

  // Create the entry for the node data.
  this->node_data_link_create();
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
void
rgtl_compact_tree<D, Policy1, Policy2>
::collapse(cell_index_type c)
{
  assert(can_collapse(c));

  // Get the index of this node.
  node_index_type node_index(child_link(c).index());

  // Tell the parent of this cell that it is now an empty leaf.
  child_link(c) = link(0, false);

  // Move the last node to fill in the hole.
  if (node_index < node_count()-1)
    {
    // Tell the new location of the moved node about its owner.
    node_owner(node_index) = node_owner(node_count()-1);

    // Tell the owner of the moved node about its new location.
    child_link(node_owner(node_index)) = link(node_index, false);

    // Tell the new location of the moved node about its children.
    child_links(node_index) = child_links(node_count()-1);

    // Tell the children of the moved node about its new location.
    for (child_index_type i(0); i < (1<<D); ++i)
      {
      cell_index_type cr(node_index, i);
      if (link& cl = child_link(cr))
        {
        leaf_data_owner(leaf_data_index_type(cl.index())) = cr;
        }
      else if (node_index_type ni = node_index_type(cl.index()))
        {
        node_owner(ni) = cr;
        }
      }

    // TODO: Add a callback here in case the user wants to know about
    // the moved node.  The node_data_link_destroy call below informs
    // the node data link management structure about the moved node
    // but does nothing if there are no node data.  We need to add a
    // third policy argument that provides this callback.  The user
    // may use that policy to inject an implementation of this
    // callback for example to update cell_index_type instances when a
    // node is moved (which would allow iterators to not be invalidated
    // when a different node is removed).
    //
    // this->node_moved(node_count()-1, node_index);
    }

  // Deallocate the unused node entry.
  node_owners_.erase(node_owners_.end()-1, node_owners_.end());
  child_links_.erase(child_links_.end()-1, child_links_.end());
  this->node_data_link_destroy(node_index);
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
bool
rgtl_compact_tree<D, Policy1, Policy2>
::can_subdivide(cell_index_type c) const
{
  if (link const& l = child_link(c))
    {
    // The cell is a leaf with data.  It cannot be divided.
    return false;
    }
  else if (l.index())
    {
    // The cell is a node.  It cannot be divided again.
    return false;
    }
  else
    {
    // The cell is a leaf with no data.  It can be divided.
    return true;
    }
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
bool
rgtl_compact_tree<D, Policy1, Policy2>
::can_collapse(cell_index_type c) const
{
  if (link const& l = child_link(c))
    {
    // The cell is a leaf with data.  It cannot be collapsed.
    return false;
    }
  else if (l.index())
    {
    // The cell is a node.
    if (this->node_data_get_impl(node_index_type(l.index())))
      {
      // The node has data.  It cannot be collapsed.
      return false;
      }
    else
      {
      // The node may be collapsed only if all children are leaf
      // cells without data.
      for (child_index_type i(0); i < (1<<D); ++i)
        {
        cell_index_type child = get_child(c, i);
        if (link const& cl = child_link(child))
          {
          // This child is a leaf with data.  The parent cannot be
          // collapsed.
          return false;
          }
        else if (cl.index())
          {
          // This child is a node.  The parent cannot be collapsed.
          return false;
          }
        }

      // All children are empty leaves.  The node may be collapsed.
      return true;
      }
    }
  else
    {
    // The cell is a leaf with no data.  It cannot be collapsed.
    return false;
    }
}

//----------------------------------------------------------------------------
template <unsigned int D, typename Policy1, typename Policy2>
bool
rgtl_compact_tree<D, Policy1, Policy2>
::has_data(cell_index_type c) const
{
  if (link const& cl = child_link(c))
    {
    return true;
    }
  else
    {
    return this->node_data_get_impl(node_index_type(cl.index()));
    }
}

#endif
