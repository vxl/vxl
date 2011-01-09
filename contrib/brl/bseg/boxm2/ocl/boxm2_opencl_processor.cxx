#include "boxm2_opencl_processor.h"
#include "boxm2_opencl_process_base.h"

//boxm2 data structures

//cameras/images
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_vector_fixed.h>
#include <vul/vul_timer.h>

//brdb stuff
#include <brdb/brdb_value.h>

bool boxm2_opencl_processor::init()
{
  vcl_cout<<"PROCESSOR INIT CALLED?"<<vcl_endl;

  // set up 2 command queues
  int status;
  for (int i=0; i<NUM_QUEUES; ++i) {
    queues_[i] = clCreateCommandQueue(this->context(),
                                      this->devices()[0],
                                      CL_QUEUE_PROFILING_ENABLE,
                                      &status);
    if (!check_val(status,CL_SUCCESS,"Failed in command queue creation" + error_to_string(status)))
      return false;
  }

  //set up our cache
  gpu_cache_ = new boxm2_opencl_cache(cpu_cache_,
                                      &(this->context()),
                                      &queues_[0],
                                      scene_);
  return true;
}

bool boxm2_opencl_processor::run(boxm2_process * process, vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output)
{
  boxm2_opencl_process_base* pro = (boxm2_opencl_process_base*) process;
  pro->set_gpu_cache(gpu_cache_);
  pro->set_command_queue(&queues_[0]);
  pro->execute(input, output);

  vcl_cout<<" Time: total ("<<pro->total_time()<<" ms), "
          <<" transfer("<<pro->transfer_time()<<" ms), "
          <<" gpu("<<pro->gpu_time()<<" ms)"<<vcl_endl;

  return true;
}

bool boxm2_opencl_processor::finish()
{
  return true;
}
