//:
// \file test_bstm_multi_block_metadata.cxx
// \brief tests the BSTM multi_block metadata object,
// \author Raphael Kargon
// \date 29-Jul-2017

#include <iostream>
#include <string>
#include <testlib/testlib_root_dir.h>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bstm_multi/bstm_multi_block_metadata.h>

void test_block_metadata_equality() {
  bstm_multi_block_metadata metadata;
  metadata.id_ = bstm_block_id(0, 0, 0, 1);
  metadata.bbox_ = vgl_box_3d<double>(0, 0, 0, 10, 11, 12);
  metadata.bbox_t_ = std::pair<double, double>(0, 4.5);

  bstm_multi_block_metadata metadata_same;
  metadata_same.id_ = bstm_block_id(0, 0, 0, 1);
  metadata_same.bbox_ = vgl_box_3d<double>(0, 0, 0, 10, 11, 12);
  metadata_same.bbox_t_ = std::pair<double, double>(0, 4.5);

  bstm_multi_block_metadata metadata_different;
  metadata_different.id_ = bstm_block_id(0, 0, 0, 2);
  metadata_different.bbox_ = vgl_box_3d<double>(0, 0, 0, 10, 11, 12);
  metadata_different.bbox_t_ = std::pair<double, double>(0, 4.5);

  bstm_multi_block_metadata metadata_different_bounds;
  metadata_different_bounds.id_ = bstm_block_id(0, 0, 0, 1);
  metadata_different_bounds.bbox_ = vgl_box_3d<double>(0, 0, 0, 10, 11, 12);
  metadata_different_bounds.bbox_t_ = std::pair<double, double>(0, 4.6);

  bstm_multi_block_metadata metadata_subdivs;
  metadata_subdivs.bbox_ = vgl_box_3d<double>(0, 0, 0, 16, 32, 16);
  metadata_subdivs.bbox_t_ = std::pair<double, double>(0, 32);
  metadata_subdivs.subdivisions_.push_back(STE_SPACE);
  metadata_subdivs.subdivisions_.push_back(STE_TIME);
  metadata_subdivs.subdivisions_.push_back(STE_SPACE);
  std::pair<vgl_vector_3d<double>, double> expected_resolution;
  expected_resolution.first = vgl_vector_3d<double>(0.25, 0.5, 0.25);
  expected_resolution.second = 1.0;

  boxm2_block_metadata boxm2_metadata_same_resolution;
  boxm2_metadata_same_resolution.sub_block_dim_ =
      vgl_vector_3d<double>(2, 4, 2);

  TEST("reflexive block metadata equality", metadata, metadata);
  TEST("block metadata equality", metadata, metadata_same);
  TEST("block metadata inequality", metadata == metadata_different, false);
  TEST("block metadata inequality",
       metadata == metadata_different_bounds,
       false);
  TEST("block resolution value",
       metadata_subdivs.resolution(),
       expected_resolution);
  TEST(
      "block resolution equality",
      voxel_resolutions_match(metadata_subdivs, boxm2_metadata_same_resolution),
      true);
}

void test_block_metadata_contains_t() {
  bstm_multi_block_metadata metadata;
  metadata.id_ = bstm_block_id(0, 0, 0, 1);
  metadata.bbox_ = vgl_box_3d<double>(0, 0, 0, 10, 11, 12);
  metadata.bbox_t_ = std::pair<double, double>(0, 4.5);
  double local_time;
  TEST("block time containment (t, min, max)",
       metadata.contains_t(-5, local_time),
       false);
  TEST("block time containment (min, max, t)",
       metadata.contains_t(55, local_time),
       false);
  TEST("block time containment (min, t, max)",
       metadata.contains_t(2.25, local_time),
       true);
  TEST("block local time", local_time, 0.5);
}

void test_block_metadata_printing() {
  bstm_multi_block_metadata metadata;
  metadata.id_ = bstm_block_id(0, 0, 0, 1);
  metadata.bbox_ = vgl_box_3d<double>(0, 0, 0, 10, 11, 12);
  metadata.bbox_t_ = std::pair<double, double>(0, 4.5);
  std::cout << metadata << std::endl;
}

void test_bstm_multi_block_metadata() {
  test_block_metadata_equality();
  test_block_metadata_contains_t();
  test_block_metadata_printing();
}

TESTMAIN(test_bstm_multi_block_metadata);
