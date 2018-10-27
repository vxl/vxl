//:
// \file
// \author Vishal Jain
// \date 26-Jan-2011
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <brdb/brdb_value.h>

#include <boxm2/cpp/algo/boxm2_merge_block_function.h>
#include <boct/boct_bit_tree.h>


void test_merge_bit_tree()
{
    //--------------------------------------------------------------------------
    //try a simple tree
    {
      boct_bit_tree btree;
      btree.set_bit_at(0, true);

      //setup alpha buffer
      float prob = .2f;
      float alph = -std::log(1.0f-prob);
      float alpha[] = {alph,
                       alph/2.0f, alph/2.0f, alph/2.0f, alph/2.0f,   //second gen
                       alph/2.0f, alph/2.0f, alph/2.0f, alph/2.0f };

      boxm2_scene_sptr scene = new boxm2_scene();

      //test single merge
      boxm2_merge_block_function merge_function(scene);
      boct_bit_tree merged = merge_function.merge_bit_tree(btree, alpha, 0.3f);

      for (int i=0; i<73; ++i) {
        if (merged.bit_at(i) != 0) {
          TEST("Merged tree merged root",true,false);
          return;
        }
      }
      TEST("Merged tree merged root", true, true);
    }

    //--------------------------------------------------------------------------
    //try slightly more complex tree
    {
      boct_bit_tree btree;
      btree.set_bit_at(0, true);   //should have 17 cells
      btree.set_bit_at(1, true);

      //setup alpha buffer
      float prob = .2f;
      float alph = -std::log(1.0f-prob);
      float alpha[17];
      for (float & i : alpha)
        i = alph/4.0f;

      boxm2_scene_sptr scene = new boxm2_scene();

      //test single merge
      boxm2_merge_block_function merge_function(scene);
      boct_bit_tree merged = merge_function.merge_bit_tree(btree, alpha, 0.3f);

      if (merged.bit_at(0) != 1) {
        TEST("Merge larger tree (merged root, shouldn't have)", true, false);
        return;
      }
      for (int i=1; i<73; ++i) {
        if (merged.bit_at(i) != 0) {
          TEST("Merge larger tree",true,false);
          return;
        }
      }
      TEST("Merge tree larger tree", true, true);
    }

    //--------------------------------------------------------------------------
    //finally test a tree that shouldn't merge (due to one cell)
    {
      boct_bit_tree btree;
      btree.set_bit_at(0, true);   //should have 17 cells
      btree.set_bit_at(1, true);

      //setup alpha buffer
      float prob = .2f;
      float alph = -std::log(1.0f-prob);
      float alpha[17];
      for (int i=0; i<17; ++i) {
        alpha[i] = alph/4.0f;
        if (i==14) alpha[i] = 8.0f*alph; //set one to be too big for .3
      }

      boxm2_scene_sptr scene = new boxm2_scene();

      //test single merge
      boxm2_merge_block_function merge_function(scene);
      boct_bit_tree merged = merge_function.merge_bit_tree(btree, alpha, 0.3f);

      if (merged.bit_at(0) == 1 && merged.bit_at(1) == 1)
        TEST("Merge fails correctly", true, true);
      else
        TEST("Merge merged too many cells!", true, false);
    }
}

void test_merge_function()
{
  test_merge_bit_tree();
}


TESTMAIN(test_merge_function);
