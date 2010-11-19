//: test utils to produce test blocks, data, scene, etc.
#include "test_utils.h"
#include <vnl/vnl_vector_fixed.h>

char* boxm2_test_utils::construct_block_test_stream()
{
    typedef vnl_vector_fixed<unsigned char, 16> uchar16; 
    typedef vnl_vector_fixed<unsigned short, 2> ushort2; 
    typedef unsigned short                      ushort; 
  
    //write size, init_level, max_level, max_mb
    int numBuffers = 50;      
    int treeLen = 1200; 
    int nums[4] = { 30, 40, 50, 0 }; 
    double dims[4] = { 0.2, 0.4, 0.6, 0.0 };  
    int numTrees = nums[0]*nums[1]*nums[2];
    long size = numTrees*(sizeof(int) + sizeof(uchar16)) +
                numBuffers*(sizeof(ushort) + sizeof(ushort2)) +
                sizeof(long) + 3*sizeof(int) + 4*sizeof(double) + 6*sizeof(int);
    
    //1. construct a dummy block byte stream manually
    char* bsize = new char[size]; 
    for(int i=0; i<size; i++) bsize[i] = (char) 0; 
    int curr_byte = 0;
                
    int init_level = 1;  
    int max_level  = 4; 
    int max_mb     = 400; 
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
      treeCountBuff[i] = (ushort) 1200; 
    curr_byte += sizeof(ushort) * numBuffers; 
    
    //7. 1d array of mem pointers
    ushort2* memPtrsBuff = (ushort2*) (bsize + curr_byte);
    for(int i=0; i<numBuffers; i++) {
      memPtrsBuff[i][0] = 0; 
      memPtrsBuff[i][1] = 1201; 
    }
    curr_byte += sizeof(ushort2) * numBuffers;

    if(curr_byte != size) 
      vcl_cerr<<"size "<<size<<" doesn't match offset "<<curr_byte<<vcl_endl;

    return bsize;
}
