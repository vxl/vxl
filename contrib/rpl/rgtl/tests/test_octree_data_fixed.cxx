//:
// \file
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <testlib/testlib_test.h>

#include <rgtl/rgtl_compact_tree.hxx>
#include <rgtl/rgtl_octree_data_fixed.hxx>

typedef rgtl_compact_tree_data_fixed<2> tt0;
template class rgtl_compact_tree<tt0::derived_arg0,
                                 tt0::derived_arg1,
                                 tt0::derived_arg2>;
template class rgtl_compact_tree_data_fixed<2>;
template class rgtl_octree_data_fixed<2>;

typedef rgtl_compact_tree_data_fixed<2, int, float> tt1;
template class rgtl_compact_tree<tt1::derived_arg0,
                                 tt1::derived_arg1,
                                 tt1::derived_arg2>;
template class rgtl_compact_tree_data_fixed<2, int, float>;
template class rgtl_octree_data_fixed<2, int, float>;

static void
test_octree_data_fixed()
{
  typedef rgtl_octree_data_fixed<2, int, float> tree_type;
  testlib_test_begin("fixed data construct");
  tree_type tree;
  testlib_test_perform(true);

  tree_type::child_index_type c0(0);
  tree_type::cell_location_type root;
  tree_type::cell_location_type child0 = root.get_child(c0);
  tree_type::cell_location_type grandchild0 = child0.get_child(c0);

  testlib_test_begin("fixed data parent location");
  testlib_test_perform(root == child0.get_parent() &&
                       child0 == grandchild0.get_parent());

  testlib_test_begin("fixed data store node");
  float nd = 1.2f;
  tree.set_node_data(child0, &nd);
  testlib_test_perform(true);

  testlib_test_begin("fixed data store leaf");
  int ld = 1;
  tree.set_leaf_data(grandchild0, &ld);
  testlib_test_perform(true);

  testlib_test_begin("fixed data read node");
  float const* pnd = tree.get_node_data(child0);
  testlib_test_perform(pnd && *pnd == nd);

  testlib_test_begin("fixed data read leaf");
  int const* pld = tree.get_leaf_data(grandchild0);
  testlib_test_perform(pld && *pld == ld);

  testlib_test_begin("fixed data read empty node");
  pnd = tree.get_node_data(root);
  testlib_test_perform(!pnd);

  testlib_test_begin("fixed data clear leaf");
  tree.set_leaf_data(grandchild0, 0);
  testlib_test_perform(true);

  testlib_test_begin("fixed data clear node");
  tree.set_node_data(child0, 0);
  testlib_test_perform(true);
}

TESTMAIN(test_octree_data_fixed);
