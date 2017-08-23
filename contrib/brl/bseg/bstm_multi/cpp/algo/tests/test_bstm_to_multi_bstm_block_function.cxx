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

#include <bstm/bstm_data.h>

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

// Essentially this test sets up a BSTM scene where each tree has one
// level of refinement. Then, all the data elemenst are set to a
// unique value based on their spatial coordinates in the scene. After
// convert_bstm_space_trees, the data elements should still be
// recoverable based on their spatial coordinates, even though they've
// been internally rearranged.
void test_convert_bstm_space_trees() {
  /*  set up Multi-BSTM block  */
  vcl_vector<space_time_enum> subdivs(2);
  subdivs[0] = STE_SPACE;
  subdivs[0] = STE_TIME;
  bstm_multi_block_metadata mdata(bstm_block_id(),
                                  vgl_box_3d<double>(0, 0, 0, 1, 1, 1),
                                  vcl_pair<double, double>(0, 1),
                                  9000,
                                  0.01,
                                  subdivs);
  bstm_multi_block *blk = new bstm_multi_block(mdata);

  /* Set up bstm blocks */
  bstm_block_metadata bstm_mdata(bstm_block_id(),
                                 vgl_point_3d<double>(0, 0, 0),
                                 0,
                                 vgl_vector_3d<double>(.1, .1, .1),
                                 .1,
                                 vgl_vector_3d<unsigned>(10, 10, 10),
                                 10,
                                 1,
                                 4,
                                 9000,
                                 .01);

  int num_space_trees = 10 * 10 * 10;
  // 10 time intervals, and each space tree will have 8 leaves (and one root
  // node)
  int num_time_trees = num_space_trees * 10 * 9;
  // each time tree will have two leaves
  int num_data_elements = num_time_trees * 2;

  char *bstm_buff = new char[num_space_trees * space_tree_size]();
  bstm_block *bstm_blk = new bstm_block(bstm_mdata.id(), bstm_mdata, bstm_buff);
  bstm_time_block *bstm_blk_t =
      new bstm_time_block(bstm_block_id(), bstm_mdata, num_time_trees);

  /* Set up data buffers */
  vcl_size_t alpha_size = bstm_data_traits<BSTM_ALPHA>::datasize();
  vcl_size_t app_size = bstm_data_traits<BSTM_MOG3_GREY>::datasize();
  bstm_data_base *alpha =
      new bstm_data_base(num_data_elements, "alpha", bstm_mdata.id());
  block_data_base *alpha_new = new block_data_base(0);
  vcl_string appearance_type = bstm_data_traits<BSTM_MOG3_GREY>::prefix();
  bstm_data_base *appearance =
      new bstm_data_base(num_data_elements, appearance_type, bstm_mdata.id());
  block_data_base *appearance_new = new block_data_base(0);

  /* Initialize data buffers */
  boxm2_array_3d<space_tree_b> &trees = bstm_blk->trees();
  array_4d<int> cell_indices_4d(VXL_NULLPTR, 10 * 8, 10 * 8, 10 * 8, 10 * 32);
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      for (int k = 0; k < 10; ++k) {
        int idx = trees.linear_index(i, j, k);
        boct_bit_tree current_tree(trees(i, j, k));
        current_tree.set_bit_at(0, true);
        current_tree.set_data_ptr(idx * 9);
        vcl_vector<int> space_cells = current_tree.get_cell_bits();
        assert(space_cells.size() == 9);

        // Iterate over cells of space tree
        for (int cell_idx = 0; cell_idx < space_cells.size(); ++cell_idx) {
          int space_data_index =
              current_tree.get_data_index(space_cells[cell_idx]);

          // calc global coordinates of this leaf
          generic_tree gen_tree(current_tree);
          index_4d local_coords =
              gen_tree.local_voxel_coords(space_cells[cell_idx]);
          index_4d child_coords =
              element_product(index_4d(i, j, k, 0), gen_tree.dimensions()) +
              local_coords;

          boxm2_array_1d<time_tree_b> time_trees =
              bstm_blk_t->get_cell_all_tt(space_data_index);
          assert(time_trees.size() == 10);
          for (int t = 0; t < 10; ++t) {
            child_coords[3] = t * 32;
            vcl_size_t row_major_index =
                cell_indices_4d.index_from_coords(child_coords);

            // two data elements for each time tree
            int tt_idx = ((idx * 9 + cell_idx) * 10 + t);
            // there might be an overflow issue here :-(
            assert(tt_idx >= 0);
            assert(tt_idx * 2 >= 0);

            bstm_time_tree time_tree(time_trees[t]);
            time_tree.set_data_ptr(tt_idx * 2);
            time_tree.set_bit_at(0, true);

            vcl_vector<int> time_leaves = time_tree.get_leaf_bits();
            assert(time_leaves.size() == 2);
            for (int tl = 0; tl < time_leaves.size(); ++tl) {
              row_major_index += 16 * tl;
              vcl_size_t tt_data_index =
                  time_tree.get_data_index(time_leaves[tl]);
              reinterpret_cast<bstm_data_traits<BSTM_ALPHA>::datatype &>(
                  alpha->data_buffer()[tt_data_index * alpha_size]) =
                  row_major_index;
              reinterpret_cast<vcl_size_t &>(
                  appearance->data_buffer()[tt_data_index * app_size]) =
                  row_major_index;
            }
          }
        }
      }
    }
  }

  convert_bstm_space_trees(blk,
                           bstm_blk,
                           bstm_blk_t,
                           1,
                           alpha,
                           alpha_new,
                           appearance,
                           appearance_new,
                           appearance_type);

  bstm_data<BSTM_ALPHA> alpha_wrap(*alpha);
  block_data<BSTM_ALPHA> alpha_new_wrap(*alpha_new);
  bstm_data<BSTM_MOG3_GREY> appearance_wrap(*appearance);
  block_data<BSTM_MOG3_GREY> appearance_new_wrap(*appearance_new);

  array_4d<space_tree_b> multi_bstm_space_trees(
      reinterpret_cast<space_tree_b *>(&(blk->get_buffer(0)[0])),
      10,
      10,
      10,
      10);
  array_4d<time_tree_b> multi_bstm_time_trees(
      reinterpret_cast<time_tree_b *>(&(blk->get_buffer(1)[0])),
      10,
      10,
      10,
      90);
  bool space_idxs_good = true, time_idxs_good = true, data_layout_good = true;
  for (int idx = 0; idx < multi_bstm_space_trees.size(); ++idx) {
    index_4d space_tree_coords = multi_bstm_space_trees.coords_from_index(idx);
    boct_bit_tree current_space_tree(multi_bstm_space_trees[idx]);
    generic_tree gen_tree(current_space_tree);
    // perhaps a better test would be to have several time trees per
    // space tree, e.g. if space trees were more refined.
    space_idxs_good &= (current_space_tree.get_data_ptr() == idx * 9);
    for (int i = 0; i < 9; ++i) {
      bstm_time_tree current_time_tree(multi_bstm_time_trees[idx * 9 + i]);
      time_idxs_good &= (current_time_tree.get_data_ptr() == (idx * 9 + i) * 2);

      index_4d local_coords = gen_tree.local_voxel_coords(i);
      index_4d child_coords =
          element_product(space_tree_coords, gen_tree.dimensions()) +
          local_coords;
      child_coords[3] *= 32;

      // data value is the row-major idx of the current voxel
      vcl_size_t data_value = cell_indices_4d.index_from_coords(child_coords);
      for (int di = 0; di < 2; ++di) {
        data_layout_good &=
            (alpha_new_wrap[current_time_tree.get_data_ptr() + di] ==
             data_value + di * 16);
        data_layout_good &=
            (reinterpret_cast<vcl_size_t &>(
                 appearance_new_wrap[current_time_tree.get_data_ptr() + di]) ==
             data_value + di * 16);
      }
    }
  }
  TEST("convert_bstm_space_trees -- space tree data ptrs",
       space_idxs_good,
       true);
  TEST(
      "convert_bstm_space_trees -- time trees data ptrs", time_idxs_good, true);
  TEST("convert_bstm_space_trees -- data elements layout",
       data_layout_good,
       true);
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
    delete[] child_level_buffer;
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

    delete[] child_level_buffer;
  }
}

void test_bstm_to_multi_bstm_block_function() {
  // test implementation sub-routines...
  test_volume();
  test_get_bstm_data_buffers();
  test_convert_bstm_space_trees();
  // test_time_differences_from_bstm_trees();
  test_make_unrefined_space_tree();
  test_make_unrefined_time_tree();
  // test_compute_trees_structure();
  // test_coalesce_trees();

  // test empty scene
  // test actual BSTM scene
}

TESTMAIN(test_bstm_to_multi_bstm_block_function);
