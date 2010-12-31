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
    boxm2_opencl_processor() {
      exec_queue_ = 0;
      loaded_[0] = boxm2_block_id(-999, -999, -999);
      loaded_[1] = boxm2_block_id(-999, -999, -999);
    }
    ~boxm2_opencl_processor() {}

    virtual bool  init();
    virtual bool  run(boxm2_process * process, vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);
    virtual bool  finish();

    //: sets the scene this processor will work on
    bool set_scene(boxm2_scene* scene) { scene_ = scene; return true; }

    //: sets scene pointers to process (hopefully in their correct order)
    bool push_scene_data( boxm2_block* blk,
                          boxm2_data_base* alph,
                          boxm2_data_base* mog );

    //: initializes scene buffers (Both of em)
    bool setup_pinned_buffers( boxm2_scene* scene,
                               boxm2_block* blk,
                               boxm2_data_base* alpha,
                               boxm2_data_base* mog );

 protected:
    //: execution queue
    int exec_queue_;

    //: scene that this processor is operating on
    boxm2_scene* scene_;

    //: list of blocks to process
    vcl_vector<boxm2_block*>     blocks_to_process_;
    vcl_vector<boxm2_data_base*> alphas_to_process_;
    vcl_vector<boxm2_data_base*> mogs_to_process_;

    // === the OpenCL processor needs a sort of "cache" to make sure blocks are loaded efficiently ===

    //: list of buffers that can be accessed by the GPU
    // ALL OF THE BOCL_MEM pointers will create Pinned host memory, which allows
    // the gpu to overlap a kernel execution
    boxm2_block_id loaded_[NUM_QUEUES];
    bocl_mem_sptr  scene_info_[NUM_QUEUES];
    bocl_mem_sptr  trees_[NUM_QUEUES];
    bocl_mem_sptr  alphas_[NUM_QUEUES];
    bocl_mem_sptr  mogs_[NUM_QUEUES];

    vnl_vector_fixed<unsigned char, 16> * trees_pin[2];
    float*   alphas_pin[2];
    char*    mogs_pin[2]; 
    char*    info_pin[2];  


    //: boxm2 command queues, two, one for in and one for out
    cl_command_queue queues_[NUM_QUEUES];

    //: Helper method to push scene information onto the gpu
    // Pushes block and data into a BOCL_mem, using \p queue_index
    bool enqueue_write_scene( boxm2_scene* scene,
                              boxm2_block* blk,
                              boxm2_data_base* alpha,
                              boxm2_data_base* mog,
                              int queue_index, int buffer_index );
};

#endif
