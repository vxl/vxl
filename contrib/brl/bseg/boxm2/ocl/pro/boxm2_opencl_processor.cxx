#include "boxm2_opencl_processor.h"

//boxm2 data structures
#include <boxm2/ocl/boxm2_opencl_cache.h>

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

bool boxm2_opencl_processor::run(vcl_vector<brdb_value_sptr> const& /*input*/,
                                 vcl_vector<brdb_value_sptr>& /*output*/)
{
  return false;
}

bool boxm2_opencl_processor::finish()
{
  return false;
}
