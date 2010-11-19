//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "test_utils.h"

#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_block_id.h>
#include <boxm2/boxm2_sio_mgr.h>
#include <vnl/vnl_vector_fixed.h>

#include <vcl_iostream.h>


void test_block_equivalence(boxm2_block& a, boxm2_block& b)
{
    typedef vnl_vector_fixed<unsigned char, 16> uchar16; 
    typedef vnl_vector_fixed<unsigned short, 2> ushort2; 
    typedef unsigned short                      ushort; 
  
    if(a.block_id() != b.block_id()) {
      TEST("boxm2_block: id failed", true, false); 
      return; 
    }
    if(a.num_buffers() != b.num_buffers()) {
      TEST("boxm2_block: num buffers failed", true, false); 
      return; 
    }
    if(a.tree_buff_length() != b.tree_buff_length()) {
      TEST("boxm2_block: tree_buff_length failed", true, false); 
      return; 
    }
    if(a.init_level() != b.init_level()) {
      TEST("boxm2_block: init_level failed", true, false); 
      return; 
    }
    if(a.max_level() != b.max_level()) {
      TEST("boxm2_block: max_level failed", true, false); 
      return; 
    }
    if(a.max_mb() != b.max_mb()) {
      TEST("boxm2_block: max_mb failed", true, false); 
      return; 
    }
    if(a.sub_block_dim() != b.sub_block_dim()) {
      TEST("boxm2_block: sub_block_dim failed", true, false); 
      return; 
    }
    if(a.sub_block_num() != b.sub_block_num()) {
      TEST("boxm2_block: sub_block_num failed", true, false); 
      return; 
    }
    TEST("boxm2_block: meta data from disk passed", true, true);  
    
    boxm2_array_3d<uchar16>&  treesa = a.trees();
    boxm2_array_3d<uchar16>&  treesb = b.trees();
    for(int i=0; i<a.sub_block_dim().x(); i++) {
      for(int j=0; j<a.sub_block_dim().y(); j++) {
        for(int k=0; k<a.sub_block_dim().z(); k++) {
          if(treesa[i][j][k] != treesb[i][j][k]) {
            TEST("boxm2_block: trees not initialized properly", true, false);
            return;
          }
        }
      }
    }
    TEST("boxm2_block: trees initialized properly", true, true);
    
    boxm2_array_2d<int>& treePtrsA = a.tree_ptrs(); 
    boxm2_array_2d<int>& treePtrsB = b.tree_ptrs(); 
    for(int i=0; i<a.num_buffers(); i++) {
      for(int j=0; j<a.tree_buff_length(); j++) {
        if(treePtrsA[i][j] != treePtrsB[i][j]) {
          TEST("boxm2_block: trees ptrs not initialized properly", true, false);
          return;
        }
      }
    }
    TEST("boxm2_block: tree ptrs initialized properly", true, true);

    boxm2_array_1d<ushort> b_in_ba = a.trees_in_buffers();
    boxm2_array_1d<ushort> b_in_bb = b.trees_in_buffers();
    for(int i=0; i<a.num_buffers(); i++) {
      if(b_in_ba[i] != b_in_bb[i]) {
        TEST("boxm2_block: blocks in buffers not initialized properly", true, false);
        return;
      }
    }
    TEST("boxm2_block: blocks in buffers initialized properly", true, true);

    boxm2_array_1d<ushort2> memPtrsA = a.mem_ptrs();
    boxm2_array_1d<ushort2> memPtrsB = b.mem_ptrs();
    for(int i=0; i<a.num_buffers(); i++) {
      if(memPtrsA[i] != memPtrsB[i]) {
        TEST("boxm2_block: mem_ptrs not initialized properly", true, false);
        return;
      }
    }
    TEST("boxm2_block: mem_ptrs initialized properly", true, true);
}

