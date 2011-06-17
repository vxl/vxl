#ifndef boxm2_ocl_cone_update_function_h_
#define boxm2_ocl_cone_update_function_h_

#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vil/vil_image_view.h>
#include <vcl_vector.h>

float boxm2_ocl_cone_update( boxm2_scene_sptr & scene,
                             bocl_device_sptr & device,
                             boxm2_opencl_cache_sptr & opencl_cache,
                             vcl_vector<bocl_kernel*> kernels, 
                             cl_command_queue& queue,
                             vcl_string data_type,
                             vcl_string num_obs_type,
                             vpgl_camera_double_sptr cam ,
                             vil_image_view_base_sptr in_img,
                             unsigned int roi_ni0=0,
                             unsigned int roi_nj0=0);


float boxm2_ocl_adaptive_cone_update(boxm2_scene_sptr & scene,
                                     bocl_device_sptr & device,
                                     boxm2_opencl_cache_sptr & opencl_cache,
                                     vcl_vector<bocl_kernel*> kernels, 
                                     cl_command_queue& queue,
                                     vcl_string data_type,
                                     vcl_string num_obs_type,
                                     vpgl_camera_double_sptr cam ,
                                     vil_image_view_base_sptr in_img,
                                     unsigned int roi_ni0=0,
                                     unsigned int roi_nj0=0);

#endif // boxm2_ocl_cone_update_function_h_
