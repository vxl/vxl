// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_render_expected_image_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for rendering the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_transform.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_render_functions.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_render_expected_image_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 1;
  std::size_t lthreads[2]={8,8};
}

bool boxm2_cpp_render_expected_image_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_render_expected_image_process_globals;

  //process takes 6 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  input_types_[3] = "unsigned";
  input_types_[4] = "unsigned";
  input_types_[5] = "vcl_string";// if identifier string is empty, then only one appearance model

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";

  bool good = pro.set_input_types(input_types_) &&
    pro.set_output_types(output_types_);
  // in case the 6th input is not set
  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  pro.set_input(5, idx);
  return good;
}

bool boxm2_cpp_render_expected_image_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_render_expected_image_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  auto ni=pro.get_input<unsigned>(i++);
  auto nj=pro.get_input<unsigned>(i++);
  std::string identifier = pro.get_input<std::string>(i);

  bool foundDataType = false;
  std::string data_type;
  std::vector<std::string> apps = scene->appearances();
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
    }
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = app;
      foundDataType = true;
    }
    else if ( app == boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
    }
  }
  if (!foundDataType) {
    std::cerr<<"BOXM2_CPP_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type\n";
    return false;
  }

  if (identifier.size() > 0) {
    data_type += "_" + identifier;
  }

  // function call
  auto * exp_img=new vil_image_view<float>(ni,nj);
  auto * vis_img=new vil_image_view<float>(ni,nj);
  exp_img->fill(0.0f);
  vis_img->fill(1.0f);
  std::vector<boxm2_block_id> vis_order;
  if (auto* pcam = // assignment, not comparison
      dynamic_cast<vpgl_perspective_camera<double>* >(cam.ptr()))
  {
    vis_order=scene->get_vis_blocks(pcam);
  }
  else
  {
    vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
  }
  std::vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    std::cout<<"Block Id "<<(*id)<<std::endl;
    boxm2_block *     blk = cache->get_block(scene,*id);
    boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data_base *  mog = cache->get_data_base(scene,*id,data_type);
    std::vector<boxm2_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);

    auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
    scene_info_wrapper->info=scene->get_blk_metadata(*id);
    //scene_info_wrapper->info->tree_buffer_length = blk->tree_buff_length();
    //scene_info_wrapper->info->data_buffer_length = 65536;
    //scene_info_wrapper->info->num_buffer = blk->num_buffers();

    boxm2_render_expected_image(scene_info_wrapper->info,
                                blk,datas,cam,exp_img,vis_img,ni,nj,0,0,data_type);
  }

  normalize_intensity f;
  vil_transform2<float,float, normalize_intensity>(*vis_img,*exp_img,f);

  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(0, exp_img);
  return true;
}
