// This is contrib/brl/bbas/bvgl/algo/tests/test_2d_geo_index.cxx
//:
// \file
// \brief Tests for bvgl_2d_geo_index
// \author Yi Dong
// \data Nov. 01, 2013

#include <iostream>
#include <testlib/testlib_test.h>
#include <bvgl/algo/bvgl_2d_geo_index.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_2d_geo_index()
{
  // construct a 2d_geo_index for a square region within (0.0, 0.0) to (1.0, 1.0)
  // leaf size is smaller than 0.05, each leaf contains a vector of integers
  float min_size = 0.2f;
  vgl_box_2d<float> bbox(0.0, 1.0, 0.0, 1.0);
  bvgl_2d_geo_index_node_sptr root = bvgl_2d_geo_index::construct_tree<std::vector<int> >(bbox, min_size);

  // test the kml write out
  unsigned tree_depth = bvgl_2d_geo_index::depth(root);
  for (unsigned i = 0; i <= tree_depth; i++) {
    std::stringstream kml_file;
    kml_file << "./" << "bvgl_2d_geo_index_depth_" << i << ".kml";
    bvgl_2d_geo_index::write_to_kml(root, i, kml_file.str());
  }
  std::vector<bvgl_2d_geo_index_node_sptr> leaves;
  bvgl_2d_geo_index::get_leaves(root, leaves);
  std::cout << " extent of 2d_geo_index: " << root->extent_.width() << 'x' << root->extent_.height() << std::endl;
  std::cout << " size of the leaf node: " << leaves[0]->extent_.width() << 'x' << leaves[0]->extent_.height() << std::endl;
  TEST("depth of the tree", tree_depth, 3);
  TEST("number of leaves", leaves.size(), 64);

  // write the tree structure
  std::string txt_file = "./tree_strcuture.txt";
  bvgl_2d_geo_index::write(root, txt_file, min_size);

  // prune the tree by polygon;
  vgl_polygon<float> poly1;
  poly1.new_sheet();
  poly1.push_back(vgl_point_2d<float>(0.55,0.65));  poly1.push_back(vgl_point_2d<float>(0.55,0.40));  poly1.push_back(vgl_point_2d<float>(0.80,0.40));
  bvgl_2d_geo_index::prune_tree(root, poly1);
  leaves.clear();
  bvgl_2d_geo_index::get_leaves(root, leaves);
  tree_depth = bvgl_2d_geo_index::depth(root);
  for (unsigned i = 0; i <= tree_depth; i++) {
    std::stringstream kml_file;
    kml_file << "./" << "bvgl_2d_geo_index_after_prune_depth_" << i << ".kml";
    bvgl_2d_geo_index::write_to_kml(root, i, kml_file.str());
  }
  std::cout << " the tree is pruned by polygon: " << std::endl;
  poly1.print(std::cout);
  TEST("prune process with polygon", leaves.size(), 6);

  // create a tree and use a polygon to prune the tree simultaneously
  bvgl_2d_geo_index_node_sptr root_poly = bvgl_2d_geo_index::construct_tree<std::vector<int> >(bbox, min_size, poly1);
  leaves.clear();
  bvgl_2d_geo_index::get_leaves(root_poly, leaves);
  tree_depth = bvgl_2d_geo_index::depth(root);
  for (unsigned i = 0; i <= tree_depth; i++) {
    std::stringstream kml_file;
    kml_file << "./" << "bvgl_2d_geo_index_poly_depth_" << i << ".kml";
    bvgl_2d_geo_index::write_to_kml(root, i, kml_file.str());
  }
  TEST("create a tree using with polygon", leaves.size(), 6);


  // create the tree from text
  double min_size1;
  bvgl_2d_geo_index_node_sptr root1 = bvgl_2d_geo_index::read_and_construct<std::vector<int> >(txt_file, min_size1);

  // quickly locate the leaves that intersect with some rectangular region
  vgl_box_2d<float> region(0.3f,0.55f,0.3f,0.8f);
  leaves.clear();
  bvgl_2d_geo_index::get_leaves(root1, leaves, region);
  std::cout << " leaves that intersect with region: " << region << ':' << std::endl;
  for (unsigned i = 0; i < leaves.size(); i++)
    std::cout << "\t " << i << ": " << leaves[i]->extent_ << std::endl;
  TEST("quick access to leaves intersecting a rectangular region", leaves.size(), 15);

  // quickly locate the leaves that intersect with a line
  std::vector<vgl_point_2d<float> > line;
  line.emplace_back(0.2f, 0.2f);
  line.emplace_back(0.2f, 0.4f);
  line.emplace_back(0.4f, 0.6f);
  leaves.clear();
  bvgl_2d_geo_index::get_leaves(root1, leaves, line);
  std::cout << " leaves that intersect with line: ";
  for (auto i : line) {
    std::cout << i << ' ';
  }
  std::cout << '\n';
  for (unsigned i = 0; i < leaves.size(); i++) {
    std::cout << "\t " << i << ": " << leaves[i]->extent_ << std::endl;
  }
  TEST("quick access to leaves intersecting a line", leaves.size(), 6);

  // quickly locate the leaves that intersect with a polygon
  leaves.clear();
  bvgl_2d_geo_index::get_leaves(root1, leaves, poly1);
  std::cout << " leaves that intersect with polygon: ";  poly1.print(std::cout);
  for (unsigned i = 0; i < leaves.size(); i++) {
    std::cout << "\t " << i << ": " << leaves[i]->extent_ << std::endl;
  }
  TEST("quick access to leaves intersecting a polygon", leaves.size(), 6);

  // quickly locate the leaf that contains the given point
  vgl_point_2d<double> pt(0.12,0.31);
  bvgl_2d_geo_index_node_sptr leaf;
  bvgl_2d_geo_index::get_leaf(root1, leaf, pt);
  std::cout << " leaf " << leaf->extent_ << " contains the point " << pt << std::endl;
  TEST("quick access to leaf containing the given point", leaf->extent_.contains(pt), true);

  // add content into each leaf
  leaves.clear();
  bvgl_2d_geo_index::get_leaves(root1, leaves);
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    auto* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<std::vector<int> >* >(leaves[l_idx].ptr());
    for (unsigned i = 0; i < 3; i++)
      leaf_ptr->contents_.push_back(i + l_idx*2);
  }

  // get the content for the leaves that intersect with the give region
  leaves.clear();
  bvgl_2d_geo_index::get_leaves(root1, leaves, region);
  std::cout << " leaves intersecting with region " << region << " have following content:" << std::endl;
  for (auto & leave : leaves) {
    std::cout << "\t leaf " << leave->extent_ << " contains: ";
    auto* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<std::vector<int> >* >(leave.ptr());
    for (int content : leaf_ptr->contents_)
      std::cout << content << ' ';
    std::cout << '\n';
  }

}

TESTMAIN( test_2d_geo_index );
