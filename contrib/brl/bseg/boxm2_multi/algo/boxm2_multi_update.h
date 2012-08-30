#ifndef boxm2_multi_update_h
#define boxm2_multi_update_h
//:
// \file
#include <boxm2_multi_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
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
    boxm2_multi_update_helper(vcl_vector<cl_command_queue>& queues,
                              vcl_vector<bocl_mem_sptr>& ray_os,
                              vcl_vector<bocl_mem_sptr>& ray_ds,
                              vcl_vector<bocl_mem_sptr>& img_dims,
                              vcl_vector<bocl_mem_sptr>& lookups,
                              vcl_vector<bocl_mem_sptr>& outputs,
                              vcl_vector<boxm2_multi_cache_group*>& group_orders,
                              vcl_vector<vcl_vector<boxm2_block_id> >& vis_orders,
                              vcl_vector<boxm2_opencl_cache*>& vis_caches,
                              vcl_size_t maxBlocks) :
        queues_(queues),
        img_dims_(img_dims),
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
    vcl_vector<cl_command_queue>& queues_;

    //ray trace vars
    vcl_vector<bocl_mem_sptr>&  img_dims_, outputs_, ray_ds_, ray_os_, lookups_;

    //visibility order for each dev
    vcl_vector<vcl_vector<boxm2_block_id> >& vis_orders_;
    vcl_vector<boxm2_opencl_cache*>& vis_caches_;
    vcl_size_t maxBlocks_;
    vcl_vector<boxm2_multi_cache_group*>& group_orders_;
};

//: patch helper
class boxm2_multi_image_patch
{
  public:
    boxm2_multi_image_patch(boxm2_block_id id,
                            float* img,
                            vcl_size_t ni, vcl_size_t nj,
                            vcl_size_t oi, vcl_size_t oj)
      : ni_(ni), nj_(nj), oi_(oi), oj_(oj), img_(img), id_(id) {}

  private:
    vcl_size_t ni_, nj_, oi_, oj_;
    float* img_;
    boxm2_block_id id_;
};


#endif
