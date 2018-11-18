#ifndef bstm_ocl_render_expected_image_function_h_
#define bstm_ocl_render_expected_image_function_h_

#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/ocl/bstm_opencl_cache.h>


float render_expected_image( bstm_scene_sptr & scene,
                            bocl_device_sptr & device,
                            bstm_opencl_cache_sptr & opencl_cache,
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
                            int apptypesize,  float time,
                            const std::string& label_data_type = "", int label_apptypesize = 0, bool render_label = false);


#endif // bstm_ocl_render_expected_image_function_h_
