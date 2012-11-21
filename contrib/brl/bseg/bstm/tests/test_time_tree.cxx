//:
// \file
// \author Ali Osman Ulusoy
// \date 05-Aug-2012
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <bstm/bstm_time_tree.h>
#include <vnl/vnl_vector_fixed.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_algorithm.h>



void test_time_tree()
{
    //-------------------------------------------------------------
    vcl_cout << "Initializing empty tree... " << vcl_endl;
    unsigned char* array = new unsigned char[8];
    for(int i = 0; i < 8; i++)
      array[i] = 0;
    bstm_time_tree empty_tree(array,6);

    vcl_cout << "Max number of cells: " << empty_tree.max_num_cells() << vcl_endl;
    vcl_cout << "Max number of inner cells: " << empty_tree.max_num_inner_cells() << vcl_endl;
    for(int i = 0; i < 63; i++)
      vcl_cout << i << " depth: " << empty_tree.depth_at(i) << " parent: "
                << empty_tree.parent_index(i) << " left most child: " << empty_tree.child_index(i) << " cell_len: " << empty_tree.cell_len(i) * bstm_time_tree::tree_range()  << " cell_center: " << empty_tree.cell_center(i) << vcl_endl;

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
        vcl_cout<<i << " ";
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
        vcl_cout<<i << " ";
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
        vcl_cout<<i << " ";
    }
    TEST("Easy case, set bits works ", true, good);

    //copy tree
    bstm_time_tree tree2_copy(tree2);

    good = true;
    for (int i=0; i<31; i++) {
      unsigned char tmp = tree2.bit_at(i);
      tree2.set_bit_at(i,(tmp) ? 0 : 1);
      good = good && (tree2.bit_at(i) != bits2[i]);
      if (!good)
        vcl_cout<<i << " " ;
    }
    TEST("Harder case, set bits works ", true, good);


    //-------------------------------------------------------------
    //Test data_ptr functionality

    good = true;
    good = good && (empty_tree.get_relative_index(0)  == 0);
    TEST("Easy case, Get relative data ptr works ", true, good);

    good = true;
    good = good && (tree.get_relative_index(0)  == 0);
    good = good && (tree.get_relative_index(1)  == 1);
    good = good && (tree.get_relative_index(3)  == 3);
    good = good && (tree.get_relative_index(4)  == 4);
    good = good && (tree.get_relative_index(7)  == 5);
    good = good && (tree.get_relative_index(9)  == 7);
    good = good && (tree.get_relative_index(10)  == 8);
    good = good && (tree.get_relative_index(19)  == 9);
    good = good && (tree.get_relative_index(22)  == 12);
    TEST("Another easy case, Get relative data ptr works ", true, good);


    good = true;
    good = good && (tree2_copy.get_relative_index(0)  == 0);
    good = good && (tree2_copy.get_relative_index(1)  == 1);
    good = good && (tree2_copy.get_relative_index(2)  == 2);
    good = good && (tree2_copy.get_relative_index(3)  == 3);
    good = good && (tree2_copy.get_relative_index(5)  == 5);
    good = good && (tree2_copy.get_relative_index(13)  == 9);
    good = good && (tree2_copy.get_relative_index(14)  == 10);
    good = good && (tree2_copy.get_relative_index(15)  == 11);
    good = good && (tree2_copy.get_relative_index(16)  == 12);
    good = good && (tree2_copy.get_relative_index(18)  == 14);
    good = good && (tree2_copy.get_relative_index(29)  == 15);
    good = good && (tree2_copy.get_relative_index(33)  == 17);
    good = good && (tree2_copy.get_relative_index(34)  == 18);
    good = good && (tree2_copy.get_relative_index(59)  == 21);
    good = good && (tree2_copy.get_relative_index(60)  == 22);
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
    good = good && (tree2_copy.num_cells() == 23);
    good = good && (tree.num_cells() == 17);
    TEST("Num cells works ", true, good);


}


TESTMAIN( test_time_tree );
