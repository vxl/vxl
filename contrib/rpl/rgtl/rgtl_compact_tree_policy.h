#ifndef rgtl_compact_tree_policy_h
#define rgtl_compact_tree_policy_h
//:
// \file
// \brief Simple node and leaf data value policies for rgtl_compact_tree.
// \author Brad King
// \date December 2006
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "rgtl_compact_tree_node_data.h"

//: Enumerate the kinds of data values stored in a rgtl_compact_tree.
enum { rgtl_compact_tree_policy_leaf_data,
       rgtl_compact_tree_policy_node_data };

//: Base class for node data value policies.
class rgtl_node_data_value_policy_base
{
 public:
  typedef std::size_t index_type;
  enum { policy_type = rgtl_compact_tree_policy_node_data };
};

//: Base class for leaf data value policies.
class rgtl_leaf_data_value_policy_base
{
 public:
  typedef std::size_t index_type;
  enum { policy_type = rgtl_compact_tree_policy_leaf_data };
};

//----------------------------------------------------------------------------

//: Node data value policy for use when no node data are needed in rgtl_compact_tree<>.
class rgtl_node_data_value_policy_empty:
  public rgtl_node_data_value_policy_base
{
 public:
  void node_data_value_create() {}
  void node_data_value_destroy(index_type) {}
};

//: Leaf data value policy for use when no leaf data are needed in rgtl_compact_tree<>.
class rgtl_leaf_data_value_policy_empty:
  public rgtl_leaf_data_value_policy_base
{
 public:
  void leaf_data_value_create() {}
  void leaf_data_value_destroy(index_type) {}
};

//----------------------------------------------------------------------------

//:
//  Node data value policy based on pure virtual functions that must
//  be implemented by a subclass of the rgtl_compact_tree<>
//  instantiation to which this policy is given.  This is useful for
//  runtime selection of the policy.
class rgtl_node_data_value_policy_abstract:
  public rgtl_node_data_value_policy_base
{
 public:
  virtual ~rgtl_node_data_value_policy_abstract() {}
  virtual void node_data_value_create() = 0;
  virtual void node_data_value_destroy(index_type) = 0;
};

//:
//  Leaf data value policy based on pure virtual functions that must
//  be implemented by a subclass of the rgtl_compact_tree<>
//  instantiation to which this policy is given.  This is useful for
//  runtime selection of the policy.
class rgtl_leaf_data_value_policy_abstract:
  public rgtl_leaf_data_value_policy_base
{
 public:
  virtual ~rgtl_leaf_data_value_policy_abstract() {}
  virtual void leaf_data_value_create() = 0;
  virtual void leaf_data_value_destroy(index_type) = 0;
};

//----------------------------------------------------------------------------

//: Template to lookup the default data value policy for the policy
//  Type NOT given as the second template argument of rgtl_compact_tree.
template <int> struct rgtl_compact_tree_policy_default;

template <>
struct rgtl_compact_tree_policy_default<rgtl_compact_tree_policy_node_data>
{
  typedef rgtl_leaf_data_value_policy_empty policy;
};

template <>
struct rgtl_compact_tree_policy_default<rgtl_compact_tree_policy_leaf_data>
{
  typedef rgtl_node_data_value_policy_empty policy;
};

//----------------------------------------------------------------------------

//: Template to lookup the real class to use for a leaf or non-empty node data value policy given to rgtl_compact_tree.
//  Used by rgtl_compact_tree_policy_lookup when the policy is not
//  rgtl_node_data_value_policy_empty or derived from it.
template <int> struct rgtl_compact_tree_policy_lookup_type;
template <>
struct rgtl_compact_tree_policy_lookup_type<rgtl_compact_tree_policy_leaf_data>
{
  //: Leaf data value policies are used directly.
  template <typename Policy>
  struct get
  {
    typedef Policy policy;
  };
};

template <>
struct rgtl_compact_tree_policy_lookup_type<rgtl_compact_tree_policy_node_data>
{
  //: Node data value policies are used through the node data policy template.
  template <typename Policy>
  struct get
  {
    typedef rgtl_compact_tree_node_data<Policy> policy;
  };
};

//----------------------------------------------------------------------------

//: Template to detect whether or not a given policy is rgtl_node_data_value_policy_empty or derived from it.
template <typename Policy>
struct rgtl_compact_tree_policy_is_empty_node
{
 private:
  typedef char (&no_type)[1];
  typedef char (&yes_type)[2];
  static no_type check(void*);
  static yes_type check(rgtl_node_data_value_policy_empty*);
 public:
  enum { value = (sizeof(yes_type) ==
                  sizeof(check(static_cast<Policy*>(0)))) };
};

//----------------------------------------------------------------------------

//: Template to switch the policy lookup based on whether the policy is for empty node data or real data.
template <bool> struct rgtl_compact_tree_policy_lookup_internal;
template <> struct rgtl_compact_tree_policy_lookup_internal<true>
{
  template <class Policy>
  struct get
  {
    //:
    //  When an empty node data value policy is used we use an empty
    //  implementation of the node data policy as an optimization.
    typedef rgtl_compact_tree_node_data_none<Policy> policy;
  };
};

template <> struct rgtl_compact_tree_policy_lookup_internal<false>
{
  template <class Policy>
  struct get
  {
    //:
    //  When any leaf data value policy or a non-empty node data value policy
    //  is given choose the real policy class based on the type of policy.
    typedef typename
    rgtl_compact_tree_policy_lookup_type<Policy::policy_type>
    ::template get<Policy>::policy policy;
  };
};

//----------------------------------------------------------------------------

//: Template to lookup the real class to use for a leaf or node data value policy given to rgtl_compact_tree.
template <typename Policy>
struct rgtl_compact_tree_policy_lookup
{
  // Detect empty node data policies.
  enum { is_empty_node_data =
         rgtl_compact_tree_policy_is_empty_node<Policy>::value };

  // Dispatch the lookup.
  typedef typename
  rgtl_compact_tree_policy_lookup_internal<(is_empty_node_data?true:false)>
  ::template get<Policy>::policy policy;
};

#endif // rgtl_compact_tree_policy_h
