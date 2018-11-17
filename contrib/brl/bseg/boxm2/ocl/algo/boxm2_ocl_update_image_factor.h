#ifndef boxm2_ocl_update_image_factor_h_
#define boxm2_ocl_update_image_factor_h_
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
class boxm2_ocl_compute_image_term
{

public:
    static bool compute_image_term(const boxm2_scene_sptr&         scene,
                                    bocl_device_sptr         device,
                                    const boxm2_opencl_cache_sptr&  opencl_cache,
                                    vpgl_camera_double_sptr  cam,
                                    const vil_image_view_base_sptr& img,
                                    const std::string& view_identifier,
                                    float resnearfactor = 100000.0,
                                    float resfarfactor = 100000.0);
private:



    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_image_kernels(const bocl_device_sptr& device, const std::string& opts = "");


    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > image_kernels_;



};
class boxm2_ocl_compute_pre_post
{

public:
    static bool compute_pre_post(const boxm2_scene_sptr&         scene,
        const bocl_device_sptr&         device,
        const boxm2_opencl_cache_sptr&  opencl_cache,
        const vpgl_camera_double_sptr&  cam,
        const vil_image_view_base_sptr& img,
        const std::string& view_identifier,
        float resnearfactor = 100000.0,
        float resfarfactor = 100000.0);
private:

    static bool update_pre(const boxm2_scene_sptr&         scene,
                           bocl_device_sptr         device,
                           const boxm2_opencl_cache_sptr&  opencl_cache,
                           vpgl_camera_double_sptr  cam,
                           unsigned int ni, unsigned int nj,
                           const std::string& view_identifier,
                           float resnearfactor = 100000.0,
                           float resfarfactor = 100000.0);
    static bool update_post(const boxm2_scene_sptr&         scene,
                            bocl_device_sptr         device,
                            const boxm2_opencl_cache_sptr&  opencl_cache,
                            vpgl_camera_double_sptr  cam,
                            unsigned int ni, unsigned int nj,
                            const std::string& view_identifier,
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

class boxm2_ocl_update_image_factor
{
public:
    static bool update_image_factor(const boxm2_scene_sptr&         scene,
        const bocl_device_sptr&         device,
        const boxm2_opencl_cache_sptr&  opencl_cache,
        bool add,
        const std::string& view_identifier);
    //compile kernels and place in static map
    static std::vector<bocl_kernel*>& get_update_image_factor_kernels(const bocl_device_sptr& device, const std::string& opts = "");
    //map of paint kernel by device
    static std::map<std::string, std::vector<bocl_kernel*> > update_image_factor_kernels_;
};


#endif //boxm2_ocl_update_image_factor_h_
