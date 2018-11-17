#ifndef boxm2_ocl_remove_low_nobs_h_
#define boxm2_ocl_remove_low_nobs_h_
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
class boxm2_ocl_remove_low_nobs
{
public:
    static bool remove_low_nobs(const boxm2_scene_sptr&         scene,
                                const bocl_device_sptr&         device,
                                const boxm2_opencl_cache_sptr&  opencl_cache,
                                float                    nobs_threshold
                                );

private:

    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_remove_low_nobs_kernels(const bocl_device_sptr& device, const std::string& opts = "");

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > remove_low_nobs_kernels_;

};


#endif
