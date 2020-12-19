#include <iostream>
#include "testlib/testlib_test.h"
#include <limits>
#include <string>
#include <map>
#include <math.h>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <bpgl/acal/acal_match_tree.h>
#include <bpgl/acal/acal_match_utils.h>

static void test_match_tree()
{
  // generic match tree contructor/destructor
  auto mt_delete_me = std::make_shared<acal_match_tree>(100);
  TEST("acal_match_tree constructor", true, true);
  mt_delete_me = nullptr;
  TEST("acal_match_tree destructor", true, true);


  // 12 -> 21
  acal_corr c11( 818, vgl_point_2d<double>(188.987, 227.430));
  acal_corr c12(1617, vgl_point_2d<double>(278.163, 315.765));
  acal_match_pair m1b(c11, c12);
  acal_corr c21( 983, vgl_point_2d<double>(109.553, 284.380));
  acal_corr c22(1834, vgl_point_2d<double>(150.748, 361.344));
  acal_match_pair m2b(c21, c22);
  acal_corr c31(1019, vgl_point_2d<double>(334.196, 286.747));
  acal_corr c32(2022, vgl_point_2d<double>(418.509, 393.688));
  acal_match_pair m3b(c31, c32);
  acal_corr c41(1073, vgl_point_2d<double>(327.695, 300.218));
  acal_corr c42(2163, vgl_point_2d<double>(420.439, 419.478));
  acal_match_pair m4b(c41, c42);
  acal_corr c51(1075, vgl_point_2d<double>(323.612, 301.511));
  acal_corr c52(2159, vgl_point_2d<double>(415.352, 420.685));
  acal_match_pair m5b(c51, c52);
  acal_corr c61(2395, vgl_point_2d<double>(224.962, 100.704));
  acal_corr c62( 541, vgl_point_2d<double>(282.362, 108.280));
  acal_match_pair m6b(c61, c62);
  std::vector<acal_match_pair> mpairs_b;
  mpairs_b.push_back(m1b);   mpairs_b.push_back(m2b);   mpairs_b.push_back(m3b);
  mpairs_b.push_back(m4b);   mpairs_b.push_back(m5b);   mpairs_b.push_back(m6b);

  // 21 -> 22
  acal_corr c11a(1988, vgl_point_2d<double>(138.454, 388.516));
  acal_corr c12a(1378, vgl_point_2d<double>(145.002, 400.503));
  acal_match_pair m1a(c11a, c12a);
  acal_corr c21a(2047, vgl_point_2d<double>(124.258, 406.217));
  acal_corr c22a(1468, vgl_point_2d<double>(130.762, 419.905));
  acal_match_pair m2a(c21a, c22a);
  acal_corr c31a(2159, vgl_point_2d<double>(415.352, 420.685));
  acal_corr c32a(1427, vgl_point_2d<double>(460.936, 405.096));
  acal_match_pair m3a(c31a, c32a);
  acal_corr c41a(2163, vgl_point_2d<double>(420.439, 419.478));
  acal_corr c42a(1429, vgl_point_2d<double>(466.727, 403.056));
  acal_match_pair m4a(c41a, c42a);
  acal_corr c51a(2207, vgl_point_2d<double>(84.9441, 445.987));
  acal_corr c52a(1629, vgl_point_2d<double>(83.7094, 476.475));
  acal_match_pair m5a(c51a, c52a);
  acal_corr c61a(2308, vgl_point_2d<double>(73.7507, 450.587));
  acal_corr c62a(1696, vgl_point_2d<double>(72.5715, 481.109));
  acal_match_pair m6a(c61a, c62a);
  std::vector<acal_match_pair> mpairs_a;
  mpairs_a.push_back(m1a);   mpairs_a.push_back(m2a);   mpairs_a.push_back(m3a);
  mpairs_a.push_back(m4a);   mpairs_a.push_back(m5a);   mpairs_a.push_back(m6a);

  size_t root_id = 12;
  acal_match_tree mt(root_id);
  std::cout << "create empty tree" << std::endl;

  auto root = mt.root_;
  TEST("acal_match_tree root_id", root->cam_id_, root_id);
  TEST("acal_match_tree root->is_root()", root->is_root(), true);
  TEST("acal_match_tree root->is_leaf()", root->is_leaf(), true);

  size_t child_id = 21;
  mt.add_child_node(root_id, child_id, mpairs_b);
  std::cout << "add single child" << std::endl;

  TEST("acal_match_tree root->size()", root->size(), 1);
  TEST("acal_match_tree root->is_root()", root->is_root(), true);
  TEST("acal_match_tree !root->is_leaf()", root->is_leaf(), false);

  auto child = root->children_[0];
  TEST("acal_match_tree child_id", child->cam_id_, child_id);
  TEST("acal_match_tree !child->is_root()", child->is_root(), false);
  TEST("acal_match_tree child->is_leaf()", child->is_leaf(), true);
  TEST("acal_match_tree child->parent_id()", child->parent_id(), root_id);

  mt.add_child_node(21, 22, mpairs_a);
  mt.add_child_node(12, 210, mpairs_b);
  mt.add_child_node(21, 220, mpairs_a);
  mt.add_child_node(210, 221, mpairs_a);
  mt.add_child_node(21, 222, mpairs_a);
  std::cout << "add many nodes" << std::endl;
  mt.update_tree_size();
  std::vector<size_t> children_ids = {21, 210};
  TEST("acal_match_tree root->children_ids()", root->children_ids(), children_ids);

  std::vector<size_t> cam_ids = {12, 21, 22, 210, 220, 221, 222};
  TEST("acal_match_tree sorted cam_ids()", mt.cam_ids(), cam_ids);

  auto nodes = mt.nodes();
  std::vector<size_t> node_ids;
  std::transform(nodes.begin(), nodes.end(), std::back_inserter(node_ids),
                 [](std::shared_ptr<acal_match_node> node) -> size_t { return node->cam_id_; });

  std::cout << "recovered node ids: ";
  for (auto node_id : node_ids)
    std::cout << node_id << " ";
  std::cout << std::endl;

  TEST("acal_match_tree nodes()", node_ids, cam_ids);

  size_t find_child_id = 222;
  auto found_child = acal_match_tree::find(root, find_child_id);
  TEST("acal_match_tree::find has correct id", found_child->cam_id_, find_child_id);

  auto parent = found_child->parent();
  TEST("acal_match_tree::find has correct parent id", parent->cam_id_, 21);

  size_t sc = parent->self_to_child_matches_.size();
  bool good = sc == 3;
  good = good && parent->self_to_child_matches_[0].size() == 2;
  TEST("add nodes to tree", good, true);

  std::map<size_t, std::vector<vgl_point_2d<double> > > corrs;
  mt.collect_correspondences(mt.root_, corrs);
  vgl_point_2d<double> p = corrs[12][0];
  good = good && p == c41.pt_;
  TEST("collect correspondences", good, true);

  // equivalence tests
  acal_match_tree mt_copy(12);
  mt_copy.add_child_node(12, 21, mpairs_b);
  mt_copy.add_child_node(21, 22, mpairs_a);
  mt_copy.add_child_node(12, 210, mpairs_b);
  mt_copy.add_child_node(21, 220, mpairs_a);
  mt_copy.add_child_node(210, 221, mpairs_a);
  mt_copy.add_child_node(21, 222, mpairs_a);

  std::cout << "created second match tree for equivalence tests" << std::endl;
  std::cout << "Original - " << mt << std::endl;
  std::cout << "Copy - " << mt_copy << std::endl;

  auto rootA = mt.root_;
  auto rootB = mt_copy.root_;
  TEST("acal_match_tree root node ==", *rootA, *rootB);

  find_child_id = 21;
  auto childA = acal_match_tree::find(mt.root_, find_child_id);
  auto childB = acal_match_tree::find(mt_copy.root_, find_child_id);
  TEST("acal_match_tree child node ==", *childA, *childB);

  TEST("acal_match_tree ==", mt, mt_copy);

  // copy constructor
  acal_match_tree mt_copy_constr(mt);
  TEST("copy constructor ==", mt, mt_copy_constr);

  std::vector<size_t> to_remove;
  to_remove.push_back(21);
  acal_match_tree mt_cam_removed_21(mt, to_remove);  
  std::cout << "Match tree with 21 removed \n" << mt_cam_removed_21 << std::endl;
  TEST("tree with one child removed", mt_cam_removed_21.size(), 3);
  // add other child to remove list
  to_remove.push_back(210);
  acal_match_tree mt_cam_removed_210(mt, to_remove);  
  std::cout << "Match tree with 21 and 210 removed \n" << mt_cam_removed_210 << std::endl;
  TEST("tree with both children removed", mt_cam_removed_210.size(), 1);
  to_remove.clear();
  to_remove.push_back(12);
  acal_match_tree mt_cam_removed_12(mt, to_remove);
  std::cout << "Match tree with 12 removed \n" << mt_cam_removed_12 << std::endl;
  TEST("invalid isolated root", mt_cam_removed_12.root_->cam_id_, size_t(-1));

  to_remove.clear();
  to_remove.push_back(3);
  acal_match_tree mt_cam_removed_3(mt, to_remove);
  std::cout << "Match tree with non_existent node 3 removed - same as copy constructor \n" << mt_cam_removed_3 << std::endl;
  TEST("copy is  ==", mt, mt_cam_removed_3);
}

TESTMAIN(test_match_tree);
