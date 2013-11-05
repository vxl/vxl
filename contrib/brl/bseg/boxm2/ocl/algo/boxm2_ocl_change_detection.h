// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_change_detection.h
#ifndef boxm2_ocl_change_detection_h_
#define boxm2_ocl_change_detection_h_
//:
// \file
// \brief  A process for change detection
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vcl_sstream.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_change_detection_globals
{
  const float PROB_THRESH = 0.1f;
  bool get_scene_appearances(boxm2_scene_sptr    scene,
                             vcl_string&         data_type,
                             vcl_string&         num_obs_type,
                             vcl_string&         options,
                             int&                apptypesize);
};

//Older, single pass, multi-res change detection
class boxm2_ocl_change_detection
{
  public:
    static bool change_detect(vil_image_view<float>&    change_img,
                              vil_image_view<vxl_byte>& rgb_change_img,
                              bocl_device_sptr          device,
                              boxm2_scene_sptr          scene,
                              boxm2_opencl_cache_sptr   opencl_cache,
                              vpgl_camera_double_sptr   cam,
                              vil_image_view_base_sptr  img,
                              vil_image_view_base_sptr  exp_img,
                              int                       n,
                              vcl_string                norm_type,
                              bool                      pmax );

  private:
    static vcl_vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, vcl_string opts);
    static vcl_map<vcl_string, vcl_vector<bocl_kernel*> > kernels_;
    static void full_pyramid(vil_image_view_base_sptr in_img, float* img_buff, unsigned cl_ni, unsigned cl_nj);
    static double mutual_information_2d(const vnl_vector<double>& X, const vnl_vector<double>& Y, int nbins);
};


//: new change detection
//  The idea behind this change detection is to help smooth out detections where
//  the image resolution and the model resolution do not match.
//  First pass: find per-cell mean observation
//  second pass: run change detection w/ observation in per cell aux data
class boxm2_ocl_two_pass_change
{
  public:
    static bool change_detect(  vil_image_view<float>&    change_img,
                                bocl_device_sptr          device,
                                boxm2_scene_sptr          scene,
                                boxm2_opencl_cache_sptr   opencl_cache,
                                vpgl_camera_double_sptr   cam,
                                vil_image_view_base_sptr  img,
                                vil_image_view_base_sptr  exp_img,
                                int                       n,
                                vcl_string                norm_type,
                                bool                      pmax=false);

  private:
    static vcl_vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, vcl_string opts);
    static vcl_map<vcl_string, vcl_vector<bocl_kernel*> > kernels_;
};

class boxm2_ocl_aux_pass_change
{
  public:
    static bool change_detect(  vil_image_view<float>&    change_img,
                                vil_image_view<float>&    vis_img,
                                bocl_device_sptr          device,
                                boxm2_scene_sptr          scene,
                                boxm2_opencl_cache_sptr   opencl_cache,
                                vpgl_camera_double_sptr   cam,
                                vil_image_view_base_sptr  img,
                                bool max_density=false,float nearfactor = 100000.0f, float farfactor= 0.000001f);

  private:
    static vcl_vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, vcl_string opts, bool maxdensity =false);
    static vcl_map<vcl_string, vcl_vector<bocl_kernel*> > kernels_;
};
#endif // boxm2_ocl_change_detection_h_
