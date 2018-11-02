#ifndef rgtl_compact_tree_h
#define rgtl_compact_tree_h
//:
// \file
// \brief Compactly represent a tree.
// \author Brad King
// \date December 2006
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "rgtl_tagged_vector.h"
#include "rgtl_compact_tree_link.h"
#include "rgtl_compact_tree_index.h"
#include "rgtl_compact_tree_policy.h"
#include "rgtl_serialize_access.h"
#include "rgtl_serialize_base.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Represent a tree structure compactly using a few contiguous arrays.
//
// The represented tree has 2^D children per internal node, where D is
// the first template argument.  Data may be associated with the
// leaves and internal nodes of the tree using the Policy template
// arguments.
//
// This implementation overcomes several deficiencies in traditional
// tree representations.  Tree structures often consume more memory
// than they actually need due to storage of null pointers on the
// bottom level, unused data on some nodes, and fragmented memory.  As
// the tree is manipulated many small blocks of memory are allocated
// and deallocated.  Fragmented tree traversal has poor locality of
// reference.
template <
  unsigned int D,
  typename Policy1 = rgtl_leaf_data_value_policy_empty,
  typename Policy2 =
  typename rgtl_compact_tree_policy_default<Policy1::policy_type>::policy>
class rgtl_compact_tree:
  protected rgtl_compact_tree_policy_lookup<Policy1>::policy,
  protected rgtl_compact_tree_policy_lookup<Policy2>::policy
{
 public:
  typedef typename rgtl_compact_tree_policy_lookup<Policy1>::policy derived1;
  typedef typename rgtl_compact_tree_policy_lookup<Policy2>::policy derived2;

  //: The branching dimension of the tree.  Internal nodes have 2^D children.
  enum { dimension = D };

  //: Type-safe index of a child within a parent node.
  typedef rgtl_child_index_type child_index_type;

  //: Type-safe index of a leaf data entry.
  typedef rgtl_leaf_data_index_type leaf_data_index_type;

  //: Type-safe index of an internal node data entry.
  typedef rgtl_node_data_index_type node_data_index_type;

  //: Type-safe index of an internal node.
  typedef rgtl_node_index_type node_index_type;

  //: Type to uniquely index any cell (node or leaf) in the tree.
  typedef rgtl_compact_tree_index<D> cell_index_type;

  //: Type of link into node data and leaf data arrays.
  typedef rgtl_compact_tree_link link;

  //: Default-construct a tree consisting of just a root leaf.
  rgtl_compact_tree();

  //: Returns true if the given cell has children.
  bool has_children(cell_index_type c) const;

  //: Return the index of a child of an internal node.
  //  The given cell must have children.
  cell_index_type get_child(cell_index_type c,
                            child_index_type child_index) const;

  //: Return the index of the parent node of the given cell.
  //  If the given cell is the root cell an invalid index is returned
  //  such that conversion to boolean yields false.
  cell_index_type get_parent(cell_index_type c) const;

  //: Return a link into the leaf data array for the given leaf.
  //  The given cell must be a leaf.
  //  If the leaf has no data the returned link will test false.
  link leaf_data_get(cell_index_type c) const;

  //: Insert a leaf data entry for the given leaf.
  //  The given cell must be a leaf.  If the leaf already has data there is
  //  no effect.  Returns a link into the leaf data array for the given leaf.
  link leaf_data_insert(cell_index_type c);

  //: Erase a leaf data entry for the given leaf.
  //  The given cell must be a leaf.
  //  If the leaf has no data there is no effect.
  void leaf_data_erase(cell_index_type c);

  //: Return a link into the node data array for the given node.
  //  The given cell must be an internal node.
  //  If the node has no data the returned link will test false.
  link node_data_get(cell_index_type c) const;

  //: Insert a node data entry for the given node.
  //  The given cell must be an internal node.
  //  If the node already has data there is no effect.
  //  Returns a link into the node data array for the given node.
  link node_data_insert(cell_index_type c);

  //: Erase a node data entry for the given node.
  //  The given cell must be an internal node.
  //  If the node has no data there is no effect.
  void node_data_erase(cell_index_type c);

  //: Create children for the given leaf thus converting it to a node.
  //  The given cell must be a leaf and have no data when this method
  //  is invoked.
  void subdivide(cell_index_type c);

  //: Remove children from the given node thus converting it to a leaf.
  //  The given cell must be a node with no data and its children must
  //  have no children or data of their own when this method is invoked.
  void collapse(cell_index_type c);

 protected:

  class child_links_entry
  {
   public:
    link& operator[](child_index_type c) { return links_[c]; }
    link const& operator[](child_index_type c) const { return links_[c]; }
   private:
    link links_[1<<D];
    friend class rgtl_serialize_access;
    template <class Serializer>
    void serialize(Serializer& sr) { sr& links_; }
  };

  bool can_subdivide(cell_index_type c) const;
  bool can_collapse(cell_index_type c) const;
  bool has_data(cell_index_type c) const;

 private:
  rgtl_tagged_vector<rgtl_node_index_tag, cell_index_type> node_owners_;
  node_index_type node_count() const
  {
    return node_index_type(node_owners_.size());
  }
  cell_index_type& node_owner(node_index_type node_index)
  {
    return node_owners_[node_index];
  }
  cell_index_type const& node_owner(node_index_type node_index) const
  {
    return node_owners_[node_index];
  }

 private:
  rgtl_tagged_vector<rgtl_node_index_tag, child_links_entry> child_links_;
  link& child_link(cell_index_type c)
  {
    return child_links_[c.parent()][c.child()];
  }
  link const& child_link(cell_index_type c) const
  {
    return child_links_[c.parent()][c.child()];
  }
  link& leaf_data_link(cell_index_type c)
  {
    return child_links_[c.parent()][c.child()];
  }
  link const& leaf_data_link(cell_index_type c) const
  {
    link const& ldl = child_links_[c.parent()][c.child()];
    assert("child link is a leaf data link" && (ldl || !ldl.index()));
    return ldl;
  }
  child_links_entry& child_links(node_index_type node_index)
  {
    return child_links_[node_index];
  }
  child_links_entry const& child_links(node_index_type node_index) const
  {
    return child_links_[node_index];
  }

 private:
  rgtl_tagged_vector<rgtl_leaf_data_index_tag,
                     cell_index_type> leaf_data_owners_;

  cell_index_type& leaf_data_owner(leaf_data_index_type leaf_data_index)
  {
    return leaf_data_owners_[leaf_data_index];
  }
  cell_index_type const&
  leaf_data_owner(leaf_data_index_type leaf_data_index) const
  {
    return leaf_data_owners_[leaf_data_index];
  }
  leaf_data_index_type leaf_data_count() const
  {
    return leaf_data_index_type(leaf_data_owners_.size());
  }

  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr& rgtl_serialize_base<derived1>(*this);
    sr& rgtl_serialize_base<derived2>(*this);
    sr& node_owners_;
    sr& child_links_;
    sr& leaf_data_owners_;
  }
};

#endif // rgtl_compact_tree_h
