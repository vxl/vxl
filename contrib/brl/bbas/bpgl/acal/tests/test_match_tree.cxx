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

static void test_match_tree()
{

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

  std::shared_ptr<acal_match_node> root(new acal_match_node(12, 21, mpairs_b));
  std::shared_ptr<acal_match_node> child = acal_match_tree::find(root, 21);
  TEST("test parent child constructor, find", bool(child), true);

  acal_match_tree mt;
  mt.root_ = root;

  mt.add_child_node(21, 22, mpairs_a);
  mt.add_child_node(12, 210, mpairs_b);
  mt.add_child_node(21, 220, mpairs_a);
  mt.add_child_node(210, 221, mpairs_a);
  mt.add_child_node(21, 222, mpairs_a);
  child = acal_match_tree::find(root, 222);

  acal_match_node* parent = child->parent_;
  size_t sc = parent->self_to_child_matches_.size();
  bool good = sc == 3;
  good = good && parent->self_to_child_matches_[0].size() == 2;
  TEST("add nodes to tree", good, true);

  std::map<size_t, std::vector<vgl_point_2d<double> > > corrs;
  mt.collect_correspondences(mt.root_, corrs);
  vgl_point_2d<double> p = corrs[12][0];
  good = good && p == c41.pt_;
  TEST("collect correspondences", good, true);
}

TESTMAIN(test_match_tree);
