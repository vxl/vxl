#ifndef boxm2_multi_store_aux_h_
#define boxm2_multi_store_aux_h_
//:
// \file
// \brief This class does the cumulative seg len and cumulative observation on the GPU.

#include <boxm2_multi_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2_multi/algo/boxm2_multi_update.h>


//: boxm2_multi_cache - example realization of abstract cache class
class boxm2_multi_store_aux
{
  public:

    //three separate sub procedures (three separate map reduce tasks)
    static float store_aux( boxm2_multi_cache& cache,
                            vil_image_view<float>& img,
                            vpgl_camera_double_sptr cam, 
                            boxm2_multi_update_helper& helper);

  private:

    // Reads aux memory from GPU to CPU ram
    static void read_aux(const boxm2_block_id& id, 
                        boxm2_opencl_cache* opencl_cache, 
                        cl_command_queue&   queue);


    static void store_aux_per_block(const boxm2_block_id& id,
                                          boxm2_scene_sptr    scene,
                                          boxm2_opencl_cache* opencl_cache,
                                          cl_command_queue&   queue,
                                          bocl_kernel*        kernel,
                                          bocl_mem_sptr&      in_image,
                                          bocl_mem_sptr&      img_dim,
                                          bocl_mem_sptr&      ray_o_buff,
                                          bocl_mem_sptr&      ray_d_buff,
                                          bocl_mem_sptr&      cl_output,
                                          bocl_mem_sptr&      lookup,
                                          vcl_size_t*         lthreads,
                                          vcl_size_t*         gthreads);

    //map keeps track of all kernels compiled and cached
    static vcl_map<vcl_string, bocl_kernel*> kernels_;

    //compile kernels and cache
    static bocl_kernel* get_kernels(bocl_device_sptr device, vcl_string opts);
};

#endif
