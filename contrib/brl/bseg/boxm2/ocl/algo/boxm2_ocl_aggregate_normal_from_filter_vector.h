#ifndef boxm2_ocl_aggregate_normal_from_filter_h_included_
#define boxm2_ocl_aggregate_normal_from_filter_h_included_
#include <vcl_string.h>

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>

#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

#include <bvpl/kernels/bvpl_kernel.h>


class boxm2_ocl_aggregate_normal_from_filter_vector
{
  public:
    typedef boxm2_data_traits<BOXM2_FLOAT> RESPONSE_DATATYPE;

    boxm2_ocl_aggregate_normal_from_filter_vector(boxm2_scene_sptr scene, boxm2_opencl_cache_sptr ocl_cache,
                                                  bocl_device_sptr device, bvpl_kernel_vector_sptr filter_vector);

    bool run();

  private:
    
    bool compile_kernel(bocl_kernel &aggregate_kernel, vcl_string opts);

    bocl_kernel kernel_;

    boxm2_scene_sptr scene_;
    boxm2_opencl_cache_sptr ocl_cache_;
    bocl_device_sptr device_;

    bvpl_kernel_vector_sptr filter_vector_;

};

#endif
