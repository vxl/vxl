//: test utils to produce test blocks, data, scene, etc.
#include "test_utils.h"


char* boxm2_test_utils::construct_block_test_stream(int numBuffers, 
                                                    int treeLen, 
                                                    int* nums, 
                                                    double* dims, 
                                                    int init_level,
                                                    int max_level,
                                                    int max_mb )
{
    typedef vnl_vector_fixed<unsigned char, 16> uchar16; 
    typedef vnl_vector_fixed<unsigned short, 2> ushort2; 
    typedef unsigned short                      ushort; 
  
    //write size, init_level, max_level, max_mb
    int numTrees = nums[0]*nums[1]*nums[2];
    long size = numTrees*(sizeof(int) + sizeof(uchar16)) +
                numBuffers*(sizeof(ushort) + sizeof(ushort2)) +
                sizeof(long) + 3*sizeof(int) + 4*sizeof(double) + 6*sizeof(int);
    
    //1. construct a dummy block byte stream manually
    char* bsize = new char[size]; 
    for(int i=0; i<size; i++) bsize[i] = (char) 0; 
    int curr_byte = 0;
                
    //write size, init_level, max_level, max_mb
    vcl_memcpy(bsize,   &size, sizeof(long));  
    curr_byte += sizeof(long); 
    vcl_memcpy(bsize+curr_byte, &init_level, sizeof(int)); 
    curr_byte += sizeof(int); 
    vcl_memcpy(bsize+curr_byte, &max_level, sizeof(int));
    curr_byte += sizeof(int); 
    vcl_memcpy(bsize+curr_byte, &max_mb, sizeof(int));
    curr_byte += sizeof(int); 
    
    //write dimension and buffer shape
    vcl_memcpy(bsize+curr_byte, dims, 4 * sizeof(double));
    curr_byte += 4 * sizeof(double);
    vcl_memcpy(bsize+curr_byte, nums, 4 * sizeof(int));
    curr_byte += 4 * sizeof(int);
    
    //3.  write number of buffers
    vcl_memcpy(bsize+curr_byte, &numBuffers, sizeof(numBuffers));
    curr_byte += sizeof(numBuffers);
    
    //3.a write length of tree buffers
    vcl_memcpy(bsize+curr_byte, &treeLen, sizeof(treeLen));
    curr_byte += sizeof(treeLen);

    //put some tree values in there
    //write in teh buffer some values for the trees (each tree gets a 1 as the root)
    for(int i=0; i<numTrees; i++) 
      bsize[curr_byte + 16*i] = (unsigned char) 1; 
    curr_byte += sizeof(uchar16)*numTrees;
     
    //5. 2d array of tree pointers
    int* treePtrsBuff = (int*) (bsize+curr_byte);
    for(int i=0; i<numTrees; i++) 
      treePtrsBuff[i] = i; 
    curr_byte += sizeof(int) * numTrees; 

    //fill in some blocks in buffers numbers
    ushort* treeCountBuff = (ushort*) (bsize + curr_byte); 
    for(int i=0; i<numBuffers; i++) 
      treeCountBuff[i] = (ushort) treeLen; 
    curr_byte += sizeof(ushort) * numBuffers; 
    
    //7. 1d array of mem pointers
    ushort2* memPtrsBuff = (ushort2*) (bsize + curr_byte);
    for(int i=0; i<numBuffers; i++) {
      memPtrsBuff[i][0] = 0; 
      memPtrsBuff[i][1] = treeLen+1; 
    }
    curr_byte += sizeof(ushort2) * numBuffers;

    if(curr_byte != size) 
      vcl_cerr<<"size "<<size<<" doesn't match offset "<<curr_byte<<vcl_endl;

    return bsize;
}


void boxm2_test_utils::test_block_equivalence(boxm2_block& a, boxm2_block& b)
{
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    typedef vnl_vector_fixed<unsigned short, 2> ushort2;
    typedef unsigned short                      ushort;

    //: omitting ID for sake of ASIO testing
    //if (a.block_id() != b.block_id()) {
        //TEST("boxm2_block: id failed", true, false);
        //return;
    //}
    if (a.num_buffers() != b.num_buffers()) {
        TEST("boxm2_block: num buffers failed", true, false);
        return;
    }
    if (a.tree_buff_length() != b.tree_buff_length()) {
        TEST("boxm2_block: tree_buff_length failed", true, false);
        return;
    }
    if (a.init_level() != b.init_level()) {
      TEST("boxm2_block: init_level failed", true, false);
      return;
    }
    if (a.max_level() != b.max_level()) {
        TEST("boxm2_block: max_level failed", true, false);
        return;
    }
    if (a.max_mb() != b.max_mb()) {
        TEST("boxm2_block: max_mb failed", true, false);
        return;
    }
    if (a.sub_block_dim() != b.sub_block_dim()) {
        TEST("boxm2_block: sub_block_dim failed", true, false);
        return;
    }
    if (a.sub_block_num() != b.sub_block_num()) {
        TEST("boxm2_block: sub_block_num failed", true, false);
        return;
    }
    TEST("boxm2_block: meta data from disk passed", true, true);

    boxm2_array_3d<uchar16>&  treesa = a.trees();
    boxm2_array_3d<uchar16>&  treesb = b.trees();
    for (int i=0; i<a.sub_block_num().x(); i++) {
      for (int j=0; j<a.sub_block_num().y(); j++) {
        for (int k=0; k<a.sub_block_num().z(); k++) {
          if (treesa[i][j][k] != treesb[i][j][k]) {
            TEST("boxm2_block: trees not initialized properly", true, false);
            return;
          }
        }
      }
    }
    TEST("boxm2_block: trees initialized properly", true, true);

    boxm2_array_2d<int>& treePtrsA = a.tree_ptrs();
    boxm2_array_2d<int>& treePtrsB = b.tree_ptrs();
    for (int i=0; i<a.num_buffers(); i++) {
      for (int j=0; j<a.tree_buff_length(); j++) {
        if (treePtrsA[i][j] != treePtrsB[i][j]) {
          TEST("boxm2_block: trees ptrs not initialized properly", true, false);
          return;
        }
      }
    }
    TEST("boxm2_block: tree ptrs initialized properly", true, true);

    boxm2_array_1d<ushort> b_in_ba = a.trees_in_buffers();
    boxm2_array_1d<ushort> b_in_bb = b.trees_in_buffers();
    for (int i=0; i<a.num_buffers(); i++) {
      if (b_in_ba[i] != b_in_bb[i]) {
        TEST("boxm2_block: blocks in buffers not initialized properly", true, false);
        return;
      }
    }
    TEST("boxm2_block: blocks in buffers initialized properly", true, true);

    boxm2_array_1d<ushort2> memPtrsA = a.mem_ptrs();
    boxm2_array_1d<ushort2> memPtrsB = b.mem_ptrs();
    for (int i=0; i<a.num_buffers(); i++) {
      if (memPtrsA[i] != memPtrsB[i]) {
        TEST("boxm2_block: mem_ptrs not initialized properly", true, false);
        return;
      }
    }
    TEST("boxm2_block: mem_ptrs initialized properly", true, true);
}


