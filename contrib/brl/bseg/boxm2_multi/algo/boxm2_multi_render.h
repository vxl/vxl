#ifndef boxm2_multi_render_h
#define boxm2_multi_render_h
//:
// \file
#include <boxm2_multi_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>


//: boxm2_multi_cache - example realization of abstract cache class
class boxm2_multi_render
{
  public:
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;

  public:

    //: multi render header
    float render(boxm2_multi_cache&       cache,
                 vil_image_view<float>&   img,
                 vpgl_camera_double_sptr  cam );

  private:

    //: map keeps track of all kernels compiled and cached
    vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels_;

    //compile kernels and cache
    vcl_vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, vcl_string opts);

    //: pick out data type
    bool get_scene_appearances(boxm2_scene_sptr    scene,
                               vcl_string&         data_type,
                               vcl_string&         options,
                               int&                apptypesize);

    //: renders single image
    float render_scene( boxm2_scene_sptr scene,
                        bocl_device_sptr device,
                        boxm2_opencl_cache* opencl_cache,
                        cl_command_queue & queue,
                        vpgl_camera_double_sptr & cam,
                        bocl_mem_sptr & exp_image,
                        bocl_mem_sptr & vis_image,
                        bocl_mem_sptr & exp_img_dim,
                        vcl_string data_type,
                        bocl_kernel* kernel,
                        vcl_size_t * lthreads,
                        unsigned cl_ni,
                        unsigned cl_nj,
                        int apptypesize  );

    //:calls render block code
    float render_block( boxm2_scene_sptr& scene,
                        boxm2_block_id id,
                        boxm2_opencl_cache* opencl_cache,
                        cl_command_queue& queue,
                        bocl_mem_sptr & ray_o_buff,
                        bocl_mem_sptr & ray_d_buff,
                        bocl_mem_sptr & exp_image,
                        bocl_mem_sptr & vis_image,
                        bocl_mem_sptr & exp_img_dim,
                        bocl_mem_sptr & cl_output,
                        bocl_mem_sptr & lookup,
                        vcl_string data_type,
                        bocl_kernel* kern,
                        vcl_size_t* lthreads,
                        unsigned cl_ni,
                        unsigned cl_nj,
                        int apptypesize);

    template <class T>
    static inline T clamp(T x, T a, T b) { return x < a ? a : (x > b ? b : x); }
};

#endif
