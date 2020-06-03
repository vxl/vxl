// acal/io/tests/test_match_tree_io.cxx
#include "test_generic_io.h"
#include <acal/io/acal_io_match_tree.h>

// acal_match_tree io test
static void
test_match_tree_io()
{
  acal_match_tree tree(0);

  std::vector<acal_match_pair> root_to_child1_matches;
  for (size_t i = 0; i < 5; i++) {
    vgl_point_2d<double> pt1(i, i);
    vgl_point_2d<double> pt2(i + 1, i + 2);
    acal_match_pair mp(acal_corr(i, pt1), acal_corr(i, pt2));
    root_to_child1_matches.push_back(mp);
  }

  tree.root_->add_child(1, root_to_child1_matches);

  std::vector<acal_match_pair> child1_to_child2_matches;
  for (size_t i = 0; i < 10; i++) {
    vgl_point_2d<double> pt1(i, i);
    vgl_point_2d<double> pt2(i, i * 2);
    acal_match_pair mp(acal_corr(i, pt1), acal_corr(i, pt2));
    child1_to_child2_matches.push_back(mp);
  }

  tree.root_->children_[0]->add_child(2, child1_to_child2_matches);

  test_generic_io(tree, "acal_match_tree");
}

TESTMAIN(test_match_tree_io);
