// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_max_vis_score_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the scene.
//
// \author Vishal Jain
// \date Mar 25, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>



#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/ocl/algo/boxm2_ocl_update_max_vis.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vul/vul_timer.h>

namespace boxm2_ocl_update_max_vis_score_process_globals
{
  constexpr unsigned int n_inputs_ = 9;
  constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_update_max_vis_score_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_max_vis_score_process_globals;

  //process takes 9 inputs (of which the four last ones are optional):
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";      //input camera
  input_types_[4] = "unsigned";      //input camera
  input_types_[5] = "unsigned";      //input camera
  input_types_[6] = "vil_image_view_base_sptr";     //mask image view
  input_types_[7] = "float";                        // near factor ( maximum # of pixels should map to the finest voxel )
  input_types_[8] = "float";                        // far factor ( minimum # of pixels should map to the finest voxel )
  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  brdb_value_sptr tnearfactor   = new brdb_value_t<float>(1e8f);  //by default update alpha
  brdb_value_sptr tfarfactor   = new brdb_value_t<float>(1e-8f);  //by default update alpha
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  pro.set_input(7, tnearfactor);
  pro.set_input(8, tfarfactor);
  pro.set_input(6, empty_mask);
  return good;
}

bool boxm2_ocl_update_max_vis_score_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_max_vis_score_process_globals;

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned int i = 0;
  bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr         scene        = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr  cam          = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto ni          = pro.get_input<unsigned int>(i++);
  auto nj          = pro.get_input<unsigned int>(i++);
  vil_image_view_base_sptr mask_img     = pro.get_input<vil_image_view_base_sptr>(i++);

  auto                    nearfactor   = pro.get_input<float>(i++);
  auto                    farfactor    = pro.get_input<float>(i++);
  vul_timer t;
  t.mark();
  boxm2_ocl_update_max_vis::update_max_vis(scene, device, opencl_cache, cam, ni,nj,mask_img,nearfactor,farfactor);
  std::cout<<"Total time taken is "<<t.all()<<std::endl;
  return true;
}



namespace boxm2_ocl_update_view_normal_dot_process_globals
{
  constexpr unsigned int n_inputs_ = 9;
  constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_update_view_normal_dot_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_view_normal_dot_process_globals;

  //process takes 9 inputs (of which the four last ones are optional):
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";      //input camera
  input_types_[4] = "unsigned";      //input camera
  input_types_[5] = "unsigned";      //input camera
  input_types_[6] = "vil_image_view_base_sptr";     //mask image view
  input_types_[7] = "float";                        // near factor ( maximum # of pixels should map to the finest voxel )
  input_types_[8] = "float";                        // far factor ( minimum # of pixels should map to the finest voxel )
  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  brdb_value_sptr tnearfactor   = new brdb_value_t<float>(1e8f);  //by default update alpha
  brdb_value_sptr tfarfactor   = new brdb_value_t<float>(1e-8f);  //by default update alpha
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  pro.set_input(7, tnearfactor);
  pro.set_input(8, tfarfactor);
  pro.set_input(6, empty_mask);
  return good;
}

bool boxm2_ocl_update_view_normal_dot_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_view_normal_dot_process_globals;

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned int i = 0;
  bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr         scene        = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr  cam          = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto ni                       = pro.get_input<unsigned int>(i++);
  auto nj                       = pro.get_input<unsigned int>(i++);
  vil_image_view_base_sptr mask_img     = pro.get_input<vil_image_view_base_sptr>(i++);
  auto                    nearfactor   = pro.get_input<float>(i++);
  auto                    farfactor    = pro.get_input<float>(i++);
  vul_timer t;
  t.mark();
  boxm2_ocl_update_cosine_angle::update_cosine_angle(scene, device, opencl_cache, cam, ni,nj,mask_img,nearfactor,farfactor);
  std::cout<<"Total time taken is "<<t.all()<<std::endl;
  return true;
}

namespace boxm2_ocl_update_surface_density_process_globals
{
  constexpr unsigned int n_inputs_ = 10;
  constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_update_surface_density_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_surface_density_process_globals;

  //process takes 9 inputs (of which the four last ones are optional):
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";      //input camera
  input_types_[4] = "unsigned";      //input camera
  input_types_[5] = "unsigned";      //input camera
  input_types_[6] = "vil_image_view_base_sptr";     //depth image view
  input_types_[7] = "vil_image_view_base_sptr";     //std depth image view
  input_types_[8] = "float";                        // near factor ( maximum # of pixels should map to the finest voxel )
  input_types_[9] = "float";                        // far factor ( minimum # of pixels should map to the finest voxel )
  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  brdb_value_sptr tnearfactor   = new brdb_value_t<float>(1e8f);  //by default update alpha
  brdb_value_sptr tfarfactor   = new brdb_value_t<float>(1e-8f);  //by default update alpha
  pro.set_input(8, tnearfactor);
  pro.set_input(9, tfarfactor);

  return good;
}

bool boxm2_ocl_update_surface_density_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_view_normal_dot_process_globals;

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned int i = 0;
  bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr         scene        = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr  cam          = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto ni                       = pro.get_input<unsigned int>(i++);
  auto nj                       = pro.get_input<unsigned int>(i++);
  vil_image_view_base_sptr exp_depth_img     = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr std_depth_img     = pro.get_input<vil_image_view_base_sptr>(i++);

  auto                    nearfactor   = pro.get_input<float>(i++);
  auto                    farfactor    = pro.get_input<float>(i++);
  vul_timer t;
  t.mark();

  if(auto * exp_depth_float_img = dynamic_cast<vil_image_view<float> *>(exp_depth_img.ptr()) )
      if(auto * std_depth_float_img = dynamic_cast<vil_image_view<float> *>(std_depth_img.ptr()) )
            boxm2_ocl_update_surface_density::update_surface_density(scene, device, opencl_cache, cam, ni,nj,*exp_depth_float_img,*std_depth_float_img,nearfactor,farfactor);
  std::cout<<"Total time taken is "<<t.all()<<std::endl;
  return true;
}
