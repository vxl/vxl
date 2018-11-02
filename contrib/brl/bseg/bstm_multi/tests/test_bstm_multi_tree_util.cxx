//:
// \file test_bstm_multi_tree_util.cxx
// \brief Tests Multi-BSTM Tree Util
// \author Raphael Kargon
// \date 21-Aug-2017

#include <iostream>
#include <string>
#include <testlib/testlib_root_dir.h>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bstm_multi/bstm_multi_tree_util.h>

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
  std::string subdivs = "time,space,time,space,spaec";
  std::vector<space_time_enum> subdivisions = parse_subdivisions(subdivs);
  TEST("parse subdivisions skip incorrect", subdivisions.size(), 4);
  TEST("parse subdivisions", subdivisions[0], STE_TIME);
  TEST("parse subdivisions", subdivisions[1], STE_SPACE);
  TEST("parse subdivisions", subdivisions[2], STE_TIME);
  TEST("parse subdivisions", subdivisions[3], STE_SPACE);

  TEST("print subdivisions",
       print_subdivisions(subdivisions),
       "time,space,time,space");

  TEST("tree_size", tree_size(STE_SPACE), 16);
  TEST("tree_size", tree_size(STE_TIME), 8);
}

void test_generic_tree() {

  // Space tree
  {
    boct_bit_tree space_tree;
    generic_tree space_gen(space_tree.get_bits(), STE_SPACE);
    TEST("space gen tree type", space_gen.type(), STE_SPACE);

    bool ok = true;
    try {
      boct_bit_tree space_tree2 = space_gen.space_tree();
    } catch (const tree_type_exception &e) {
      ok = false;
    }
    TEST("space tree get space tree", ok, true);
    // getting a time tree should throw an exception.
    ok = false;
    try {
      bstm_time_tree space_tree2 = space_gen.time_tree();
    } catch (const tree_type_exception &e) {
      ok = true;
    }
    TEST("space tree get time tree", ok, true);

    TEST("space gen tree size", space_gen.tree_size(), tree_size(STE_SPACE));
    TEST("space gen tree bits", space_gen.bits(), space_tree.get_bits());

    // bit manip
    space_tree.set_bit_at(0, true);
    TEST("space gen tree root bit", space_tree.bit_at(0), space_gen.root_bit());

    space_gen.set_bit_at(17, true);
    TEST("space gen tree set_bit_at", space_tree.bit_at(17), true);
    TEST("space gen tree bit_at", space_gen.bit_at(17), true);

    space_tree.set_data_ptr(5000);
    TEST("space gen tree get data ptr",
         space_gen.get_data_ptr(),
         space_tree.get_data_ptr());
    space_gen.set_data_ptr(600);
    TEST("space gen tree set data ptr",
         space_gen.get_data_ptr(),
         space_tree.get_data_ptr());
    TEST("space gen get leaf bits",
         space_gen.get_leaf_bits(),
         space_tree.get_leaf_bits());

    TEST("space gen num leaves",
         space_gen.num_leaves(),
         space_tree.num_leaves());

    // local coords - voxel nodes
    TEST("space gen tree local voxel coords",
         space_gen.local_voxel_coords(73),
         index_4d(0, 0, 0, 0));
    TEST("space gen tree local voxel coords",
         space_gen.local_voxel_coords(74),
         index_4d(1, 0, 0, 0));
    TEST("space gen tree local voxel coords",
         space_gen.local_voxel_coords(584),
         index_4d(7, 7, 7, 0));
    // local coords - leaf/inner nodes
    TEST("space gen tree local voxel coords",
         space_gen.local_voxel_coords(0),
         index_4d(0, 0, 0, 0));
    TEST("space gen tree local voxel coords",
         space_gen.local_voxel_coords(9),
         index_4d(0, 0, 0, 0));
    TEST("space gen tree local voxel coords",
         space_gen.local_voxel_coords(2),
         index_4d(4, 0, 0, 0));
    TEST("space gen tree local voxel coords",
         space_gen.local_voxel_coords(3),
         index_4d(0, 4, 0, 0));
  }

  // Space tree
  {
    bstm_time_tree time_tree;
    generic_tree time_gen(time_tree.get_bits(), STE_TIME);
    TEST("time gen tree type", time_gen.type(), STE_TIME);

    bool ok = true;
    try {
      bstm_time_tree time_tree2 = time_gen.time_tree();
    } catch (const tree_type_exception &e) {
      ok = false;
    }
    TEST("time tree get time tree", ok, true);
    // getting a time tree should throw an exception.
    ok = false;
    try {
      boct_bit_tree time_tree2 = time_gen.space_tree();
    } catch (const tree_type_exception &e) {
      ok = true;
    }
    TEST("time tree get space tree", ok, true);

    TEST("time gen tree size", time_gen.tree_size(), tree_size(STE_TIME));
    TEST("time gen tree bits", time_gen.bits(), time_tree.get_bits());

    // bit manip
    time_tree.set_bit_at(0, true);
    TEST("time gen tree root bit", time_tree.bit_at(0), time_gen.root_bit());

    time_gen.set_bit_at(17, true);
    TEST("time gen tree set_bit_at", time_tree.bit_at(17), true);
    TEST("time gen tree bit_at", time_gen.bit_at(17), true);

    time_tree.set_data_ptr(5000);
    TEST("time gen tree get data ptr",
         time_gen.get_data_ptr(),
         time_tree.get_data_ptr());
    time_gen.set_data_ptr(600);
    TEST("time gen tree set data ptr",
         time_gen.get_data_ptr(),
         time_tree.get_data_ptr());
    TEST("time gen get leaf bits",
         time_gen.get_leaf_bits(),
         time_tree.get_leaf_bits());

    TEST("time gen num leaves", time_gen.num_leaves(), time_tree.num_leaves());

    // local coords - voxel nodes
    TEST("time gen tree local voxel coords",
         time_gen.local_voxel_coords(31),
         index_4d(0, 0, 0, 0));
    TEST("time gen tree local voxel coords",
         time_gen.local_voxel_coords(32),
         index_4d(0, 0, 0, 1));
    TEST("time gen tree local voxel coords",
         time_gen.local_voxel_coords(62),
         index_4d(0, 0, 0, 31));
    // local coords - leaf/inner nodes
    TEST("time gen tree local voxel coords",
         time_gen.local_voxel_coords(0),
         index_4d(0, 0, 0, 0));
    TEST("time gen tree local voxel coords",
         time_gen.local_voxel_coords(9),
         index_4d(0, 0, 0, 8));
    TEST("time gen tree local voxel coords",
         time_gen.local_voxel_coords(10),
         index_4d(0, 0, 0, 12));
  }
}

void test_bstm_multi_tree_util() {
  test_space_time_enum();
  test_generic_tree();
}

TESTMAIN(test_bstm_multi_tree_util);
