/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt) */

#ifndef rgtl_compact_tree_index_h
#define rgtl_compact_tree_index_h
//:
// \file
// \brief Index a node in a rgtl_compact_tree.
// \author Brad King
// \date December 2006


#include "rgtl_tagged_index.h"
#include "rgtl_serialize_access.h"

//: Tags for type-safe indexes.
class rgtl_child_index_tag {};
class rgtl_leaf_data_index_tag {};
class rgtl_node_data_index_tag {};
class rgtl_node_index_tag {};

//: Type-safe index of a child within a parent node.
typedef rgtl_tagged_index<rgtl_child_index_tag> rgtl_child_index_type;

//: Type-safe index of a leaf data entry.
typedef rgtl_tagged_index<rgtl_leaf_data_index_tag> rgtl_leaf_data_index_type;

//: Type-safe index of an internal node data entry.
typedef rgtl_tagged_index<rgtl_node_data_index_tag> rgtl_node_data_index_type;

//: Type-safe index of an internal node.
typedef rgtl_tagged_index<rgtl_node_index_tag> rgtl_node_index_type;

//: Index a node in a rgtl_compact_tree.
//
// Since rgtl_compact_tree does not allocate storage for the leaf nodes
// in a tree, a simple integer index cannot be used to reference all
// nodes.  Instead we use a two-part index: the parent and child.
// Storage is allocated for all internal tree nodes to store
// information about their children.  A sentinel node is allocated to
// represent the parent of the root node.  Therefore any node can be
// indexed by storing the index of its parent and the index of the
// child within the parent.
template <unsigned int D>
class rgtl_compact_tree_index
{

 public:
  //: Type-safe index type for nodes.
  typedef rgtl_node_index_type node_index_type;

  //: Type-safe index type for children.
  typedef rgtl_child_index_type child_index_type;

  //: Default-construct to the index of the root node.
  rgtl_compact_tree_index(): p_(0), c_(0) {}

  //: Construct with given parent and child index components explicitly.
  rgtl_compact_tree_index(node_index_type p,
                          child_index_type c): p_(p), c_(c) {}

  //: Get the index of the parent node as a type-safe index.
  //  This is the parent of the node indexed.
  node_index_type parent() const { return node_index_type(p_); }

  //: Get the index of the child within the parent as a type-safe index.
  child_index_type child() const { return child_index_type(c_); }

  //: Return true if the index is valid.
  explicit operator bool () const
  { return p_ != invalid_parent_value()? true : false; }

  //: Return true if the index is invalid.
  bool operator!() const
  { return p_ == invalid_parent_value(); }

  //: Order the indices.
  friend bool operator<(rgtl_compact_tree_index l,
                        rgtl_compact_tree_index r)
  {
    if (l.parent() < r.parent()) { return true; }
    else if (l.parent() > r.parent()) { return false; }
    else { return l.child() < r.child(); }
  }

  //: Test Equality
  friend bool operator==(rgtl_compact_tree_index l,
                         rgtl_compact_tree_index r)
  {
    return l.parent() == r.parent() && l.child() == r.child();
  }

  //: Order the indices.
  friend bool operator>(rgtl_compact_tree_index l,
                        rgtl_compact_tree_index r)
    {
    if (l.parent() > r.parent()) { return true; }
    else if (l.parent() < r.parent()) { return false; }
    else { return l.child() > r.child(); }
    }

  //: Return a special index value used to indicate an invalid index.
  static rgtl_compact_tree_index invalid()
  {
    return
      rgtl_compact_tree_index(node_index_type(invalid_parent_value()),
                              child_index_type(0));
  }

 private:
  typedef typename node_index_type::index_type index_type;

  //: The number of bits needed to store the index of the child within the parent.
  //  All internal nodes have 2^D children, so the child index consumes D bits.
  enum { child_bits = D };

  //: The number of bits left to store the index of the parent.
  enum { parent_bits = sizeof(index_type)*8 - child_bits };

  //: Return a special parent index value used to indicate an invalid index.
  static index_type invalid_parent_value()
  {
    // In order to allow the maximum number of parents possible, use
    // the largest available parent index to indicate an invalid
    // index.
    return (index_type(1)<<parent_bits)-1;
  }

  //: The combined index can be efficiently stored as a single integer.
  //  The components can be accessed easily using bit-fields.
  index_type p_: parent_bits;
  index_type c_: child_bits;

  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    // TODO: Convert the members to a union with a struct for the
    // bitfields.
    index_type& data = reinterpret_cast<index_type&>(*this);
    sr & data;
  }
};

#endif
