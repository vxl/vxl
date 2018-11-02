//:
// \file
// \author Ali Osman Ulusoy
// \date 05-Aug-2012
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <bstm/bstm_time_tree.h>
#include <vnl/vnl_vector_fixed.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


void test_time_tree()
{
    //-------------------------------------------------------------
    std::cout << "Initializing empty tree... " << std::endl;
    auto* array = new unsigned char[8];
    for(int i = 0; i < 8; i++)
      array[i] = 0;
    bstm_time_tree empty_tree(array,6);

    std::cout << "Max number of cells: " << empty_tree.max_num_cells() << std::endl;
    std::cout << "Max number of inner cells: " << empty_tree.max_num_inner_cells() << std::endl;
    for(int i = 0; i < 63; i++)
      std::cout << i << " depth: " << empty_tree.depth_at(i) << " parent: "
                << empty_tree.parent_index(i) << " left most child: " << empty_tree.child_index(i) << " cell_len: " << empty_tree.cell_len(i) * bstm_time_tree::tree_range()  << " cell_center: " << empty_tree.cell_center(i) << std::endl;

    //-------------------------------------------------------------
    //Test get bit_at functionality
    unsigned char bits[31] = {  //{0} depth 0
                                1,
                                //{1-2}  depth 1
                                1,0,
                                //{3-6}  depth 2
                                1,1,0,0,
                                //{7-14} depth 3
                                0,0,1,1,0,0,0,0,
                                //{15-30} depth 4
                                0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0
                                  };
    unsigned char test_tree[8] = {216,96,18,0,0,0,0,0};
    bstm_time_tree tree(test_tree,6);
    bool good = true;
    for (int i=0; i<31; i++) {
      good = good && (tree.bit_at(i) == bits[i]);
      if (!good)
        std::cout<<i << " ";
    }
    TEST("Easy test, get bits works ", true, good);


    unsigned char bits2[31] = {  //{0} depth 0
                                1,
                                //{1-2}  depth 1
                                1,1,
                                //{3-6}  depth 2
                                1,0,0,1,
                                //{7-14} depth 3
                                1,1,0,0,0,0,0,1,
                                //{15-30} depth 4
                                0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0};

    unsigned char test_tree2[8] = {243,130,160,4,0,0,0};
    bstm_time_tree tree2(test_tree2,6);
    good = true;
    for (int i=0; i<31; i++) {
      good = good && (tree2.bit_at(i) == bits2[i]);
      if (!good)
        std::cout<<i << " ";
    }
    TEST("Hard case, get bits works ", true, good);


    //-------------------------------------------------------------
    //Test set bit_at functionality
    good = true;
    for (int i=0; i<31; i++) {
      unsigned char tmp = tree.bit_at(i);
      tree.set_bit_at(i,tmp);
      good = good && (tree.bit_at(i) == bits[i]);
      if (!good)
        std::cout<<i << " ";
    }
    TEST("Easy case, set bits works ", true, good);

    //copy tree
    bstm_time_tree tree2_copy(tree2);

    good = true;
    for (int i=0; i<31; i++) {
      unsigned char tmp = tree2.bit_at(i);
      tree2.set_bit_at(i,(tmp) ? false : true);
      good = good && (tree2.bit_at(i) != bits2[i]);
      if (!good)
        std::cout<<i << " " ;
    }
    TEST("Harder case, set bits works ", true, good);


    //-------------------------------------------------------------
    //Test data_ptr functionality

    good = true;
    good = good && (tree.get_relative_index(7)  == 0);
    good = good && (tree.get_relative_index(8)  == 1);
    good = good && (tree.get_relative_index(39)  == 2);
    good = good && (tree.get_relative_index(40)  == 3);
    good = good && (tree.get_relative_index(20)  == 4);
    good = good && (tree.get_relative_index(21)  == 5);
    good = good && (tree.get_relative_index(45)  == 6);
    good = good && (tree.get_relative_index(46)  == 7);
    good = good && (tree.get_relative_index(2)  == 8);
    TEST("Another easy case, Get relative data ptr works ", true, good);



    good = true;
    good = good && (tree2_copy.get_relative_index(15)  == 0);
    good = good && (tree2_copy.get_relative_index(33)  == 1);
    good = good && (tree2_copy.get_relative_index(34)  == 2);
    good = good && (tree2_copy.get_relative_index(17)  == 3);
    good = good && (tree2_copy.get_relative_index(37)  == 4);
    good = good && (tree2_copy.get_relative_index(38)  == 5);
    good = good && (tree2_copy.get_relative_index(4)  == 6);
    good = good && (tree2_copy.get_relative_index(5)  == 7);
    good = good && (tree2_copy.get_relative_index(13)  == 8);
    good = good && (tree2_copy.get_relative_index(59)  == 9);
    good = good && (tree2_copy.get_relative_index(60)  == 10);
    good = good && (tree2_copy.get_relative_index(30)  == 11);
    TEST("Harder case, Get relative data ptr works ", true, good);


    //-------------------------------------------------------------
    //Test traverse functionality
    good = true;
    good = good && (tree2_copy.traverse(0 / bstm_time_tree::tree_range())  == 15);
    good = good && (tree2_copy.traverse(1 / bstm_time_tree::tree_range())  == 15);
    good = good && (tree2_copy.traverse(2 / bstm_time_tree::tree_range())  == 33);
    good = good && (tree2_copy.traverse(3 / bstm_time_tree::tree_range())  == 34);
    good = good && (tree2_copy.traverse(4 / bstm_time_tree::tree_range())  == 17);
    good = good && (tree2_copy.traverse(5 / bstm_time_tree::tree_range())  == 17);
    good = good && (tree2_copy.traverse(6 / bstm_time_tree::tree_range())  == 37);
    good = good && (tree2_copy.traverse(7 / bstm_time_tree::tree_range())  == 38);
    good = good && (tree2_copy.traverse(8 / bstm_time_tree::tree_range())  == 4);
    good = good && (tree2_copy.traverse(10 / bstm_time_tree::tree_range()) == 4);
    good = good && (tree2_copy.traverse(16 / bstm_time_tree::tree_range()) == 5);
    good = good && (tree2_copy.traverse(23 / bstm_time_tree::tree_range()) == 5);
    good = good && (tree2_copy.traverse(24 / bstm_time_tree::tree_range()) == 13);
    good = good && (tree2_copy.traverse(27 / bstm_time_tree::tree_range()) == 13);
    good = good && (tree2_copy.traverse(31 / bstm_time_tree::tree_range()) == 30);
    good = good && (tree2_copy.traverse(29 / bstm_time_tree::tree_range()) == 60);
    good = good && (tree2_copy.traverse(28 / bstm_time_tree::tree_range()) == 59);
    TEST("Traversal works ", true, good);

    good = true;
    good = good && (empty_tree.num_cells() == 1);
    good = good && (tree2_copy.num_cells() == 23);
    good = good && (tree.num_cells() == 17);
    TEST("Num cells works ", true, good);


    std::vector<int> leaves  = tree.get_leaf_bits(0);
    good = true;
    good = good && (leaves[0] == 7);
    good = good && (leaves[1] == 8);
    good = good && (leaves[2] == 39);
    good = good && (leaves[3] == 40);
    good = good && (leaves[4] == 20);
    good = good && (leaves[5] == 21);
    good = good && (leaves[6] == 45);
    good = good && (leaves[7] == 46);
    good = good && (leaves[8] == 2);
    good = good && (leaves.size() == 9);
    TEST("Easy case, get leaves in pre-order traversal works ", true, good);

    leaves  = tree2_copy.get_leaf_bits(0);
    good = true;
    good = good && (leaves[0] == 15);
    good = good && (leaves[1] == 33);
    good = good && (leaves[2] == 34);
    good = good && (leaves[3] == 17);
    good = good && (leaves[4] == 37);
    good = good && (leaves[5] == 38);
    good = good && (leaves[6] == 4);
    good = good && (leaves[7] == 5);
    good = good && (leaves[8] == 13);
    good = good && (leaves[9] == 59);
    good = good && (leaves[10] == 60);
    good = good && (leaves[11] == 30);
    good = good && (leaves.size() == 12);
    TEST("Harder case, get leaves in pre-order traversal works ", true, good);

    //  test fill cells
    {
      bool frames[32];
      std::memset(frames, false, 32);
      frames[7] = true;
      frames[8] = true;
      frames[9] = true;
      frames[10] = true;
      frames[11] = true;
      frames[12] = true;
      frames[13] = true;
      frames[14] = true;
      // frames[21] = true;
      bstm_time_tree tree;
      tree.fill_cells(frames);
      unsigned char bits[31] = {  //{0} depth 0
                                1,
                                //{1-2}  depth 1
                                1,0,
                                //{3-6}  depth 2
                                1,1,0,0,
                                //{7-14} depth 3
                                0,1,1,1,0,0,0,0,
                                //{15-30} depth 4
                                0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0};
      bool good = true;
      for(int i=0; i<=30; i++) {
        good &= (tree.bit_at(i) == bits[i]);
      }
      TEST("testing fill_cells, with 8 different frames in the middle", good, true);
    }
    {
      bool frames[32];
      std::memset(frames, false, 32);
      bstm_time_tree tree;
      tree.fill_cells(frames);
      unsigned char bits[31] = {0};

      bool good = true;
      for(int i=0; i<=30; i++) {
        good &= (tree.bit_at(i) == bits[i]);
      }
      TEST("testing fill_cells, all frames the same", good, true);
    }
    {
      bool frames[32];
      std::memset(frames, false, 32);
      frames[0] = true;
      bstm_time_tree tree;
      tree.fill_cells(frames);
      unsigned char bits[31] = {0};

      bool good = true;
      for(int i=0; i<=30; i++) {
        good &= (tree.bit_at(i) == bits[i]);
      }
      TEST("testing fill_cells, first frame is different (which actually means all frames are the same)", good, true);
    }
    {
      bool frames[32];
      std::memset(frames, false, 32);
      frames[1] = true;
      bstm_time_tree tree;
      tree.fill_cells(frames);
      unsigned char bits[31] = {  //{0} depth 0
                                1,
                                //{1-2}  depth 1
                                1,0,
                                //{3-6}  depth 2
                                1,0,0,0,
                                //{7-14} depth 3
                                1,0,0,0,0,0,0,0,
                                //{15-30} depth 4
                                1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

      bool good = true;
      for(int i=0; i<=30; i++) {
        good &= (tree.bit_at(i) == bits[i]);
      }
      TEST("testing fill_cells, second frame is different", good, true);
    }
    {
      bool frames[32];
      std::memset(frames, false, 32);
      frames[2] = true;
      bstm_time_tree tree;
      tree.fill_cells(frames);
      unsigned char bits[31] = {  //{0} depth 0
                                1,
                                //{1-2}  depth 1
                                1,0,
                                //{3-6}  depth 2
                                1,0,0,0,
                                //{7-14} depth 3
                                1,0,0,0,0,0,0,0,
                                //{15-30} depth 4
                                0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

      bool good = true;
      for(int i=0; i<=30; i++) {
        good &= (tree.bit_at(i) == bits[i]);
      }
      TEST("testing fill_cells, third frame is different", good, true);
    }
    {
      bool frames[32];
      std::memset(frames, false, 32);
      frames[8] = true;
      frames[16] = true;
      bstm_time_tree tree;
      tree.fill_cells(frames);
      unsigned char bits[31] = {  //{0} depth 0
                                1,
                                //{1-2}  depth 1
                                1,0,
                                //{3-6}  depth 2
                                0,0,0,0,
                                //{7-14} depth 3
                                0,0,0,0,0,0,0,0,
                                //{15-30} depth 4
                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

      bool good = true;
      for(int i=0; i<=30; i++) {
        good &= (tree.bit_at(i) == bits[i]);
      }
      TEST("testing fill_cells, middle eight frames are all one different frame", good, true);
    }
    {
      bool frames[32];
      std::memset(frames, false, 32);
      frames[10] = true;
      frames[21] = true;
      bstm_time_tree tree;
      tree.fill_cells(frames);
      unsigned char bits[31] = {  //{0} depth 0
                                1,
                                //{1-2}  depth 1
                                1,1,
                                //{3-6}  depth 2
                                0,1,1,0,
                                //{7-14} depth 3
                                0,0,1,0,0,1,0,0,
                                //{15-30} depth 4
                                0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0};

      bool good = true;
      for(int i=0; i<=30; i++) {
        good &= (tree.bit_at(i) == bits[i]);
      }
      TEST("testing fill_cells, frames 10 and 21 are different from predecessor", good, true);
    }
}


TESTMAIN( test_time_tree );
