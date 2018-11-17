#ifndef boxm2_ocl_fuse_based_visibility_h_
#define boxm2_ocl_fuse_based_visibility_h_
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
class boxm2_ocl_fuse_based_visibility
{
  public:
    static bool fuse_based_visibility( boxm2_scene_sptr         sceneA,
                                       const boxm2_scene_sptr&         sceneB,
                                       const bocl_device_sptr&         device,
                                       const boxm2_opencl_cache_sptr&  opencl_cache);

  private:
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_kernels(const bocl_device_sptr& device, const std::string& opts="");

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;


};

//: boxm2_ocl_paint_batch class
class boxm2_ocl_fuse_based_orientation
{
  public:
    static bool fuse_based_orientation( boxm2_scene_sptr         sceneA,
                                               const boxm2_scene_sptr&         sceneB,
                                               const bocl_device_sptr&         device,
                                               const boxm2_opencl_cache_sptr&  opencl_cache);

  private:
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_kernels(const bocl_device_sptr& device, const std::string& opts="");

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;


};


//: boxm2_ocl_paint_batch class
class boxm2_ocl_fuse_surface_density
{
  public:
    static bool fuse_surface_density( boxm2_scene_sptr         sceneA,
                                               const boxm2_scene_sptr&         sceneB,
                                               const bocl_device_sptr&         device,
                                               const boxm2_opencl_cache_sptr&  opencl_cache);

  private:
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_kernels(const bocl_device_sptr& device, const std::string& opts="");

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;


};
#endif // boxm2_ocl_fuse_based_visibility_h_
