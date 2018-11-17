#ifndef boxm2_ocl_update_sun_visibilities_h_
#define boxm2_ocl_update_sun_visibilities_h_
//:
// \file
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//boxm2 includes
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/io/boxm2_cache.h>

#include <vil/vil_image_view_base.h>

//: boxm2_ocl_paint_batch class
class boxm2_ocl_update_sun_visibilities
{
  public:
    static bool update( const boxm2_scene_sptr&         scene,
                        bocl_device_sptr         device,
                        const boxm2_opencl_cache_sptr&  opencl_cache,
                        const boxm2_cache_sptr& cache,
                        vpgl_camera_double_sptr  sun_cam,
                        unsigned ni,
                        unsigned nj,
                        const std::string&               prefix_name
                      );

  private:
    static void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels,const std::string& opts);

    //map of kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;
};

#endif // boxm2_ocl_update_sun_visibilities_h_
