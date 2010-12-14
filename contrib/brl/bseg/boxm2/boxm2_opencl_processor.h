#ifndef bomx2_opencl_processor_h
#define bomx2_opencl_processor_h
//:
// \file
#include <boxm2/boxm2_processor.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_block_id.h>
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

//: boxm2_opencl_processor is a singleton bocl_manager as well. 
class boxm2_opencl_processor: public boxm2_processor, public bocl_manager<boxm2_opencl_processor>
{
 public:
    boxm2_opencl_processor() :
      scene_info_(0),
      trees_(0),
      alphas_(0), 
      mogs_(0) {}
    ~boxm2_opencl_processor(){}

    virtual bool  init();
    virtual bool  run(boxm2_process * process, vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);
    virtual bool  finish();
    
    //: first attempt at factoring out the SCENE data
    bool set_data(boxm2_scene_sptr& scene,
                  boxm2_block_sptr& blk, 
                  boxm2_data_base_sptr& alph,
                  boxm2_data_base_sptr& mog); 
 protected:

    //: boxm2 command queues, two, one for in and one for out   
    cl_command_queue command_queue_[2];
    
    //: scene that this processor is operating on
    boxm2_scene* scene_;

    //: list of blocks that are loaded into the GPU
    vcl_vector<boxm2_block_id> loaded_;

    // === the OpenCL processor needs a sort of "cache" to make sure blocks are loaded efficiently ===

    //: list of buffers that can be accessed by the GPU (figure out a better way to do this...)
    bocl_mem_sptr scene_info_; 
    bocl_mem_sptr trees_; 
    bocl_mem_sptr alphas_;
    bocl_mem_sptr mogs_; 

};

#endif
