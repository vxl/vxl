// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_render_cone_expected_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for rendering the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <vcl_fstream.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/cpp/algo/boxm2_render_functions.h>

//vil includes
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_transform.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_render_cone_expected_image_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 1;
  vcl_size_t lthreads[2]={8,8};
}

bool boxm2_cpp_render_cone_expected_image_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_render_cone_expected_image_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  input_types_[3] = "unsigned";
  input_types_[4] = "unsigned";


  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_render_cone_expected_image_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_render_cone_expected_image_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  unsigned ni=pro.get_input<unsigned>(i++);
  unsigned nj=pro.get_input<unsigned>(i++);

  //make sure the scene corresponds to this datatype
  vcl_string data_type, num_obs_type, options;
  if ( scene->has_data_type(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) ) {
    data_type = boxm2_data_traits<BOXM2_MOG3_GREY>::prefix();
    options=" -D MOG_TYPE_8 ";
  }
  else {
    vcl_cout<<"boxm2_cpp_render_cone_process ERROR: scene doesn't have BOXM2_GAUSS_RGB data type"<<vcl_endl;
    return false;
  }

  // function call
  vil_image_view<float> * exp_img = new vil_image_view<float>(ni,nj);
  vil_image_view<float> * vis_img = new vil_image_view<float>(ni,nj);
  exp_img->fill(0.0f);
  vis_img->fill(1.0f);
  vcl_vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_perspective_camera<double>*>(cam.ptr()));
  vcl_vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    vcl_cout<<"Cone Rendering Block Id "<<(*id)<<vcl_endl;
    boxm2_block *      blk  = cache->get_block(*id);
    boxm2_data_base *  alph = cache->get_data_base(*id,boxm2_data_traits<BOXM2_GAMMA>::prefix());
    boxm2_data_base *  mog  = cache->get_data_base(*id,data_type);
    vcl_vector<boxm2_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);

    boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
    scene_info_wrapper->info=scene->get_blk_metadata(*id);

    boxm2_render_cone_exp_image(scene_info_wrapper->info,
                                blk,datas,cam,exp_img,vis_img,ni,nj);
  }

  //normalize the expected image...
  normalize_intensity f;
  vil_transform2<float,float, normalize_intensity>(*vis_img,*exp_img,f);

  vil_save(*vis_img, "f:/vis_img.tiff");

  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(0, exp_img);
  return true;
}