void test_block()
{
    typedef vnl_vector_fixed<unsigned char, 16> uchar16; 
    typedef vnl_vector_fixed<unsigned short, 2> ushort2; 
    typedef unsigned short                      ushort; 
    
    //test xml file 
    vcl_string root_dir  = testlib_root_dir();
    vcl_string test_file = root_dir + "/contrib/brl/bseg/boxm2/tests/"; 
  
    //construct block from stream
    char* stream = boxm2_test_utils::construct_block_test_stream(); 
    vcl_cout<<"returned stream"<<vcl_endl;
    boxm2_block test_block(boxm2_block_id(0,0,0), stream); 
    vcl_cout<<"constructed test block "<<vcl_endl;
    boxm2_sio_mgr::save_block(test_file, &test_block);
    vcl_cout<<"saved block "<<vcl_endl;
  
    //make sure all the meta data matches... 
    int numBuffers = 50;      
    int treeLen = 1200; 
    int init_level = 1;  
    int max_level  = 4; 
    int max_mb     = 400; 
    int nums[4] = { 30, 40, 50, 0 }; 
    double dims[4] = { 0.2, 0.4, 0.6, 0.0 };  
    int numTrees = nums[0]*nums[1]*nums[2];
    if(test_block.block_id() != boxm2_block_id(0,0,0)) {
      TEST("boxm2_block: id failed", true, false); 
      return; 
    }
    if(test_block.num_buffers() != numBuffers) {
      TEST("boxm2_block: num buffers failed", true, false); 
      return; 
    }
    if(test_block.tree_buff_length() != treeLen) {
      TEST("boxm2_block: tree_buff_length failed", true, false); 
      return; 
    }
    if(test_block.init_level() != init_level) {
      TEST("boxm2_block: init_level failed", true, false); 
      return; 
    }
    if(test_block.max_level() != max_level) {
      TEST("boxm2_block: max_level failed", true, false); 
      return; 
    }
    if(test_block.max_mb() != max_mb) {
      TEST("boxm2_block: max_mb failed", true, false); 
      return; 
    }
    if(test_block.sub_block_dim() != vgl_vector_3d<double>(dims[0], dims[1], dims[2])) {
      TEST("boxm2_block: sub_block_dim failed", true, false); 
      return; 
    }
    if(test_block.sub_block_num() != vgl_vector_3d<int>(nums[0], nums[1], nums[2])) {
      TEST("boxm2_block: sub_block_num failed", true, false); 
      return; 
    }
    TEST("boxm2_block: meta data passed", true, true);
    
    boxm2_array_3d<uchar16>&  trees = test_block.trees();
    uchar16 comp((unsigned char) 0); comp[0] = 1; 
    for(int i=0; i<dims[0]; i++) {
      for(int j=0; j<dims[1]; j++) {
        for(int k=0; k<dims[2]; k++) {
          if(trees[i][j][k] != comp) {
            TEST("boxm2_block: trees not initialized properly", true, false);
            return;
          }
        }
      }
    }
    TEST("boxm2_block: trees initialized properly", true, true);
    
    boxm2_array_2d<int>& treePtrs = test_block.tree_ptrs(); 
    int index = 0; 
    for(int i=0; i<numBuffers; i++) {
      for(int j=0; j<treeLen; j++) {
        if(treePtrs[i][j] != index) {
          TEST("boxm2_block: trees ptrs not initialized properly", true, false);
          return;
        }
        index++;
      }
    }
    TEST("boxm2_block: tree ptrs initialized properly", true, true);

    boxm2_array_1d<ushort> b_in_b = test_block.trees_in_buffers();
    for(int i=0; i<numBuffers; i++) {
      if(b_in_b[i] != treeLen) {
        TEST("boxm2_block: blocks in buffers not initialized properly", true, false);
        return;
      }
    }
    TEST("boxm2_block: blocks in buffers initialized properly", true, true);

    boxm2_array_1d<ushort2> memPtrs = test_block.mem_ptrs();
    ushort2 memInit((ushort) 0); memInit[1] = 1201; 
    for(int i=0; i<numBuffers; i++) {
      if(memPtrs[i] != memInit) {
        TEST("boxm2_block: mem_ptrs not initialized properly", true, false);
        return;
      }
    }
    TEST("boxm2_block: mem_ptrs initialized properly", true, true);

    ////2. read in sub block dimension, sub block num
    boxm2_block* loaded = boxm2_sio_mgr::load_block(test_file, boxm2_block_id(0,0,0)); 
    test_block_equivalence(test_block, *loaded);

    //clean up boxm2_block
    delete loaded; 
    
    //clean up file left behind "tests/block_id.0.0.0.bin"
}


TESTMAIN( test_block );
