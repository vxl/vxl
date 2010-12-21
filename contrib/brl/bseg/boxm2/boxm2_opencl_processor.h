#ifndef bomx2_opencl_processor_h
#define bomx2_opencl_processor_h
//:
// \file
#include <boxm2/boxm2_processor.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <brdb/brdb_value_sptr.h>
#include <vcl_vector.h>

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_mem.h>

//: boxm2_opencl_processor is an abstraction for a single GPU (using opencl). 
//  Becuase of memory restrictions on the GPU, the boxm2_opencl_processor must 
//  also serve as a sort of "dumb cache" where only one block is allowed into 
//  gpu memory at one time. 
//  The GPU carries scene data 
//    - Blocks (voxel structure)
//    - Data buffers (occupancy/appearance models)
//    - Block Metadata (origin, sub block dimensions/numbers)
//  It also stores a list of blocks to process (in a given order). 
//  When run is called, it will run the same process on each set of blocks
//  (this may need to be pushed down to the processor)

//: boxm2_opencl_processor is a singleton bocl_manager as well. 
class boxm2_opencl_processor: public boxm2_processor, public bocl_manager<boxm2_opencl_processor>
{
 public:
    boxm2_opencl_processor() :
      scene_info_(0),
      trees_(0),
      alphas_(0), 
      mogs_(0) { loaded_ = boxm2_block_id(-999, -999, -999);  }
    ~boxm2_opencl_processor(){}

    virtual bool  init();
    virtual bool  run(boxm2_process * process, vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);
    virtual bool  finish();
    
    //: sets the scene this processor will work on
    bool set_scene(boxm2_scene* scene) { scene_ = scene; }
    
    //: sets data on the GPU (creates CL mems)
    bool set_data(boxm2_scene* scene,
                  boxm2_block* blk,
                  boxm2_data_base* alpha,
                  boxm2_data_base* mog);
                  
    //: sets scene pointers to process (hopefully in their correct order)
    bool push_scene_data( boxm2_block* blk, 
                          boxm2_data_base* alph, 
                          boxm2_data_base* mog ); 
 protected:

    //: boxm2 command queues, two, one for in and one for out   
    cl_command_queue command_queue_[2];
    
    //: scene that this processor is operating on
    boxm2_scene* scene_;
    
    //: list of blocks to process
    vcl_vector<boxm2_block*>     blocks_to_process_; 
    vcl_vector<boxm2_data_base*> alphas_to_process_;
    vcl_vector<boxm2_data_base*> mogs_to_process_; 

    // === the OpenCL processor needs a sort of "cache" to make sure blocks are loaded efficiently ===
    //: list of buffers that can be accessed by the GPU (figure out a better way to do this...)
    boxm2_block_id loaded_; 
    bocl_mem_sptr scene_info_; 
    bocl_mem_sptr trees_; 
    bocl_mem_sptr alphas_;
    bocl_mem_sptr mogs_; 

};

#endif
