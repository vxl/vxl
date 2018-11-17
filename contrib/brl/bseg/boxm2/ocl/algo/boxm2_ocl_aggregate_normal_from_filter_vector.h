#ifndef boxm2_ocl_aggregate_normal_from_filter_h_included_
#define boxm2_ocl_aggregate_normal_from_filter_h_included_
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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

    boxm2_ocl_aggregate_normal_from_filter_vector(const boxm2_scene_sptr& scene, const boxm2_opencl_cache_sptr& ocl_cache,
                                                  const bocl_device_sptr& device, const bvpl_kernel_vector_sptr& filter_vector,bool optimize_transfers = false);

    bool run(bool clear_cache=true);
    bool reset();

  private:

    bool compile_kernel(bocl_kernel &aggregate_kernel, std::string opts);

    bocl_kernel kernel_;

    boxm2_scene_sptr scene_;
    boxm2_opencl_cache_sptr ocl_cache_;
    bocl_device_sptr device_;
    bool optimize_transfers_;
    bvpl_kernel_vector_sptr filter_vector_;

};

#endif
