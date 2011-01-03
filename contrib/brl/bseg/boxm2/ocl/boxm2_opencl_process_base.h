#ifndef boxm2_opencl_process_base_h
#define boxm2_opencl_process_base_h

#include <boxm2/boxm2_process.h>
#include <bocl/bocl_cl.h>
#include <vcl_string.h>

class boxm2_opencl_process_base:public boxm2_process
{
  public: 
    virtual bool init_kernel(cl_context& context, cl_device_id& device, vcl_string opts="")=0;
    virtual void set_command_queue(cl_command_queue* queue)=0; 
};

#endif
