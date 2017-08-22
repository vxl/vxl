//:
// \file test_bstm_to_multi_bstm_block_function.cxx
// \brief Tests BSTM to Multi-BSTM conversion function
// \author Raphael Kargon
// \date 21-Aug-2017

#include <testlib/testlib_root_dir.h>
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_utility.h>

#include <vgl/vgl_vector_3d.h>

#include <bstm_multi/cpp/algo/bstm_multi_bstm_block_to_bstm_multi_block_function.h>

void test_volume() {
  vcl_pair<vgl_vector_3d<int>, int> region(vgl_vector_3d<int>(2, 3, 4), 5);
  TEST("test volume int", volume(region), 120);
  vcl_pair<vgl_vector_3d<double>, double> region2(
      vgl_vector_3d<double>(10, 10, 10), 10);
  TEST("test volume double", volume(region2), 1e4);
}

void test_get_bstm_data_buffers() {
  bstm_data_base *alpha = VXL_NULLPTR, *app = VXL_NULLPTR;
  vcl_string app_type, alpha_prefix = bstm_data_traits<BSTM_ALPHA>::prefix();
  vcl_map<vcl_string, bstm_data_base *> map;
  get_bstm_data_buffers(map, alpha, app, app_type);
  TEST("get bstm data buffs, empty map", alpha, VXL_NULLPTR);
  TEST("get bstm data buffs, empty map", app, VXL_NULLPTR);

  map[alpha_prefix] = new bstm_data_base(VXL_NULLPTR, 0, bstm_block_id());
  get_bstm_data_buffers(map, alpha, app, app_type);
  TEST("get bstm data buffs, no app", alpha, map[alpha_prefix]);
  TEST("get bstm data buffs, no app", app, VXL_NULLPTR);

  map["BSTM_SCHMERZ"] = new bstm_data_base(VXL_NULLPTR, 0, bstm_block_id());
  get_bstm_data_buffers(map, alpha, app, app_type);
  TEST("get bstm data buffs, no matching app", alpha, map[alpha_prefix]);
  TEST("get bstm data buffs, no matching app", app, VXL_NULLPTR);

  map[bstm_data_traits<BSTM_MOG3_GREY>::prefix()] =
      new bstm_data_base(VXL_NULLPTR, 0, bstm_block_id());
  get_bstm_data_buffers(map, alpha, app, app_type);
  TEST("get bstm data buffs, has app", alpha, map[alpha_prefix]);
  TEST("get bstm data buffs, has app",
       app,
       map[bstm_data_traits<BSTM_MOG3_GREY>::prefix()]);
}

void test_make_unrefined_time_tree() {
  // This is a somewhat contrived test - this function is an
  // intermediate step and as such assumes data is arranged a certain
  // way.
  {
    bstm_time_tree t;
    vcl_vector<bool> diffs(32, false);
    make_unrefined_time_tree(t, 0, diffs);
    TEST("make unrefined time tree, all frames the same", t.num_leaves(), 1);
  }
  {
    bstm_time_tree t;
    // three sets of time trees, but 't' corresponds to the middle one, index=1
    vcl_vector<bool> diffs(32 * 3, false);
    diffs[32 + 16] = true;
    make_unrefined_time_tree(t, 1, diffs);
    TEST("make unrefined time tree, two sets of frames split at middle",
         t.num_leaves(),
         2);
  }
  {
    bstm_time_tree t;
    // three sets of time trees, but 't' corresponds to the middle one, index=1
    vcl_vector<bool> diffs(32 * 3, false);
    vcl_fill(diffs.begin(), diffs.end(), true);
    make_unrefined_time_tree(t, 1, diffs);
    TEST("make unrefined time tree, all frames different from the last",
         t.num_leaves(),
         32);
  }
}

void test_make_unrefined_space_tree() {
  {
    boct_bit_tree t;
    vcl_pair<vgl_vector_3d<unsigned>, unsigned> num_regions(
        vgl_vector_3d<unsigned>(2, 2, 2), 2);
    int vol = volume(num_regions);
    index_4d coords(1, 0, 0, 1);
    unsigned char *child_level_buffer =
        new unsigned char[vol * 512 * space_tree_size]();
    vcl_vector<bool> diffs(vol * 512, false);

    bool res = make_unrefined_space_tree(
        t, num_regions, coords, child_level_buffer, STE_SPACE, diffs);
    TEST("unrefine space tree, all children empty and same", res, false);
    TEST(
        "unrefine space tree, all children empty and same", t.bit_at(0), false);
  }

  {
    boct_bit_tree t = boct_bit_tree();
    vcl_pair<vgl_vector_3d<unsigned>, unsigned> num_regions(
        vgl_vector_3d<unsigned>(2, 2, 2), 2);
    int vol = volume(num_regions);
    index_4d coords(1, 0, 0, 1);
    unsigned char *child_level_buffer =
        new unsigned char[vol * 512 * space_tree_size]();
    vcl_vector<bool> diffs(vol * 512, false);

    // set one of the sub-trees to be different
    diffs[9 * 512 + 100 + 1] = true;
    unsigned char *current_children_start =
        child_level_buffer + (8 * 512 + 1) * space_tree_size;
    boct_bit_tree(current_children_start).set_bit_at(0, 1);
    boct_bit_tree(current_children_start + space_tree_size * 2)
        .set_bit_at(0, 1);

    // first two cells (in row major order) should be refined, as well as their
    // parents ofc.

    bool res = make_unrefined_space_tree(
        t, num_regions, coords, child_level_buffer, STE_SPACE, diffs);
    TEST("unrefine space tree, some children different", res, true);
    int l1_idx =
        t.parent_index(t.traverse(vgl_point_3d<double>(0, 0, 0), 4, true));
    int l2_idx = t.parent_index(
        t.traverse(vgl_point_3d<double>(0, 0, 1 / 8.0), 4, true));
    // test that other low-level bits are unrefined
    bool refined = false;
    for (int i = 73; i < 584; ++i) {
      if (i != l1_idx && i != l2_idx) {
        refined |= t.bit_at(i);
      }
    }
    TEST("All but two bits are unrefined.", refined, false);
    // Test refined bits
    for (; l1_idx >= 0; l1_idx = t.parent_index(l1_idx)) {
      TEST("l1_idx", t.bit_at(l1_idx), true);
    }
    for (; l2_idx >= 0; l2_idx = t.parent_index(l2_idx)) {
      TEST("l2_idx", t.bit_at(l2_idx), true);
    }
  }
}

void test_bstm_to_multi_bstm_block_function() {
  // test implementation sub-routines...
  test_volume();
  test_get_bstm_data_buffers();
  // test_convert_bstm_space_trees();
  // test_time_differences_from_bstm_trees();
  test_make_unrefined_space_tree();
  test_make_unrefined_time_tree();
  // test_compute_trees_structure();
  // test_coalesce_trees();

  // test empty scene
  // test actual BSTM scene
}

TESTMAIN(test_bstm_to_multi_bstm_block_function);
