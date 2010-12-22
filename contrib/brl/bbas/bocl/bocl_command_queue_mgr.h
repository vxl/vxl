#ifndef bocl_command_queue_mgr_h
#define bocl_command_queue_mgr_h
//:
// \file
// \brief boxm2_opencl_processor is an abstraction for a single GPU (using opencl).
//  Because of memory restrictions on the GPU, the boxm2_opencl_processor must
//  also serve as a sort of "dumb cache" where only one block is allowed into
//  gpu memory at one time.
//  The GPU carries scene data
//    - Blocks (voxel structure)
//    - Data buffers (occupancy/appearance models)
//    - Block Metadata (origin, sub block dimensions/numbers)
//  It also stores a list of blocks to process (in a given order).
//  When run is called, it will run the same process on each set of blocks
//  (this may need to be pushed down to the processor)
#include <vcl_vector.h>

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_kernel.h>


//: boxm2_opencl_processor is a singleton bocl_manager as well.
class bocl_command_queue_mgr: public bocl_manager<bocl_command_queue_mgr>
{
 public:
    bocl_command_queue_mgr() {}
    ~bocl_command_queue_mgr(){}

    bool init_kernel(); 
    bool test_async_command_queue();

 protected:

    bocl_mem*  buffer_; 
    bocl_mem*  write_buffer_;

    //: boxm2 command queues, two, one for in and one for out
    cl_command_queue command_queue_;
    bocl_kernel test_kernel_; 
    
};

#endif
