#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vsl/vsl_binary_io.h>
#include <vul/vul_timer.h>
#include <bbas/volm/volm_tile.h>
#include <bbas/volm/volm_geo_index.h>
#include <bbas/volm/volm_io.h>
#include <bkml/bkml_parser.h>

static void test_index()
{
  volm_tile tile(0.0, 0.0, 'N', 'W', 1.0, 1.0, 3600, 3600);
  volm_geo_index_node_sptr root = volm_geo_index::construct_tree(tile, tile.scale_i()/4);
  TEST("root children", root->children_.size(), 4);
  TEST("root children", root->children_[0]->children_.size(), 4);
  TEST("root children", root->children_[0]->children_[0]->children_.size(), 0);
  TEST("root depth", volm_geo_index::depth(root), 2);
  std::cout << " depth of root: " << volm_geo_index::depth(root) << std::endl;
  std::string test_file  = "./root_depth_0.kml";
  std::string test_file2 = "./root_depth_2.kml";
  volm_geo_index::write_to_kml(root, 0, test_file);
  volm_geo_index::write_to_kml(root, volm_geo_index::depth(root), test_file2);

  // construct the tree given a polygon
  vgl_polygon<double> polyw;
  polyw.new_sheet();
  polyw.push_back(0.2, 0.3);  polyw.push_back(0.6,0.3);  polyw.push_back(0.6, 0.6);  polyw.push_back(0.5, 0.5);
  std::cout << "world polygon has: " << polyw[0].size() << " vertices: ";
  polyw.print(std::cout);
  float min_size = tile.scale_i()/4.0;
  volm_geo_index_node_sptr root2 = volm_geo_index::construct_tree(tile, tile.scale_i()/4, polyw);
  std::cout << " depth of root2: " << volm_geo_index::depth(root2) << std::endl;
  volm_geo_index::write_to_kml(root2, 0, "./root2_depth_0.kml");
  volm_geo_index::write_to_kml(root2, volm_geo_index::depth(root2), "./root2_depth_2.kml");
  std::vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves(root2, leaves);
  TEST("leaves within the given polygon", leaves.size(), 5);


  // io test
  volm_geo_index::write(root2, "./root2_index.txt", min_size);
  float min_size3;
  volm_geo_index_node_sptr root3 = volm_geo_index::read_and_construct("./root2_index.txt", min_size3);
  std::vector<volm_geo_index_node_sptr> leaves3;
  volm_geo_index::get_leaves(root3, leaves3);
  TEST("io test of min_size", min_size, min_size3);
  TEST("io test of leave structure", leaves.size(), leaves3.size());
  std::cout << " depth of root3: " << volm_geo_index::depth(root3) << std::endl;
  volm_geo_index::write_to_kml(root3, 0, "./root3_depth_0.kml");
  volm_geo_index::write_to_kml(root3, volm_geo_index::depth(root3), "./root3_depth_2.kml");

}

TESTMAIN(test_index);
