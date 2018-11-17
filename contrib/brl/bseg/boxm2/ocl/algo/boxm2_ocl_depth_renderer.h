#ifndef boxm2_ocl_depth_renderer_h_included_
#define boxm2_ocl_depth_renderer_h_included_

#include <boxm2/boxm2_scene.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vil/vil_image_view.h>

class boxm2_ocl_depth_renderer
{
  public:
    boxm2_ocl_depth_renderer(const boxm2_scene_sptr& scene,
                             const boxm2_opencl_cache_sptr& ocl_cache,
                             const std::string& ident="");
    ~boxm2_ocl_depth_renderer();

    bool render(vpgl_camera_double_sptr camera,
                unsigned ni,
                unsigned nj,
                float nearfactor=0.0f,
                float farfactor=1000000.0f);

    bool get_last_rendered(vil_image_view<float> &img);
    bool get_last_vis(vil_image_view<float> &vis_img);

  private:
    boxm2_scene_sptr scene_;
    bocl_device_sptr device_;
    boxm2_opencl_cache_sptr opencl_cache_;
    std::string data_type_;
    bool buffers_allocated_;
    bool compile_kernels(const bocl_device_sptr& device);
    bool cleanup_render_buffers();
    bool allocate_render_buffers(int cl_ni, int cl_nj);

    bocl_kernel depth_kern_;
    bocl_kernel depth_norm_kern_;

    bool render_success_;
    vil_image_view<float> depth_img_;
    vil_image_view<float> vis_img_;

    // persistant ocl and host buffers for efficiency
    cl_int img_dim_buff_[4];
    cl_float tnearfar_buff_[2];
    cl_float output_buff_[100];
    cl_uchar lookup_buff_[256];
    cl_float subblk_dim_;

    cl_int prev_ni_;
    cl_int prev_nj_;
    cl_float* depth_buff_;
    cl_float* vis_buff_;
    cl_float* prob_buff_;
    cl_float* var_buff_;
    cl_float* t_infinity_buff_;

    cl_float* ray_origins_buff_;
    cl_float* ray_directions_buff_;

    bocl_mem_sptr depth_image_;
    bocl_mem_sptr vis_image_;
    bocl_mem_sptr var_image_;
    bocl_mem_sptr prob_image_;
    bocl_mem_sptr t_infinity_image_;

    bocl_mem_sptr ray_origins_image_;
    bocl_mem_sptr ray_directions_image_;

    bocl_mem_sptr img_dim_;
    bocl_mem_sptr tnearfar_;
    bocl_mem_sptr cl_output_;
    bocl_mem_sptr lookup_;
    bocl_mem_sptr cl_subblk_dim_;
};

#endif
