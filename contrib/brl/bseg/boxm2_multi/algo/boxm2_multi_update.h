#ifndef boxm2_multi_update_h
#define boxm2_multi_update_h
//:
// \file
#include <boxm2_multi/boxm2_multi_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache1.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_manager.h>
#include <vil/vil_image_view.h>

//: boxm2_multi_cache - example realization of abstract cache class
class boxm2_multi_update
{
  public:
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;

    //multi render header
    static float update(boxm2_multi_cache& cache, vil_image_view<float>& img, vpgl_camera_double_sptr cam );
};


//: Helper class that stores factored out update objects - rays, queues, bit lookup
class boxm2_multi_update_helper
{
  public:
    boxm2_multi_update_helper(std::vector<cl_command_queue>& queues,
                              std::vector<bocl_mem_sptr>& ray_os,
                              std::vector<bocl_mem_sptr>& ray_ds,
                              std::vector<bocl_mem_sptr>& img_dims,
                              std::vector<bocl_mem_sptr>& tnearfarptrs,
                              std::vector<bocl_mem_sptr>& lookups,
                              std::vector<bocl_mem_sptr>& outputs,
                              std::vector<boxm2_multi_cache_group*>& group_orders,
                              std::vector<std::vector<boxm2_block_id> >& vis_orders,
                              std::vector<boxm2_opencl_cache1*>& vis_caches,
                              std::size_t maxBlocks) :
      queues_(queues),
      img_dims_(img_dims),
      tnearfarptrs_(tnearfarptrs),
      outputs_(outputs),
      ray_ds_(ray_ds),
      ray_os_(ray_os),
      lookups_(lookups),
      vis_orders_(vis_orders),
      vis_caches_(vis_caches),
      maxBlocks_(maxBlocks),
      group_orders_(group_orders)
    {}

    //update command queues
    std::vector<cl_command_queue>& queues_;

    //ray trace vars
    std::vector<bocl_mem_sptr>&  img_dims_, outputs_, ray_ds_, ray_os_, lookups_, tnearfarptrs_;

    //visibility order for each dev
    std::vector<std::vector<boxm2_block_id> >& vis_orders_;
    std::vector<boxm2_opencl_cache1*>& vis_caches_;
    std::size_t maxBlocks_;
    std::vector<boxm2_multi_cache_group*>& group_orders_;
};

//: patch helper
class boxm2_multi_image_patch
{
  public:
    boxm2_multi_image_patch(boxm2_block_id id,
                            float* img,
                            std::size_t ni, std::size_t nj,
                            std::size_t oi, std::size_t oj)
      : ni_(ni), nj_(nj), oi_(oi), oj_(oj), img_(img), id_(id) {}

  private:
    std::size_t ni_, nj_, oi_, oj_;
    float* img_;
    boxm2_block_id id_;
};


#endif
