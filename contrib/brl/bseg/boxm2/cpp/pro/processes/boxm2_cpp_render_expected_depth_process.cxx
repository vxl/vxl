// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_render_expected_depth_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for rendering the depth image of the scene.
//
// \author Ozge C. Ozcanli
// \date May 3, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_render_functions.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_render_expected_depth_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 1;
  std::size_t lthreads[2]={8,8};
}

bool boxm2_cpp_render_expected_depth_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_render_expected_depth_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  input_types_[3] = "unsigned";
  input_types_[4] = "unsigned";


  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_render_expected_depth_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_render_expected_depth_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  auto ni=pro.get_input<unsigned>(i++);
  auto nj=pro.get_input<unsigned>(i++);

  // function call
  auto * exp_img=new vil_image_view<float>(ni,nj);
  auto * vis_img=new vil_image_view<float>(ni,nj);
  auto * len_img=new vil_image_view<float>(ni,nj);
  exp_img->fill(0.0f);
  vis_img->fill(1.0f);
  len_img->fill(0.0f);
  std::vector<boxm2_block_id> vis_order=scene->get_vis_blocks((vpgl_generic_camera<double>*)(cam.ptr()));
  std::vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    std::cout<<"Block Id "<<(*id)<<std::endl;
    boxm2_block *     blk = cache->get_block(scene,*id);
    boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());

    auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
    scene_info_wrapper->info=scene->get_blk_metadata(*id);

    boxm2_render_expected_depth(scene_info_wrapper->info,
                                blk,alph,cam,exp_img,vis_img,len_img,ni,nj);
  }

  float min_val, max_val;
  vil_math_value_range(*exp_img, min_val, max_val);
  vil_math_scale_values(*exp_img, 1/max_val);
  vil_math_value_range(*exp_img, min_val, max_val);

  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(0, exp_img);
  return true;
}
