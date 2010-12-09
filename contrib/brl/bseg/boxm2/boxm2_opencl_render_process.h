#ifndef boxm2_opencl_render_process_h
#define boxm2_opencl_render_process_h

#include <boxm2/boxm2_opencl_process_base.h>
#include <vcl_vector.h>

//open cl includes
#include <bocl/bocl_kernel.h>

class boxm2_opencl_render_process : public boxm2_opencl_process_base
{
  public: 
    bool init();
    bool execute(vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output); 
    
    //: opencl specific methods
    virtual bool init_kernel(cl_context& context, cl_device_id& device);
  
  private: 
    bocl_kernel render_kernel_;
    
};

#endif
