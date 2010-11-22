#ifndef boxm2_test_utils_h_
#define boxm2_test_utils_h_
//:
// \file
#include <vcl_iostream.h>
#include <boxm2/boxm2_block.h>


class boxm2_test_utils
{
  public:
    //: creates a valid, though predictable block byte stream
    static char* construct_block_test_stream( int numBuffers, 
                                              int treeLen, 
                                              int* nums, 
                                              double* dims, 
                                              int init_level,
                                              int max_level,
                                              int max_mb );
    
    static void  test_block_equivalence(boxm2_block& a, boxm2_block& b);
    
};

#endif
