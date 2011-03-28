#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

void render_expected_image(bocl_device_sptr & device,
                              boxm2_scene_sptr & scene,
                              boxm2_opencl_cache_sptr & opencl_cache,
                              vpgl_camera_double_sptr & cam,
                              cl_command_queue & queue,
                              bocl_mem_sptr & exp_image,
                              bocl_mem_sptr & exp_img_dim,
                              vcl_string identifier,
                              vcl_string data_type,
                              vcl_map<vcl_string,vcl_vector<bocl_kernel*> > & kernels,
                              vcl_size_t * lthreads,
                              unsigned cl_ni,unsigned cl_nj);
