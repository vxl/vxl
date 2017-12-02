//:
// \file
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <testlib/testlib_test.h>
#include <rgtl/rgtl_octree_dual_visit.h>
#include <rgtl/rgtl_octree_data_fixed.hxx>

template <class Tree2D>
class dual_counter
{
 public:
  typedef Tree2D tree_type;
  dual_counter(tree_type const& t): tree_(t)
  {
    this->counts_[0] = 0;
    this->counts_[1] = 0;
    this->counts_[2] = 0;
    this->ecount_[0] = 0;
    this->ecount_[1] = 0;
  }
  tree_type const& tree() const
  {
    return this->tree_;
  }

  // Test visiting dual points.
  void operator()(rgtl_octree_dual_element<2, 0> const&)
  {
    ++this->counts_[0];
  }

  // Test visiting dual edges with orientation.
  template <unsigned int m>
  void operator()(rgtl_octree_dual_element_oriented<2, 1, m> const& in)
  {
    ++this->counts_[1];
    ++this->ecount_[m-1];
  }

  // Test visiting dual cells.
  void operator()(rgtl_octree_dual_element<2, 2> const&)
  {
    ++this->counts_[2];
  }

  int count(unsigned int i) const { return this->counts_[i]; }
  int hcount() const { return this->ecount_[0]; }
  int vcount() const { return this->ecount_[1]; }
 private:
  tree_type const& tree_;
  int counts_[3];
  int ecount_[2];
  RGTL_STATIC_ASSERT(tree_type::dimension == 2);
};

static void
test_octree_dual_visit()
{
  // Create a simple tree.
  typedef rgtl_octree_data_fixed<2, int> tree_type;
  tree_type::child_index_type c0(0);
  tree_type::child_index_type c3(3);
  tree_type::cell_location_type root;
  tree_type::cell_location_type child0 = root.get_child(c0);
  tree_type::cell_location_type grandchild3 = child0.get_child(c3);
  tree_type::cell_location_type greatgrandchild0 = grandchild3.get_child(c0);
  tree_type tree;
  int ld = 0;
  tree.set_leaf_data(greatgrandchild0, &ld);

  // Check the dual cell visitation counts.
  dual_counter<tree_type> c(tree);
  rgtl_octree_dual_visit(c);
  TEST("dual cell visit",c.count(0) == 10 &&
                         c.count(1) == 18 &&
                         c.count(2) == 9 &&
                         c.hcount() == 9 &&
                         c.vcount() == 9, true);
}

TESTMAIN(test_octree_dual_visit);
