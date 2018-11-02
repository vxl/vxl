/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_compact_tree_data_fixed_h
#define rgtl_compact_tree_data_fixed_h

#include <iostream>
#include <vector>
#include "rgtl_compact_tree.h"
#include "rgtl_serialize_access.h"
#include "rgtl_serialize_base.h"
#include "rgtl_serialize_stl_vector.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <typename LeafDataType>
class rgtl_compact_tree_data_fixed_leaf_data_policy:
  public rgtl_leaf_data_value_policy_base
{
public:
  typedef LeafDataType leaf_data_type;
  rgtl_compact_tree_data_fixed_leaf_data_policy():
    rgtl_leaf_data_value_policy_base() {}
  void leaf_data_value_create()
    {
    leaf_data_values_.push_back(leaf_data_type());
    }
  void leaf_data_value_destroy(index_type index)
    {
    index_type last = leaf_data_values_.size()-1;
    if(index < last)
      {
      leaf_data_values_[index] = leaf_data_values_[last];
      }
    leaf_data_values_.erase(leaf_data_values_.end()-1,
                            leaf_data_values_.end());
    }
  leaf_data_type const*
  leaf_data_value_set(rgtl_compact_tree_link const& l,
                      leaf_data_type const& d)
    {
    leaf_data_type& ldv = this->leaf_data_values_[l.index()];
    ldv = d;
    return &ldv;
    }
  leaf_data_type const*
  leaf_data_value_get(rgtl_compact_tree_link const& l) const
    {
    if(l)
      {
      return &this->leaf_data_values_[l.index()];
      }
    else
      {
      return 0;
      }
    }
  std::vector<leaf_data_type> leaf_data_values_;
private:
  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
    {
    sr & leaf_data_values_;
    }
};

template <typename NodeDataType>
class rgtl_compact_tree_data_fixed_node_data_policy:
  public rgtl_node_data_value_policy_base
{
public:
  typedef NodeDataType node_data_type;
  rgtl_compact_tree_data_fixed_node_data_policy():
    rgtl_node_data_value_policy_base() {}
  void node_data_value_create()
    {
    node_data_values_.push_back(node_data_type());
    }
  void node_data_value_destroy(index_type index)
    {
    index_type last = node_data_values_.size()-1;
    if(index < last)
      {
      node_data_values_[index] = node_data_values_[last];
      }
    node_data_values_.erase(node_data_values_.end()-1,
                            node_data_values_.end());
    }
  node_data_type const*
  node_data_value_set(rgtl_compact_tree_link const& l,
                      node_data_type const& d)
    {
    node_data_type& ndv = this->node_data_values_[l.index()];
    ndv = d;
    return &ndv;
    }
  node_data_type const*
  node_data_value_get(rgtl_compact_tree_link const& l) const
    {
    if(l)
      {
      return &this->node_data_values_[l.index()];
      }
    else
      {
      return 0;
      }
    }

  std::vector<node_data_type> node_data_values_;
private:
  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
    {
    sr & node_data_values_;
    }
};

struct rgtl_compact_tree_data_fixed_unused;

template <>
class rgtl_compact_tree_data_fixed_leaf_data_policy<void>:
  public rgtl_leaf_data_value_policy_empty
{
public:
  typedef rgtl_compact_tree_link link;
  typedef rgtl_compact_tree_data_fixed_unused leaf_data_type;
  leaf_data_type const*
  leaf_data_value_set(link const&, leaf_data_type const&) { return 0; }
  leaf_data_type const* leaf_data_value_get(link const&) const { return 0; }
private:
  friend class rgtl_serialize_access;
  template <class Serializer> void serialize(Serializer&) {}
};

template <>
class rgtl_compact_tree_data_fixed_node_data_policy<void>:
  public rgtl_node_data_value_policy_empty
{
public:
  typedef rgtl_compact_tree_link link;
  typedef rgtl_compact_tree_data_fixed_unused node_data_type;
  node_data_type const*
  node_data_value_set(link const&, node_data_type const&) { return 0; }
  node_data_type const* node_data_value_get(link const&) const { return 0; }
private:
  friend class rgtl_serialize_access;
  template <class Serializer> void serialize(Serializer&) {}
};

template <unsigned int D,
          typename LeafDataType = void,
          typename NodeDataType = void>
class rgtl_compact_tree_data_fixed:
  public rgtl_compact_tree<
    D,
    rgtl_compact_tree_data_fixed_leaf_data_policy<LeafDataType>,
    rgtl_compact_tree_data_fixed_node_data_policy<NodeDataType>
    >
{
public:
  typedef rgtl_compact_tree_data_fixed<D, LeafDataType, NodeDataType> self;
  typedef rgtl_compact_tree<
    D,
    rgtl_compact_tree_data_fixed_leaf_data_policy<LeafDataType>,
    rgtl_compact_tree_data_fixed_node_data_policy<NodeDataType> > derived;
  enum { derived_arg0 = D };
  typedef rgtl_compact_tree_data_fixed_leaf_data_policy<LeafDataType>
    derived_arg1;
  typedef rgtl_compact_tree_data_fixed_node_data_policy<NodeDataType>
    derived_arg2;
  typedef typename derived::leaf_data_type leaf_data_type;
  typedef typename derived::node_data_type node_data_type;
  typedef typename derived::cell_index_type cell_index_type;
  typedef typename derived::child_index_type child_index_type;
  typedef typename derived::node_index_type node_index_type;
  typedef typename derived::node_data_index_type node_data_index_type;
  typedef typename derived::leaf_data_index_type leaf_data_index_type;
  enum { dimension = derived::dimension };

  leaf_data_type const* set_leaf_data(cell_index_type c,
                                      leaf_data_type const* d)
    {
    if(d)
      {
      return this->leaf_data_value_set(this->leaf_data_insert(c), *d);
      }
    else
      {
      this->leaf_data_erase(c);
      return 0;
      }
    }

  leaf_data_type const* get_leaf_data(cell_index_type c) const
    {
    return this->leaf_data_value_get(this->leaf_data_get(c));
    }

  node_data_type const* set_node_data(cell_index_type c,
                                      node_data_type const* d)
    {
    if(d)
      {
      return this->node_data_value_set(this->node_data_insert(c), *d);
      }
    else
      {
      this->node_data_erase(c);
      return 0;
      }
    }

  node_data_type const* get_node_data(cell_index_type c) const
    {
    return this->node_data_value_get(this->node_data_get(c));
    }
private:
  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
    {
    sr & rgtl_serialize_base<derived>(*this);
    }
};

#endif
