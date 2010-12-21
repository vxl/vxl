#ifndef boxm2_opencl_render_process_h
#define boxm2_opencl_render_process_h
//:
// \file
#include <boxm2/boxm2_opencl_process_base.h>
#include <vcl_vector.h>

//cameras/images
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>

class boxm2_opencl_render_process : public boxm2_opencl_process_base
{
  public:
    boxm2_opencl_render_process() : context_(0), image_(0), vis_img_(0) {}

    bool init();
    bool execute(vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);

    //: opencl specific methods
    virtual bool init_kernel(cl_context& context, cl_device_id& device, vcl_string opts="");

    //: render specific method
    bool set_image( bocl_mem* img ) { image_ = img; return true; }

    //: get commandqueue
    cl_command_queue& command_queue() { return command_queue_; }

    //: get image
    bocl_mem* image() { return image_; }

  private:

    //: cl context
    cl_context* context_;

    //: render kernel (other processes may have many kernels
    bocl_kernel render_kernel_;

    //: image (so it isn't created over and over)
    bocl_mem* image_;
    
    //: visibility image
    bocl_mem* vis_img_; 

    //0----- HACK STUFF TO GO SOMEWEHRE ELSE ----------------------------------
    //: command queue for thsi process.
    cl_command_queue command_queue_;

    //: cam method should go somwehre else, here for now
    cl_float* set_persp_camera(vpgl_camera_double_sptr cam);

    //: get lookup table
    cl_uchar* set_bit_lookup();
};

#endif
