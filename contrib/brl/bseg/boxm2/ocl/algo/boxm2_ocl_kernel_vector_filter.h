#ifndef boxm2_ocl_kernel_vector_filter_h_included_
#define boxm2_ocl_kernel_vector_filter_h_included_

#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <bocl/bocl_device.h>
#include <bvpl/kernels/bvpl_kernel.h>


class boxm2_ocl_kernel_vector_filter
{
  public:
    //: constructor
  boxm2_ocl_kernel_vector_filter( const bocl_device_sptr& device,bool optimize_transfers = false);

    //: run the given filters on the given scene
  bool run(const boxm2_scene_sptr& scene, const boxm2_opencl_cache_sptr& opencl_cache, const bvpl_kernel_vector_sptr& filter_vector);

  private:
    bool compile_filter_kernel();

    bocl_device_sptr device_;

    bocl_kernel kernel_;

    bool optimize_transfers_;
};

#endif
