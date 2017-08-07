//:
// \file test_bstm_multi_block_metadata.cxx
// \brief tests the BSTM multi_block metadata object,
// \author Raphael Kargon
// \date 29-Jul-2017

#include <testlib/testlib_root_dir.h>
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

#include <bstm_multi/bstm_multi_block_metadata.h>

void test_space_time_enum() {
  TEST("STE_SPACE to string", ste_as_string(STE_SPACE), "space");
  TEST("STE_TIME to string", ste_as_string(STE_TIME), "time");
  space_time_enum ste;
  TEST("STE_TIME from string success", ste_from_string("time", ste), true);
  TEST("STE_TIME from string", ste, STE_TIME);
  TEST("STE_SPACE from string success", ste_from_string("space", ste), true);
  TEST("STE_SPACE from string", ste, STE_SPACE);
  TEST("STE from string failure", ste_from_string("lol", ste), false);

  // last one should be skipped since it's a typo
  vcl_string subdivs = "time,space,time,space,spaec";
  vcl_vector<space_time_enum> subdivisions = parse_subdivisions(subdivs);
  TEST("parse subdivisions skip incorrect", subdivisions.size(), 4);
  TEST("parse subdivisions", subdivisions[0], STE_TIME);
  TEST("parse subdivisions", subdivisions[1], STE_SPACE);
  TEST("parse subdivisions", subdivisions[2], STE_TIME);
  TEST("parse subdivisions", subdivisions[3], STE_SPACE);

  TEST("print subdivisions",
       print_subdivisions(subdivisions),
       "time,space,time,space");
}

void test_block_metadata_equality() {
  bstm_multi_block_metadata metadata;
  metadata.id_ = bstm_block_id(0, 0, 0, 1);
  metadata.bbox_ = vgl_box_3d<double>(0, 0, 0, 10, 11, 12);
  metadata.bbox_t_ = vcl_pair<double, double>(0, 4.5);

  bstm_multi_block_metadata metadata_same;
  metadata_same.id_ = bstm_block_id(0, 0, 0, 1);
  metadata_same.bbox_ = vgl_box_3d<double>(0, 0, 0, 10, 11, 12);
  metadata_same.bbox_t_ = vcl_pair<double, double>(0, 4.5);

  bstm_multi_block_metadata metadata_different;
  metadata_different.id_ = bstm_block_id(0, 0, 0, 2);
  metadata_different.bbox_ = vgl_box_3d<double>(0, 0, 0, 10, 11, 12);
  metadata_different.bbox_t_ = vcl_pair<double, double>(0, 4.5);

  bstm_multi_block_metadata metadata_different_bounds;
  metadata_different_bounds.id_ = bstm_block_id(0, 0, 0, 1);
  metadata_different_bounds.bbox_ = vgl_box_3d<double>(0, 0, 0, 10, 11, 12);
  metadata_different_bounds.bbox_t_ = vcl_pair<double, double>(0, 4.6);

  bstm_multi_block_metadata metadata_subdivs;
  metadata_subdivs.bbox_ = vgl_box_3d<double>(0, 0, 0, 16, 32, 16);
  metadata_subdivs.bbox_t_ = vcl_pair<double,double>(0, 32);
  metadata_subdivs.subdivisions_.push_back(STE_SPACE);
  metadata_subdivs.subdivisions_.push_back(STE_TIME);
  metadata_subdivs.subdivisions_.push_back(STE_SPACE);
  vcl_pair<vgl_vector_3d<double>, double> expected_resolution;
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
  TEST("block resolution value", metadata_subdivs.resolution(), expected_resolution);
  TEST("block resolution equality", voxel_resolutions_match(metadata_subdivs, boxm2_metadata_same_resolution), true);
}

void test_block_metadata_contains_t() {
  bstm_multi_block_metadata metadata;
  metadata.id_ = bstm_block_id(0, 0, 0, 1);
  metadata.bbox_ = vgl_box_3d<double>(0, 0, 0, 10, 11, 12);
  metadata.bbox_t_ = vcl_pair<double, double>(0, 4.5);
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
  metadata.bbox_t_ = vcl_pair<double, double>(0, 4.5);
  vcl_cout << metadata << vcl_endl;
}

void test_bstm_multi_block_metadata() {
  test_space_time_enum();
  test_block_metadata_equality();
  test_block_metadata_contains_t();
  test_block_metadata_printing();
}

TESTMAIN(test_bstm_multi_block_metadata);
