#include <testlib/testlib_test.h>
//:
// \file
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <rgtl/rgtl_compact_tree.hxx>
#include <rgtl/rgtl_compact_tree_data_fixed.h>


typedef rgtl_compact_tree_data_fixed<3> tt0;

template class rgtl_compact_tree_data_fixed<3>;

template class rgtl_compact_tree<tt0::derived_arg0,
                                 tt0::derived_arg1,
                                 tt0::derived_arg2>;

typedef rgtl_compact_tree_data_fixed<3, int, float> tt1;
template class rgtl_compact_tree_data_fixed<3, int, float>;
template class rgtl_compact_tree<tt1::derived_arg0,
                                 tt1::derived_arg1,
                                 tt1::derived_arg2>;

static void
test_data_empty()
{
  typedef rgtl_compact_tree_data_fixed<3> tree_type;

  testlib_test_begin("empty data construct");
  tree_type tree;
  testlib_test_perform(true);

  testlib_test_begin("empty data subdivide");
  tree_type::child_index_type c0;
  tree_type::cell_index_type root;
  tree.subdivide(root);
  tree.subdivide(tree.get_child(root, c0));
  testlib_test_perform(tree.has_children(root) &&
                       tree.has_children(tree.get_child(root, c0)));

  testlib_test_begin("empty data collapse");
  tree.collapse(tree.get_child(root, c0));
  tree.collapse(root);
  testlib_test_perform(!tree.has_children(root));
}

static void
test_data_fixed()
{
  typedef rgtl_compact_tree_data_fixed<3, int, float> tree_type;
  testlib_test_begin("fixed data construct");
  tree_type tree;
  testlib_test_perform(true);

  testlib_test_begin("fixed data subdivide");
  tree_type::child_index_type c0;
  tree_type::cell_index_type root;
  tree.subdivide(root);
  tree_type::cell_index_type child0 = tree.get_child(root, c0);
  tree.subdivide(child0);
  tree_type::cell_index_type grandchild0 = tree.get_child(child0, c0);
  testlib_test_perform(tree.has_children(root) &&
                       tree.has_children(child0));

  testlib_test_begin("fixed data parents");
  testlib_test_perform(!tree.get_parent(root) &&
                       root == tree.get_parent(child0) &&
                       child0 == tree.get_parent(grandchild0));

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

  testlib_test_begin("fixed data clear leaf");
  tree.set_leaf_data(grandchild0, 0);
  testlib_test_perform(true);

  testlib_test_begin("fixed data clear node");
  tree.set_node_data(child0, 0);
  testlib_test_perform(true);

  testlib_test_begin("fixed data collapse");
  tree.collapse(child0);
  tree.collapse(root);
  testlib_test_perform(!tree.has_children(root));
}

static void test_compact_tree_data_fixed()
{
  test_data_empty();
  test_data_fixed();
}

TESTMAIN(test_compact_tree_data_fixed);
