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
                                               bocl_device_sptr         device,
                                               boxm2_opencl_cache_sptr  opencl_cache,
                                               vil_image_view_base_sptr hmap_mean,
                                               vil_image_view_base_sptr hmap_var,
                                               vil_image_view_base_sptr ximg,
                                               vil_image_view_base_sptr yimg,
                                               int smoothingradius,
                                               float resnearfactor = 100000.0,
                                               float resfarfactor = 100000.0);
    //compile kernels and place in static map
    static vcl_vector<bocl_kernel*>& get_smooth_heightmap_pdata_kernels(bocl_device_sptr device, vcl_string opts = "");
    //map of paint kernel by device
    static vcl_map<vcl_string, vcl_vector<bocl_kernel*> > smooth_heightmap_pdata_kernels_;
};

class boxm2_ocl_compute_heightmap_pre_post
{

public:
    static bool compute_pre_post(boxm2_scene_sptr         scene,
        bocl_device_sptr         device,
        boxm2_opencl_cache_sptr  opencl_cache,
        vil_image_view_base_sptr hmap_mean,
        vil_image_view_base_sptr hmap_var,
        vil_image_view_base_sptr ximg,
        vil_image_view_base_sptr yimg,
        int smoothingradius = 16,
        float resnearfactor = 100000.0,
        float resfarfactor = 100000.0);
private:

    static bool update_pre(boxm2_scene_sptr         scene,
        bocl_device_sptr         device,
        boxm2_opencl_cache_sptr  opencl_cache,
        vil_image_view_base_sptr ximg,
        vil_image_view_base_sptr yimg,
        float resnearfactor = 100000.0,
        float resfarfactor = 100000.0);
    static bool update_post(boxm2_scene_sptr         scene,
        bocl_device_sptr         device,
        boxm2_opencl_cache_sptr  opencl_cache,
        vil_image_view_base_sptr ximg,
        vil_image_view_base_sptr yimg,
        float resnearfactor = 100000.0,
        float resfarfactor = 100000.0);

    //compile kernels and place in static map
    static vcl_vector<bocl_kernel*>& get_pre_kernels(bocl_device_sptr device, vcl_string opts = "");
    //compile kernels and place in static map
    static vcl_vector<bocl_kernel*>& get_post_kernels(bocl_device_sptr device, vcl_string opts = "");

    //map of paint kernel by device
    static vcl_map<vcl_string, vcl_vector<bocl_kernel*> > pre_kernels_;

    //map of paint kernel by device
    static vcl_map<vcl_string, vcl_vector<bocl_kernel*> > post_kernels_;


};

class boxm2_ocl_update_heightmap_factor
{
public:
    static bool update_heightmap_factor(boxm2_scene_sptr         scene,
        bocl_device_sptr         device,
        boxm2_opencl_cache_sptr  opencl_cache,
        bool add);
    //compile kernels and place in static map
    static vcl_vector<bocl_kernel*>& get_update_heightmap_factor_kernels(bocl_device_sptr device, vcl_string opts = "");
    //map of paint kernel by device
    static vcl_map<vcl_string, vcl_vector<bocl_kernel*> > update_heightmap_factor_kernels_;
};


#endif //boxm2_ocl_update_heightmap_factor_h_
