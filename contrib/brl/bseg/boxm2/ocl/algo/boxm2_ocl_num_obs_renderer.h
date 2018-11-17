#ifndef boxm2_ocl_num_obs_renderer_h_included_
#define boxm2_ocl_num_obs_renderer_h_included_

#include <boxm2/boxm2_scene.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vil/vil_image_view.h>

class boxm2_ocl_num_obs_renderer
{
  public:
    boxm2_ocl_num_obs_renderer(const boxm2_scene_sptr& scene,
                               const boxm2_opencl_cache_sptr& ocl_cache,
                               const std::string& ident="");
    ~boxm2_ocl_num_obs_renderer();

    bool render(vpgl_camera_double_sptr camera, unsigned ni, unsigned nj, float nearfactor=0.0f, float farfactor=1000000.0f);

    bool get_last_rendered(vil_image_view<float> &img);
    bool get_last_vis(vil_image_view<float> &vis_img);

  private:
    boxm2_scene_sptr scene_;
    bocl_device_sptr device_;
    boxm2_opencl_cache_sptr opencl_cache_;
    std::string nobs_type_;
    bool buffers_allocated_;
    int nobs_type_size_;
    std::vector<bocl_kernel*> kernels_;
    bool compile_kernels(const bocl_device_sptr& device, std::vector<bocl_kernel*> & vec_kernels, boxm2_data_type data_type);
    bool cleanup_render_buffers();
    bool allocate_render_buffers(int cl_ni, int cl_nj);

    bool render_success_;
    vil_image_view<float> expected_nobs_image_;
    vil_image_view<float> vis_img_;

    // persistant ocl and host buffers for efficiency
    cl_int img_dim_buff_[4];
    cl_float tnearfar_buff_[2];

    cl_int prev_ni_;
    cl_int prev_nj_;
    cl_float* img_buff_;
    cl_float* vis_buff_;
    cl_float* max_omega_buff_;
    bocl_mem_sptr exp_nobs_image_;
    bocl_mem_sptr vis_image_;
    bocl_mem_sptr max_omega_image_;
    bocl_mem_sptr img_dim_;
    bocl_mem_sptr tnearfar_;
};

#endif
