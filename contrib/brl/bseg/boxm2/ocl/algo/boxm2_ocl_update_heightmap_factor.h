#ifndef boxm2_ocl_update_heightmap_factor_h_
#define boxm2_ocl_update_heightmap_factor_h_
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
class boxm2_ocl_smooth_heightmap_pdata
{
public:
    static bool compute_smooth_heightmap_pdata(boxm2_scene_sptr         scene,
                                               const bocl_device_sptr&         device,
                                               const boxm2_opencl_cache_sptr&  opencl_cache,
                                               const vil_image_view_base_sptr& hmap_mean,
                                               const vil_image_view_base_sptr& hmap_var,
                                               const vil_image_view_base_sptr& ximg,
                                               const vil_image_view_base_sptr& yimg,
                                               int smoothingradius,
                                               float resnearfactor = 100000.0,
                                               float resfarfactor = 100000.0);
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_smooth_heightmap_pdata_kernels(const bocl_device_sptr& device, const std::string& opts = "");
    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > smooth_heightmap_pdata_kernels_;
};

class boxm2_ocl_compute_heightmap_pre_post
{

public:
    static bool compute_pre_post(const boxm2_scene_sptr&         scene,
        const bocl_device_sptr&         device,
        const boxm2_opencl_cache_sptr&  opencl_cache,
        const vil_image_view_base_sptr& hmap_mean,
        const vil_image_view_base_sptr& hmap_var,
        const vil_image_view_base_sptr& ximg,
        const vil_image_view_base_sptr& yimg,
        int smoothingradius = 16,
        float resnearfactor = 100000.0,
        float resfarfactor = 100000.0);
private:

    static bool update_pre(const boxm2_scene_sptr&         scene,
        const bocl_device_sptr&         device,
        const boxm2_opencl_cache_sptr&  opencl_cache,
        const vil_image_view_base_sptr& ximg,
        const vil_image_view_base_sptr& yimg,
        float resnearfactor = 100000.0,
        float resfarfactor = 100000.0);
    static bool update_post(const boxm2_scene_sptr&         scene,
        const bocl_device_sptr&         device,
        const boxm2_opencl_cache_sptr&  opencl_cache,
        const vil_image_view_base_sptr& ximg,
        const vil_image_view_base_sptr& yimg,
        float resnearfactor = 100000.0,
        float resfarfactor = 100000.0);

    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_pre_kernels(const bocl_device_sptr& device, const std::string& opts = "");
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_post_kernels(const bocl_device_sptr& device, const std::string& opts = "");

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > pre_kernels_;

    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > post_kernels_;


};

class boxm2_ocl_update_heightmap_factor
{
public:
    static bool update_heightmap_factor(const boxm2_scene_sptr&         scene,
        const bocl_device_sptr&         device,
        const boxm2_opencl_cache_sptr&  opencl_cache,
        bool add);
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_update_heightmap_factor_kernels(const bocl_device_sptr& device, const std::string& opts = "");
    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > update_heightmap_factor_kernels_;
};


#endif //boxm2_ocl_update_heightmap_factor_h_
