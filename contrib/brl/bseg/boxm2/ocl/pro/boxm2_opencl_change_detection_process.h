#ifndef boxm2_opencl_change_detection_process_h
#define boxm2_opencl_change_detection_process_h
//:
// \file
#include <boxm2/ocl/pro/boxm2_opencl_process_base.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vcl_vector.h>
#include <boxm2/boxm2_data_traits.h>

//cameras/images
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>

class boxm2_opencl_change_detection_process : public boxm2_opencl_process_base
{
  public:
    boxm2_opencl_change_detection_process() : in_image_(0), image_(0), vis_img_(0) {}
    
    //: process init and execute
    bool init() { return true; }
    bool execute(vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);
    bool clean(); 

    //: opencl specific init - compiles kernels associated with this process
    virtual bool init_kernel(cl_context* context, cl_device_id* device, vcl_string opts="");

    ////////////////////////////////////////////////////////////////////////////
    //: render specific methods
    bool set_image( bocl_mem* img ) { image_ = img; return true; }
    
    //: get image
    bocl_mem* image() { return image_; }
    ////////////////////////////////////////////////////////////////////////////

  private:
    bool write_input_image(vil_image_view<float>* input_image);

    //: render kernel (other processes may have many kernels
    bocl_kernel change_kernel_;
    //: kernel to normalize images
    bocl_kernel normalize_change_kernel_;
    //: image (so it isn't created over and over)
    bocl_mem* in_image_;

    //: image (so it isn't created over and over)
    bocl_mem* image_;
    
    //: visibility image
    bocl_mem* vis_img_; 

};

#endif
