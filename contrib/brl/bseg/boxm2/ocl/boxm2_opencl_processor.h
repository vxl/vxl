#ifndef bomx2_opencl_processor_h
#define bomx2_opencl_processor_h
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

#include <boxm2/boxm2_processor.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <brdb/brdb_value_sptr.h>
#include <vcl_vector.h>

#include "boxm2_opencl_cache.h"

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_mem.h>

//Number of queues can be tweaked
#define NUM_QUEUES 2

//: boxm2_opencl_processor is a singleton bocl_manager as well.
class boxm2_opencl_processor: public boxm2_processor, public bocl_manager<boxm2_opencl_processor>
{
 public:
    boxm2_opencl_processor() {}
    ~boxm2_opencl_processor() {}

    virtual bool  init();
    virtual bool  run(boxm2_process * process, vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);
    virtual bool  finish();

    //: handles the sequencing of blocks
    bool sequencing(vcl_vector<boxm2_block_id> ids,
                    boxm2_process* process,
                    vcl_vector<brdb_value_sptr>& input,
                    vcl_vector<brdb_value_sptr>& output);

    float exec_time() { return exec_time_; }

    //: sets the scene this processor will work on
    bool set_scene(boxm2_scene* scene) { scene_ = scene; return true; }
    void set_cpu_cache(boxm2_cache* cache) { cpu_cache_ = cache; }
    
    cl_command_queue* get_queue() { return &queues_[0]; } 

 protected:
  
    //:opencl cache to keep track of scene stuff (ensure only 1 copy)
    boxm2_cache*        cpu_cache_;
    boxm2_opencl_cache* gpu_cache_;     
 
    //: execution time (in ms)
    float exec_time_; 
 
    //: execution queue
    int exec_queue_;

    //: scene that this processor is operating on
    boxm2_scene* scene_;
    
    //: boxm2 command queues, two, one for in and one for out
    cl_command_queue queues_[NUM_QUEUES];

};

#endif
