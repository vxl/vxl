// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_multi_render_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for rendering the scene.
//
// \author Vishal Jain
// \date Aug 28, 2014

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2_multi/boxm2_multi_cache.h>
#include <boxm2_multi/algo/boxm2_multi_render.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vil/vil_image_view.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vul/vul_timer.h>


namespace boxm2_multi_render_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 1;
}

bool boxm2_multi_render_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_multi_render_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_multi_cache_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  input_types_[3] = "unsigned";
  input_types_[4] = "unsigned";
  input_types_[5] = "vcl_string";
  input_types_[6] = "float";                        // near factor ( maximum # of pixels should map to the finest voxel )
  input_types_[7] = "float";                        // far factor ( minimum # of pixels should map to the finest voxel )

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
    bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  // in case the 7th input is not set
  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  brdb_value_sptr tnearfactor = new brdb_value_t<float>(100000.0f);  //by default update alpha
  brdb_value_sptr tfarfactor = new brdb_value_t<float>(100000.0f);  //by default update alpha

  pro.set_input(5, idx);
  pro.set_input(6, tnearfactor);
  pro.set_input(7, tfarfactor);

  return good;
}

bool boxm2_multi_render_process(bprb_func_process& pro)
{
  using namespace boxm2_multi_render_process_globals;

  vul_timer rtime;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_multi_cache_sptr multi_cache= pro.get_input<boxm2_multi_cache_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  auto ni=pro.get_input<unsigned>(i++);
  auto nj=pro.get_input<unsigned>(i++);
  std::string ident = pro.get_input<std::string>(i++);
  auto   nearfactor = pro.get_input<float>(i++);
  auto   farfactor = pro.get_input<float>(i++);
  boxm2_multi_render renderer;
  auto* exp_img_out=new vil_image_view<float>(ni,nj);
  float gpu_time = renderer.render(*(multi_cache.ptr()), *exp_img_out, cam);

  i=0;
  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(i++, exp_img_out);

  std::cout<<"Multi Cache:\n"<<multi_cache->to_string()<<std::endl;
  return true;
}
