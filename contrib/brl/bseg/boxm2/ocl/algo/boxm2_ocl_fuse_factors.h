#ifndef boxm2_ocl_fuse_factors_h_
#define boxm2_ocl_fuse_factors_h_
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
class boxm2_ocl_fuse_factors
{
public:
    static bool fuse_factors(boxm2_scene_sptr         scene,
                                       bocl_device_sptr         device,
                                       boxm2_opencl_cache_sptr  opencl_cache,
                                       std::vector<std::string> factors_ident,
                                       std::vector<float>   weights);
private:

    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_fuse_factors_kernels(bocl_device_sptr device, std::string opts = "");

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > fuse_factors_kernels_;

};


#endif
