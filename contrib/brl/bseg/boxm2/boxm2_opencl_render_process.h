#ifndef boxm2_opencl_render_process_h
#define boxm2_opencl_render_process_h
//:
// \file
#include <boxm2/boxm2_opencl_process_base.h>
#include <vcl_vector.h>

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>

class boxm2_opencl_render_process : public boxm2_opencl_process_base
{
  public:
    boxm2_opencl_render_process() : context_(0) {}

    bool init();
    bool execute(vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);

    //: opencl specific methods
    virtual bool init_kernel(cl_context& context, cl_device_id& device);

  private:

    //: cl context
    cl_context* context_;

    //: command queue for thsi process.
    cl_command_queue command_queue_;

    //: render kernel (other processes may have many kernels
    bocl_kernel render_kernel_;
};

#endif
