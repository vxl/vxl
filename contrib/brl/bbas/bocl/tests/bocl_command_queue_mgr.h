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
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_kernel.h>

#define NUM_QUEUES 2


//: boxm2_opencl_processor is a singleton bocl_manager as well.
class bocl_command_queue_mgr: public bocl_manager<bocl_command_queue_mgr>
{
 public:

    bool init_kernel();
    bool test_async_command_queue();
    bool test_dual_command_queue();

 protected:

    int memLength_;
    int memHalf_;

    bocl_mem*  pinned_in_;
    bocl_mem*  input_;

    bocl_mem*  pinned_out_;
    bocl_mem*  output_;

    //: boxm2 command queues, two, one for in and one for out
    cl_command_queue queue_a_;
    cl_command_queue queue_b_;
    bocl_kernel kernel_a_;
    bocl_kernel kernel_b_;

    bocl_mem*  offsets_[NUM_QUEUES];
    cl_command_queue queues_[NUM_QUEUES];
    bocl_kernel kernels_[NUM_QUEUES];

 private:
    friend class bocl_manager<bocl_command_queue_mgr>;
    // don't allow users to create their own instances (force use of instance())
    bocl_command_queue_mgr() = default;
    ~bocl_command_queue_mgr() override= default;

};

#endif
