#ifndef boxm2_opencl_render_depth_process_h
#define boxm2_opencl_render_depth_process_h
//:
// \file
#include "boxm2_opencl_process_base.h"
#include <vcl_vector.h>

//cameras/images
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>

class boxm2_opencl_render_depth_process : public boxm2_opencl_process_base
{
  public:
    boxm2_opencl_render_depth_process() : context_(0) {}

    bool init();
    bool execute(vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);

    //: opencl specific methods
    virtual bool init_kernel(cl_context& context, cl_device_id& device);

  private:

    //: cl context
    cl_context* context_;

    //: render kernel (other processes may have many kernels
    bocl_kernel render_kernel_;

    //0----- HACK STUFF TO GO SOMEWHERE ELSE ----------------------------------

    //: command queue for thsi process.
    cl_command_queue command_queue_;

    //: cam method should go somewhere else, here for now
    cl_float* set_persp_camera(vpgl_camera_double_sptr cam);

    //: get lookup table
    cl_uchar* set_bit_lookup();

    // === the OpenCL processor needs a sort of "cache" to make sure blocks are loaded efficiently ===

    //: list of buffers that can be accessed by the GPU (figure out a better way to do this...)
    bocl_mem* scene_info_;
    bocl_mem* trees_;
    bocl_mem* alphas_;
    bocl_mem* mogs_;
};

#endif
