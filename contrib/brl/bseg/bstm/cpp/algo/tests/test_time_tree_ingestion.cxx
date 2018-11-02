#include <iostream>
#include <fstream>
#include <vector>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <bstm/bstm_time_tree.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif



#define DATA_LEN 80

void ingest(bstm_time_tree& tree, char*  tree_data ,float time, char data)
{
  bstm_time_tree old_tree(tree);

  //first check curr data is same as ingested data
  int offset = tree.get_relative_index( tree.traverse(time));
  if (data != tree_data[offset])
  {
    bool split_complete = false;
    while (!split_complete)
    {
      int curr_cell = tree.traverse(time);
      int currDepth = tree.depth_at(curr_cell);

      float cell_min,cell_max;
      tree.cell_range(curr_cell, cell_min,cell_max);
      if (cell_min == time) //found cell starting at queried time.
      {
        split_complete = true;
      }
      else if (currDepth < 5)
      {
        tree.set_bit_at(curr_cell,true);
      }
      else //reached end of tree...
        split_complete = true;
    }

    //move data
    int newSize = tree.num_cells();
    char new_data[DATA_LEN];

    int cellsMoved = 0;
    int oldDataPtr = 0;
    int newDataPtr = 0;
    int newInitCount = 0;
    for (int j=0; j < 63 && cellsMoved<newSize; ++j)
    {
      //--------------------------------------------------------------------
      //4 Cases:
      // - Old cell and new cell exist - transfer data over
      // - new cell exists, old cell doesn't - create new occupancy based on depth
      // - old cell exists, new cell doesn't - uh oh this is bad news
      // - neither cell exists - do nothing and carry on
      //--------------------------------------------------------------------
      //if parent bit is 1, then you're a valid cell
      int pj = old_tree.parent_index(j);           //Bit_index of parent bit
      bool validCellOld = (j==0) || old_tree.bit_at(pj);
      bool validCellNew = (j==0) || tree.bit_at(pj);
      if (validCellOld && validCellNew) {
        //move root data to new location
        new_data[newDataPtr]  = tree_data[oldDataPtr];

        //increment
        ++oldDataPtr;
        ++newDataPtr;
        ++cellsMoved;
      }
      //case where it's a new leaf...
      else if (validCellNew) {
        //find parent in old tree
        int valid_parent_bit = pj;
        while ( valid_parent_bit !=0 && !old_tree.bit_at( old_tree.parent_index(valid_parent_bit) ) )
          valid_parent_bit = old_tree.parent_index(valid_parent_bit);

        new_data[newDataPtr]  = tree_data[ old_tree.get_relative_index(valid_parent_bit) ];

        //update new data pointer
        ++newDataPtr;
        ++newInitCount;
        ++cellsMoved;
      }
    }
    //write new data in
    new_data[ tree.get_relative_index( tree.traverse(time)) ] = data;
    std::cout << "Placing " << data << " to " << tree.get_relative_index( tree.traverse(time))  << std::endl;
    for (int i = 0; i < DATA_LEN; tree_data[i] = new_data[i], ++i); //copy new data into old data
  }
}

void test_time_tree_ingestion()
{
  char data[] = {'a','a','b','c',
                 'd','d','e','d',
                 'a','a','a','a',
                 'b','a','a','b',
                 'a','a','b','c',
                 'g','h','g','h',
                 'f','f','f','f',
                 'f','f','f','f'}; //DATA_LEN
  char*  tree_data = new char[DATA_LEN];
  for (int i = 0; i < DATA_LEN; ++i) tree_data[i] = '0';

  bstm_time_tree tree;

  for (int i = 0; i < 32; ++i) //loop over each data item
      ingest(tree, tree_data, (float)i /32, data[i]);

  std::vector<int> leaf_bits = tree.get_leaf_bits(0);
  for (int leaf_bit : leaf_bits)
    std::cout << "Data at leaf " << leaf_bit << " is " << tree_data[tree.get_relative_index(leaf_bit)] << std::endl;

  bool good = tree.bit_at(0)  == 1
           && tree.bit_at(1)  == 1
           && tree.bit_at(2)  == 1
           && tree.bit_at(3)  == 1
           && tree.bit_at(4)  == 1
           && tree.bit_at(5)  == 1
           && tree.bit_at(7)  == 1
           && tree.bit_at(8)  == 1
           && tree.bit_at(9)  == 0
           && tree.bit_at(10) == 1
           && tree.bit_at(11) == 1
           && tree.bit_at(12) == 1
           && tree.bit_at(15) == 0
           && tree.bit_at(16) == 1
           && tree.bit_at(17) == 0
           && tree.bit_at(18) == 1
           && tree.bit_at(21) == 1
           && tree.bit_at(22) == 1
           && tree.bit_at(23) == 0
           && tree.bit_at(24) == 1
           && tree.bit_at(25) == 1
           && tree.bit_at(26) == 1;
  TEST("Tree structure  ", true, good);
}

TESTMAIN(test_time_tree_ingestion);
