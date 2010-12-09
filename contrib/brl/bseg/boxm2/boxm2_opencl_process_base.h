#ifndef boxm2_opencl_process_base_h
#define boxm2_opencl_process_base_h

#include <boxm2/boxm2_process.h>
#include <bocl/bocl_cl.h>

class boxm2_opencl_process_base:public boxm2_process
{
  virtual bool init_kernel(cl_context& context, cl_device_id& device);
};

#endif
