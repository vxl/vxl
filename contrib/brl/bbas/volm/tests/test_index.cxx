#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
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
  vcl_vector<volm_tile> tiles = volm_tile::generate_p1_wr2_tiles(); 
  
  volm_geo_index_node_sptr root = volm_geo_index::construct_tree(tiles[3], tiles[0].scale_i_/4);
  TEST("root children", root->children_.size(), 4);
  TEST("root children", root->children_[0]->children_.size(), 4);
  TEST("root children", root->children_[0]->children_[0]->children_.size(), 0);
  TEST("root depth", volm_geo_index::depth(root), 2);
  vcl_cout << "depth of root: " << volm_geo_index::depth(root) << vcl_endl;
  vcl_string test_file = ".\\root.kml";
  vcl_string test_file2 = ".\\root_depth_2.kml";
  volm_geo_index::write_to_kml(root, 0, ".\\tile_3.kml");

  vgl_polygon<double> polyw = bkml_parser::parse_polygon(".\\Coast-ROI.kml");


  vcl_cout << "world poly  has: " << polyw[0].size() << vcl_endl;
  // tile 3 contains charleston area
  volm_geo_index_node_sptr root2 = volm_geo_index::construct_tree(tiles[3], 100*tiles[3].scale_i_/3600, polyw);
  vcl_cout << "depth of root2: " << volm_geo_index::depth(root2) << vcl_endl;
  volm_geo_index::write_to_kml(root2, 0, ".\\roo2_depth_0.kml");
  volm_geo_index::write_to_kml(root2, 1, ".\\roo2_depth_1.kml");
  volm_geo_index::write_to_kml(root2, 5, ".\\roo2_depth_5.kml");
  //TEST_NEAR("tile 2 img to global lat ", lat3, lat2, 0.01);

  volm_geo_index::write(root2, ".\\roo2.txt", 100*tiles[3].scale_i_/3600);

  float min_s;
  volm_geo_index_node_sptr root3 = volm_geo_index::read_and_construct(".\\roo2.txt", min_s);
  volm_geo_index::write_to_kml(root3, 0, ".\\roo3_depth_0.kml");
  volm_geo_index::write_to_kml(root3, 1, ".\\roo3_depth_1.kml");
  volm_geo_index::write_to_kml(root3, 6, ".\\roo3_depth_6.kml");
  
  vcl_vector<volm_geo_index_node_sptr> leaves3;
  volm_geo_index::get_leaves(root3, leaves3);
  vcl_vector<volm_geo_index_node_sptr> leaves2;
  volm_geo_index::get_leaves(root2, leaves2);
  TEST("size of leaves", leaves3.size() == leaves2.size(), true);
  vcl_cout << "there are " << leaves3.size() << " leaves in root3!\n";
  vcl_cout << "depth of root3: " << volm_geo_index::depth(root3) << vcl_endl;
}

TESTMAIN(test_index);
