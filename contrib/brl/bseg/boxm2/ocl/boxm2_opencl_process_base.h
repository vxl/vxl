#ifndef boxm2_opencl_process_base_h
#define boxm2_opencl_process_base_h

#include <boxm2/boxm2_process.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <bocl/bocl_cl.h>
#include <vcl_string.h>

class boxm2_opencl_process_base : public boxm2_process
{
  public: 
  
    boxm2_opencl_process_base() : command_queue_(0), cache_(0) {}
  
    //: init_kernel compiles the process' kernels (MUST IMPLEMENT in subclasses)
    virtual bool init_kernel(cl_context& context, cl_device_id& device, vcl_string opts="")=0;
    
    //: set this process' command queue (to execute the kernels on)
    virtual void set_command_queue(cl_command_queue* queue) { command_queue_ = queue; }
    
    //: set this gpu cache (so you can grab bocl_mem's of scene data) 
    virtual bool set_gpu_cache(boxm2_opencl_cache* cache)   { cache_ = cache; } 
    
    //: set/get command queue (needed for executing th ekernel
    virtual cl_command_queue* command_queue() { return command_queue_; }

  protected:
  
    //: opencl cache to get block mems
    boxm2_opencl_cache* cache_;
  
    //: current command queue for this process
    cl_command_queue* command_queue_;
  
};

#endif
