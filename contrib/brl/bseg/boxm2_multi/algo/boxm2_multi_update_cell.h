#ifndef boxm2_multi_update_cell_h_
#define boxm2_multi_update_cell_h_
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
class boxm2_multi_update_cell
{
  public:
    //three separate sub procedures (three separate map reduce tasks)
    //static float update_cells(boxm2_multi_cache& cache,
    //                          const vil_image_view<float>& img,
    //                          vpgl_camera_double_sptr cam,
    //                          std::map<bocl_device*, float*>& vis_map,
    //                          std::map<bocl_device*, float*>& pre_map,
    //                          float*                         norm_image,
    //                          boxm2_multi_update_helper& helper);
    static float update_cells(boxm2_multi_cache& cache,
                              const vil_image_view<float>& img,
                              const vpgl_camera_double_sptr& cam,
                              float* norm_image,
                              boxm2_multi_update_helper& helper);

  private:
    //runs pre/vis on single block
    static float calc_beta_per_block(const boxm2_block_id&     id,
                                    const boxm2_scene_sptr&    scene,
                                    boxm2_opencl_cache1* opencl_cache,
                                    cl_command_queue&   queue,
                                    const std::string&          data_type,
                                    bocl_kernel*        kern,
                                    bocl_mem_sptr&      vis_image,
                                    bocl_mem_sptr&      pre_image,
                                    bocl_mem_sptr&      norm_image,
                                    bocl_mem_sptr&      img_dim,
                                    bocl_mem_sptr&      ray_o_buff,
                                    bocl_mem_sptr&      ray_d_buff,
                    bocl_mem_sptr&      tnearfarptr,
                                    bocl_mem_sptr&      cl_output,
                                    bocl_mem_sptr&      lookup,
                                    std::size_t*         lthreads,
                                    std::size_t*         gThreads);

    static float calc_beta_reduce( boxm2_multi_cache& mcache,
                                   const vpgl_camera_double_sptr& cam,
                                   boxm2_multi_update_helper& helper);

    //map keeps track of all kernels compiled and cached
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;

    //compile kernels and cache
    static std::vector<bocl_kernel*>& get_kernels(const bocl_device_sptr& device, const std::string& opts);
};

#endif
