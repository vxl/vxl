#ifndef boxm2_multi_store_aux_h_
#define boxm2_multi_store_aux_h_
//:
// \file
// \brief This class does the cumulative seg len and cumulative observation on the GPU.

#include <boxm2_multi_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache1.h>
#include <boxm2_multi/algo/boxm2_multi_update.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vil/vil_image_view.h>


//: boxm2_multi_cache - example realization of abstract cache class
class boxm2_multi_store_aux
{
  public:

    //three separate sub procedures (three separate map reduce tasks)
    static float store_aux( boxm2_multi_cache& cache,
                            vil_image_view<float>& img,
                            const vpgl_camera_double_sptr& cam,
                            boxm2_multi_update_helper& helper);

    //static float store_aux_color( boxm2_multi_cache& cache,
    //                              vil_image_view<vil_rgba<float> >& img,
    //                              vgpl_camera_double_sptr cam,
    //                              boxm2_multi_update_helper& helper);

  private:

    static void store_aux_per_block(const boxm2_block_id& id,
                                          const boxm2_scene_sptr&    scene,
                                          boxm2_opencl_cache1* opencl_cache,
                                          cl_command_queue&   queue,
                                          bocl_kernel*        kernel,
                                          bocl_mem_sptr&      in_image,
                                          bocl_mem_sptr&      img_dim,
                                          bocl_mem_sptr&      ray_o_buff,
                                          bocl_mem_sptr&      ray_d_buff,
                                          bocl_mem_sptr&        tnearfarptr,
                                          bocl_mem_sptr&      cl_output,
                                          bocl_mem_sptr&      lookup,
                                          std::size_t*         lthreads,
                                          std::size_t*         gthreads,
                                          bool                store_rgb=false);

    // Reads aux memory from GPU to CPU ram
    static void read_aux(const boxm2_block_id& id,
                         boxm2_opencl_cache1* opencl_cache,
                         cl_command_queue&   queue);

    //map keeps track of all kernels compiled and cached
    static std::map<std::string, bocl_kernel*> kernels_;

    //compile kernels and cache
    static bocl_kernel* get_kernels(const bocl_device_sptr& device, const std::string& opts);

    //compile rgb kernels and store
    static bocl_kernel* get_kernels_color(const bocl_device_sptr& device, const std::string& opts);
};

#endif
