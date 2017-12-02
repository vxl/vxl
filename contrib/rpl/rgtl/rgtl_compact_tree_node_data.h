// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_compact_tree_node_data_h
#define rgtl_compact_tree_node_data_h
//:
// \file
// \brief Node data policy implementations for rgtl_compact_tree.
// \author Brad King
// \date December 2006

#include "rgtl_tagged_vector.h"
#include "rgtl_compact_tree_link.h"
#include "rgtl_compact_tree_index.h"
#include "rgtl_serialize_access.h"
#include "rgtl_serialize_base.h"

//: Node data policy for rgtl_compact_tree used when the node data value policy is not rgtl_node_data_value_policy_empty.
//  Provides real implementations of node data policy methods.
template <typename NodeValuePolicy>
class rgtl_compact_tree_node_data: protected NodeValuePolicy
{
 protected:
  typedef NodeValuePolicy derived;

  //: Type-safe index of an internal node data entry.
  typedef rgtl_node_data_index_type node_data_index_type;

  //: Type-safe index of an internal node.
  typedef rgtl_node_index_type node_index_type;

  //: Type of link into node data arrays.
  typedef rgtl_compact_tree_link link;

  //: Default-construct a tree consisting of just a root leaf.
  rgtl_compact_tree_node_data(): node_data_links_(1), node_data_owners_(0) {}

  //: Create a node data link for the new node created by rgtl_compact_tree<>::subdivide.
  void node_data_link_create()
  {
    // Create the new node data link.
    node_data_links_.push_back(link(0, false));
  }

  //: Destroy the node data link for the old node removed by rgtl_compact_tree<>::collapse.
  void node_data_link_destroy(node_index_type node_index)
  {
    // Get the number of nodes.
    node_index_type node_count(node_data_links_.size());

    // Move the last node to fill in the hole.
    if (node_index < node_count-1)
    {
      // Tell the new location of the moved node about its data.
      node_data_link(node_index) = node_data_link(node_count-1);

      // Tell the data of the moved node about its new location.
      if (link const& ndl = node_data_link(node_index))
      {
        node_data_owner(node_data_index_type(ndl.index())) = node_index;
      }
    }

    // Deallocate the unused node data link.
    node_data_links_.erase(node_data_links_.end()-1,
                           node_data_links_.end());
  }

  //: Internal implementation of rgtl_compact_tree<>::node_data_get method.
  link node_data_get_impl(node_index_type node_index) const
  {
    return node_data_link(node_index);
  }

  //: Internal implementation of rgtl_compact_tree<>::node_data_insert method.
  link node_data_insert_impl(node_index_type node_index)
  {
    if (link& ndl = node_data_link(node_index))
    {
      // The node data entry already exists.
      return ndl;
    }
    else
    {
      // Create a link to the new data location.
      link l(node_data_count(), true);

      // Tell the owner of the new data about its location.
      node_data_link(node_index) = l;

      // Tell the new data about its owner.
      node_data_owners_.push_back(node_index);

      // Create the new node data value entry according to the node
      // data value policy.
      this->node_data_value_create();

      return l;
    }
  }

  //: Internal implementation of rgtl_compact_tree<>::node_data_erase method.
  void node_data_erase_impl(node_index_type node_index)
  {
    // Erase the node data entry if it exists.  If it is not the last
    // entry move the last entry into its place and then erase the
    // last entry.
    if (link const& ndl = node_data_link(node_index))
    {
      // Get the index of the node data entry.
      node_data_index_type ndi(ndl.index());

      // The original owner of node data at this index no longer owns data.
      node_data_link(node_data_owner(ndi)) = link(0, false);

      // Move the last data entry to fill in the hole.
      if (ndi < node_data_count()-1)
      {
        // Tell the new location of the moved data about its owner.
        node_data_index_type last(node_data_count()-1);
        node_data_owner(ndi) = node_data_owner(last);

        // Tell the owner of the moved data about its new location.
        node_data_link(node_data_owner(ndi)) = link(ndi, true);
      }

      // Deallocate the owner portion of the unused node data entry.
      node_data_owners_.erase(node_data_owners_.end()-1,
                              node_data_owners_.end());

      // Remove the old node data value entry according to the node
      // data value policy.
      this->node_data_value_destroy(ndi);
    }
  }

 private:
  rgtl_tagged_vector<rgtl_node_index_tag, link> node_data_links_;
  link& node_data_link(node_index_type node_index)
  {
    return node_data_links_[node_index];
  }
  link const& node_data_link(node_index_type node_index) const
  {
    return node_data_links_[node_index];
  }

 private:
  rgtl_tagged_vector<rgtl_node_data_index_tag,
                     node_index_type> node_data_owners_;
  node_index_type& node_data_owner(node_data_index_type node_data_index)
  {
    return node_data_owners_[node_data_index];
  }
  node_index_type const&
  node_data_owner(node_data_index_type node_data_index) const
  {
    return node_data_owners_[node_data_index];
  }
  node_data_index_type node_data_count() const
  {
    return node_data_index_type(node_data_owners_.size());
  }
 private:
  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr & rgtl_serialize_base<derived>(*this);
    sr & node_data_links_;
    sr & node_data_owners_;
  }
};

//: Node data policy for rgtl_compact_tree used when the node data value policy is rgtl_node_data_value_policy_empty.
//  Provides empty implementations of node data policy methods.
template <typename NodeValuePolicy>
class rgtl_compact_tree_node_data_none: protected NodeValuePolicy
{
 protected:
  typedef NodeValuePolicy derived;

  //: Default constructor declared to give it protected access.
  rgtl_compact_tree_node_data_none() {}

  //: Create a node data link for the new node created by rgtl_compact_tree<>::subdivide.
  //  This empty version is used when the node data value policy is empty.
  void node_data_link_create() {}

  //: Destroy the node data link for the old node removed by rgtl_compact_tree<>::collapse.
  //  This empty version is used when the node data value policy is empty.
  void node_data_link_destroy(rgtl_node_index_type) {}

  //: Internal implementation of rgtl_compact_tree<>::node_data_get method.
  //  This empty version is used when the node data value policy is empty.
  rgtl_compact_tree_link node_data_get_impl(rgtl_node_index_type) const
  {
    return rgtl_compact_tree_link();
  }

  //: Internal implementation of rgtl_compact_tree<>::node_data_insert method.
  //  This empty version is used when the node data value policy is empty.
  rgtl_compact_tree_link node_data_insert_impl(rgtl_node_index_type)
  {
    return rgtl_compact_tree_link();
  }

  //: Internal implementation of rgtl_compact_tree<>::node_data_erase method.
  //  This empty version is used when the node data value policy is empty.
  void node_data_erase_impl(rgtl_node_index_type) {}

  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr & rgtl_serialize_base<derived>(*this);
  }
};

#endif
