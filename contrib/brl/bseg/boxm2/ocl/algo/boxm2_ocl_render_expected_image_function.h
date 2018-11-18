#ifndef boxm2_ocl_render_expected_image_function_h_
#define boxm2_ocl_render_expected_image_function_h_

#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/ocl/boxm2_opencl_cache1.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>

float render_expected_image( boxm2_scene_sptr & scene,
                             bocl_device_sptr & device,
                             boxm2_opencl_cache_sptr & opencl_cache,
                             cl_command_queue & queue,
                             vpgl_camera_double_sptr & cam,
                             bocl_mem_sptr & exp_image,
                             bocl_mem_sptr & vis_image,
                             bocl_mem_sptr & max_omega_image,
                             bocl_mem_sptr & exp_img_dim,
                             const std::string& data_type,
                             bocl_kernel* kernel,
                             std::size_t * lthreads,
                             unsigned cl_ni,
                             unsigned cl_nj,
                             int apptypesize,
                             bocl_mem_sptr & tnearfar_mem_ptr,
                             std::size_t startI=0,
                             std::size_t startJ=0);

float render_expected_image2( boxm2_scene_sptr & scene,
                              bocl_device_sptr & device,
                              boxm2_opencl_cache_sptr & opencl_cache,
                              cl_command_queue & queue,
                              vpgl_camera_double_sptr & cam,
                              bocl_mem_sptr & exp_image,
                              bocl_mem_sptr & vis_image,
                              bocl_mem_sptr & max_omega_image,
                              bocl_mem_sptr & exp_img_dim,
                              const std::string& data_type,
                              bocl_kernel* kernel,
                              std::size_t * lthreads,
                              unsigned cl_ni,
                              unsigned cl_nj,
                              int apptypesize,
                              bocl_mem_sptr & tnearfar_mem_ptr);

float render_cone_expected_image( boxm2_scene_sptr & scene,
                                  bocl_device_sptr & device,
                                  boxm2_opencl_cache_sptr & opencl_cache,
                                  cl_command_queue & queue,
                                  vpgl_camera_double_sptr & cam,
                                  bocl_mem_sptr & exp_image,
                                  bocl_mem_sptr & vis_image,
                                  bocl_mem_sptr & ray_level_image,
                                  bocl_mem_sptr & exp_img_dim,
                                  const std::string& data_type,
                                  bocl_kernel* kernel,
                                  std::size_t * lthreads,
                                  unsigned cl_ni,
                                  unsigned cl_nj );
float render_expected_shadow_map(  boxm2_scene_sptr & scene,
                              bocl_device_sptr & device,
                              boxm2_opencl_cache_sptr & opencl_cache,
                              cl_command_queue & queue,
                              vpgl_camera_double_sptr & cam,
                              bocl_mem_sptr & exp_image,
                              bocl_mem_sptr & vis_image,
                              bocl_mem_sptr & exp_img_dim,
                              const std::string& data_type,
                              bocl_kernel* kernel,
                              std::size_t * lthreads,
                              unsigned cl_ni,
                              unsigned cl_nj );

float render_expected_phongs_image( boxm2_scene_sptr & scene,
                            bocl_device_sptr & device,
                            boxm2_opencl_cache_sptr & opencl_cache,
                            cl_command_queue & queue,
                            vpgl_camera_double_sptr & cam,
                            bocl_mem_sptr & exp_image,
                            bocl_mem_sptr & vis_image,
                            bocl_mem_sptr & exp_img_dim,
                            const std::string& data_type,
                            bocl_kernel* kernel,
                            std::size_t * lthreads,
                            unsigned cl_ni,
                            unsigned cl_nj,
                            const bocl_mem_sptr& sundir);

float render_expected_image_naa(  boxm2_scene_sptr & scene,
                                  bocl_device_sptr & device,
                                  boxm2_opencl_cache_sptr & opencl_cache,
                                  cl_command_queue & queue,
                                  vpgl_camera_double_sptr & cam,
                                  bocl_mem_sptr & exp_image,
                                  bocl_mem_sptr & vis_image,
                                  bocl_mem_sptr & exp_img_dim,
                                  bocl_kernel* kernel,
                                  std::size_t * lthreads,
                                  unsigned cl_ni,
                                  unsigned cl_nj,
                                  const brad_image_metadata_sptr& metadata,
                                  const brad_atmospheric_parameters_sptr& atm_params);

float render_expected_albedo_normal(  boxm2_scene_sptr & scene,
                                      bocl_device_sptr & device,
                                      boxm2_opencl_cache_sptr & opencl_cache,
                                      cl_command_queue & queue,
                                      vpgl_camera_double_sptr & cam,
                                      bocl_mem_sptr & exp_image,
                                      bocl_mem_sptr & vis_image,
                                      bocl_mem_sptr & exp_img_dim,
                                      bocl_kernel* kernel,
                                      std::size_t * lthreads,
                                      unsigned cl_ni,
                                      unsigned cl_nj);

#endif // boxm2_ocl_render_expected_image_function_h_
