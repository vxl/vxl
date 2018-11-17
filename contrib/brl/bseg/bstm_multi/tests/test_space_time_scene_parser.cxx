//:
// \file test_space_time_scene_parser.cxx
// \brief Tests the generic space-time scene parser.
// \author Raphael Kargon
// \date 29-Jul-2017

#include <iostream>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_box_3d.h>

#include <testlib/testlib_root_dir.h>
#include <testlib/testlib_test.h>

#include <bstm/bstm_block.h>
#include <bstm_multi/bstm_multi_block.h>
#include <bstm_multi/space_time_scene_parser.hxx>

// This returns default-initialized parser on failure.
template <typename Block>
space_time_scene_parser<Block> load_file(const std::string& filename) {
  space_time_scene_parser<Block> parser;
  if (filename.size() > 0) {
    std::FILE *xml_file = std::fopen(filename.c_str(), "r");
    if (!xml_file) {
      std::cerr << filename.c_str() << " error on opening\n";
      perror("Error!");
      TEST("Error opening file", true, false);
      return parser;
    }
    if (!parser.parseFile(xml_file)) {
      std::cerr << XML_ErrorString(parser.XML_GetErrorCode()) << " at line "
               << parser.XML_GetCurrentLineNumber() << '\n';
      TEST("Error parsing XML", true, false);
      return parser;
    }
    std::fclose(xml_file);
  }
  return parser;
}

// Tests whether the bounds of two bounding boxes are equal, within the given
// tolerance.
void test_bbox_equal_approx(const char *s,
                            const vgl_box_3d<double> &b1,
                            const vgl_box_3d<double> &b2,
                            double tol = 1e-12) {
  testlib_test_begin(s);
  testlib_test_assert_near("", b1.min_x(), b2.min_x(), tol);
  testlib_test_assert_near("", b1.min_y(), b2.min_y(), tol);
  testlib_test_assert_near("", b1.min_z(), b2.min_z(), tol);
  testlib_test_assert_near("", b1.max_x(), b2.max_x(), tol);
  testlib_test_assert_near("", b1.max_y(), b2.max_y(), tol);
  testlib_test_assert_near("", b1.max_z(), b2.max_z(), tol);
}

void test_space_time_scene_parser() {
  std::string data_dir =
      testlib_root_dir() + "/contrib/brl/bseg/bstm_multi/tests/";
  std::string bstm_filename = data_dir + "bstm_scene/scene.xml";
  std::string bstm_multi_filename = data_dir + "bstm_multi_scene/scene.xml";

  vgl_box_3d<double> block_bbox(-1, -4, -0.25, 1.88, -0.16, 2.63);
  std::vector<std::string> appearances;
  appearances.emplace_back("bstm_mog6_view_compact");
  appearances.emplace_back("bstm_num_obs_view_compact");

  {
    space_time_scene_parser<bstm_block> bstm_parser =
        load_file<bstm_block>(bstm_filename);
    TEST("bstm appearances", bstm_parser.appearances(), appearances);
    // Compare bounding boxes and time range of each block
    std::map<bstm_block_id, bstm_block_metadata> bstm_blocks =
        bstm_parser.blocks();
    std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
    for (iter = bstm_blocks.begin(); iter != bstm_blocks.end(); ++iter) {
      test_bbox_equal_approx(
          ("Block ID " + iter->first.to_string() + " has correct bounding box")
              .c_str(),
          iter->second.bbox(),
          block_bbox);
      // Check that block's time origin is equal to block time ID *
      // (frames/block)
      TEST(("Block ID " + iter->first.to_string() + " has correct time origin")
               .c_str(),
           iter->first.t() * iter->second.sub_block_dim_t_ *
               iter->second.sub_block_num_t_,
           iter->second.local_origin_t_);
    }
    TEST("bstm version", bstm_parser.version(), 2);
  }

  {
    space_time_scene_parser<bstm_multi_block> bstm_multi_parser =
        load_file<bstm_multi_block>(bstm_multi_filename);
    TEST(
        "bstm_multi appearances", bstm_multi_parser.appearances(), appearances);
    // Compare bounding boxes and time range of each block
    std::map<bstm_block_id, bstm_multi_block_metadata> bstm_multi_blocks =
        bstm_multi_parser.blocks();
    std::map<bstm_block_id, bstm_multi_block_metadata>::const_iterator iter;
    for (iter = bstm_multi_blocks.begin(); iter != bstm_multi_blocks.end();
         ++iter) {
      test_bbox_equal_approx(
          ("Block ID " + iter->first.to_string() + " has correct bounding box")
              .c_str(),
          iter->second.bbox(),
          block_bbox);

      // Check that block's time origin is equal to block time ID *
      // (frames/block)
      std::pair<double, double> bbox_t = iter->second.bbox_t();
      TEST(("Block ID " + iter->first.to_string() + " has correct time origin")
               .c_str(),
           iter->first.t() * (bbox_t.second - bbox_t.first),
           bbox_t.first);

      // Check subdivisions
      TEST(("Block ID " + iter->first.to_string() + " has correct subdivisions")
               .c_str(),
           iter->second.subdivisions_,
           parse_subdivisions("time,space,time,space"));
    }
    TEST("bstm multi version", bstm_multi_parser.version(), 2);
  }
}

TESTMAIN(test_space_time_scene_parser);
