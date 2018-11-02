// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_multi_update_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the scene using multiple GPUs.
//
// \author Vishal Jain
// \date Aug 28, 2014

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2_multi/boxm2_multi_cache.h>
#include <boxm2_multi/algo/boxm2_multi_render.h>
#include <boxm2_multi/algo/boxm2_multi_update.h>
#include <boxm2_multi/algo/boxm2_multi_refine.h>

#include <vcl_where_root_dir.h>

//executable args
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vul/vul_timer.h>
#include <vul/vul_arg.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/ocl/boxm2_opencl_cache1.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_multi_update_process_globals
{
  constexpr unsigned int n_inputs_ = 10;
  constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_multi_update_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_multi_update_process_globals;

  //process takes 9 inputs (of which the four last ones are optional):
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_multi_cache_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";      //input camera
  input_types_[3] = "vil_image_view_base_sptr";     //input image
  input_types_[4] = "vcl_string";                   //illumination identifier
  input_types_[5] = "bool";                         //do_update_alpha/don't update alpha
  input_types_[6] = "float";                        //variance value? if 0.0 or less, then use variable variance
  input_types_[7] = "bool";                         //do_update_app/don't update alpha
  input_types_[8] = "float";                        // near factor ( maximum # of pixels should map to the finest voxel )
  input_types_[9] = "float";                        // far factor ( minimum # of pixels should map to the finest voxel )

  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  // default 5, 6 and 7 and 8 inputs
  brdb_value_sptr idx        = new brdb_value_t<std::string>("");
  brdb_value_sptr up_alpha   = new brdb_value_t<bool>(true);  //by default update alpha
  brdb_value_sptr def_var    = new brdb_value_t<float>(-1.0f);
  brdb_value_sptr up_app   = new brdb_value_t<bool>(true);  //by default update alpha
  brdb_value_sptr tnearfactor   = new brdb_value_t<float>(1e6f);  //by default update alpha
  brdb_value_sptr tfarfactor   = new brdb_value_t<float>(1e-6f);  //by default update alpha
  pro.set_input(4, idx);
  pro.set_input(5, up_alpha);
  pro.set_input(6, def_var);
  pro.set_input(7, up_app);
  pro.set_input(8, tnearfactor);
  pro.set_input(9, tfarfactor);
  return good;
}

bool boxm2_multi_update_process(bprb_func_process& pro)
{
  using namespace boxm2_multi_update_process_globals;

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned int i = 0;
  boxm2_multi_cache_sptr   multi_cache  = pro.get_input<boxm2_multi_cache_sptr>(i++);
  boxm2_scene_sptr         scene        = pro.get_input<boxm2_scene_sptr>(i++);
  vpgl_camera_double_sptr  cam          = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img          = pro.get_input<vil_image_view_base_sptr>(i++);
  std::string               ident        = pro.get_input<std::string>(i++);
  bool                     update_alpha = pro.get_input<bool>(i++);
  auto                    mog_var      = pro.get_input<float>(i++);
  bool                     update_app   = pro.get_input<bool>(i++);
  auto                    nearfactor   = pro.get_input<float>(i++);
  auto                    farfactor    = pro.get_input<float>(i++);
  vul_timer t; t.mark();

  vil_image_view_base_sptr inImg = boxm2_util::prepare_input_image(img, true);
  auto* inImgPtr = dynamic_cast<vil_image_view<float>* >(inImg.ptr());

  float gpu_time = boxm2_multi_update::update(*(multi_cache.ptr()), *inImgPtr, cam);
  float total = t.all();
  std::cout<<"  ===> Total update time: "<<total<<" ms\n"
      <<"  ===> total GPU time   : "<<gpu_time<<" ms\n"
      <<"  ===> total gpu / total: "<<gpu_time/total<<std::endl;

  std::cout<<"Total time taken is "<<t.all()<<std::endl;

  return false;
}
