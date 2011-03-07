#ifndef boxm2_opencl_batch_update_process_h
#define boxm2_opencl_batch_update_process_h
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



class boxm2_opencl_batch_update_process : public boxm2_opencl_process_base
{
  public:
    //PASS ENUM
    enum {
      UPDATE_SEGLEN = 0,
      UPDATE_HIST = 1,
      CLEAN_SEGLEN = 2,
      UPDATE_PROB = 3
    };
  
    boxm2_opencl_batch_update_process() : data_type_("8bit"), 
                                          image_(0),
                                          vis_image_(0)
                                          {}

    //: process init and execute
    bool init() { return true; }
    bool execute(vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);
    bool clean();

    //: opencl specific init - compiles kernels associated with this process
    virtual bool init_kernel(cl_context* context, cl_device_id* device, vcl_string opts="");

  private:

      vcl_string img_dir_;
      vcl_string cam_dir_;
      //: render kernel (other processes may have many kernels)
      vcl_vector<bocl_kernel*> update_kernels_;

      vcl_string data_type_;
      //: workspace
      vcl_size_t lThreads_[2];
      vcl_size_t gThreads_[2];
      vcl_size_t img_size_[2];

      //: INPUT IMAGE:
      bocl_mem* image_;
      bocl_mem* vis_image_;         //will be a float image, maintains visibility between blocks

      //: block stuff
      bocl_mem* blk_info_;
      bocl_mem* blk_;
      bocl_mem* alpha_;
      bocl_mem* hist_;
      bocl_mem* aux_;
      bocl_mem* intensity_;

      //: cam
      bocl_mem* persp_cam_;
      bocl_mem* img_dim_;
      bocl_mem* cl_output_;
      bocl_mem* lookup_;

      //----- Update Helper Methods ----------------------------------------------

      //: set args
      bool set_args(unsigned pass);

      //: Set workspace helper method
      bool set_workspace(unsigned pass);

      //: write input image to buffer
      bool write_input_image(vil_image_view<float>* input_image);
};


#endif
