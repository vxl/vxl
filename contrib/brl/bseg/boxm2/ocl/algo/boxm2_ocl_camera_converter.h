#ifndef boxm2_ocl_convert_camera_h_
#define boxm2_ocl_convert_camera_h_
//:
// \file
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

//: Conversion class
class boxm2_ocl_camera_converter
{
  public:
    //: Initializes bocl_mem_sptr ray_origins and ray_directions given a vpgl_camera_double_sptr
    static void compute_ray_image( bocl_device_sptr & device,
                                   cl_command_queue & queue,
                                   vpgl_camera_double_sptr & cam,
                                   unsigned cl_ni,
                                   unsigned cl_nj,
                                   bocl_mem_sptr & ray_origins,
                                   bocl_mem_sptr & ray_directions);
  private:
    //: private helper method converts perspective camera to ray image
    static float convert_persp_to_generic( bocl_device_sptr & device,
                                           cl_command_queue & queue,
                                           vpgl_perspective_camera<double>* pcam,
                                           bocl_mem_sptr & ray_origins,
                                           bocl_mem_sptr & ray_directions,
                                           unsigned cl_ni,
                                           unsigned cl_nj );

    //compile helper kernel, should only occur once
    static void compile_persp_to_generic_kernel(bocl_device_sptr device);

    //TODO: make this a map of kernels (by device)
    static bocl_kernel* persp_to_generic_kernel;
};

#endif // boxm2_ocl_convert_camera_h_
