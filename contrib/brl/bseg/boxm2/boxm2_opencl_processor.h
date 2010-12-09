#ifndef bomx2_opencl_processor_h
#define bomx2_opencl_processor_h

#include <boxm2/boxm2_processor.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block_id.h>
#include <brdb/brdb_value_sptr.h>
#include <vcl_vector.h>

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_mem.h>

//: boxm2_opencl_processor is a singleton bocl_manager as well. 
class boxm2_opencl_processor: public boxm2_processor, public bocl_manager<boxm2_opencl_processor>
{
 public:
    boxm2_opencl_processor(){}
    ~boxm2_opencl_processor(){}

    virtual bool  init();
    virtual bool  run(boxm2_process * process, vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);
    virtual bool  finish();
        
 protected:
    
    //: scene that this processor is operating on
    boxm2_scene* scene_;
    
    //: list of blocks that are loaded into the GPU 
    vcl_vector<boxm2_block_id> loaded_; 
    
    //: the OpenCL processor needs a sort of "cache" to make sure blocks are loaded efficiently
    //: list of buffers that can be accessed by the GPU (figure out a better way to do this...)
    bocl_mem* blocks_; 
    bocl_mem* alphas_;
    bocl_mem* mogs_; 

};

#endif
