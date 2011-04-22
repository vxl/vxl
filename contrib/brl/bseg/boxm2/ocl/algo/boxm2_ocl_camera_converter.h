#ifndef boxm2_ocl_convert_camera_h_
#define boxm2_ocl_convert_camera_h_

#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

//: Conversion class
class boxm2_ocl_camera_converter
{
  public: 
  
    static float convert_persp_to_generic( bocl_device_sptr & device,
                                           cl_command_queue & queue,
                                           vpgl_camera_double_sptr & pcam,
                                           bocl_mem_sptr & ray_origins,
                                           bocl_mem_sptr & ray_directions, 
                                           unsigned cl_ni,
                                           unsigned cl_nj );

    static void compile_persp_to_generic_kernel(bocl_device_sptr device); 
  
  private:
  
    //TODO make this a map of kernels (by device)
    static bocl_kernel* persp_to_generic_kernel;

};
#endif // boxm2_ocl_convert_camera_h_
