// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_change_detection_process2.cxx
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
#include <vil/vil_image_view.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_change_detection_functor.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_change_detection_process2_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 1;
}

bool boxm2_cpp_change_detection_process2_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_change_detection_process2_globals;

  //process takes 6 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  input_types_[3] = "vil_image_view_base_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vcl_string";// if identifier string is empty, then only one appearance model

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  // in case the 6th input is not set
  brdb_value_sptr idx = new brdb_value_t<vcl_string>("");
  pro.set_input(5, idx);
  return good;
}

bool boxm2_cpp_change_detection_process2(bprb_func_process& pro)
{
  using namespace boxm2_cpp_change_detection_process2_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr input_img=pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr exp_in_img=pro.get_input<vil_image_view_base_sptr>(i++);
  vcl_string identifier = pro.get_input<vcl_string>(i);

  bool foundDataType = false;
  vcl_string data_type;
  vcl_vector<vcl_string> apps = scene->appearances();
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
    }
  }
  if (!foundDataType) {
    vcl_cout<<"BOXM2_OCL_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }

  if (identifier.size() > 0) {
    data_type += "_" + identifier;
  }

  vil_image_view_base_sptr in_float_img=boxm2_util::prepare_input_image(input_img);
  if (vil_image_view<float> * in_img=dynamic_cast<vil_image_view<float> *> ( in_float_img.ptr()))
      if (vil_image_view<float> * exp_img=dynamic_cast<vil_image_view<float> *> ( exp_in_img.ptr()))
      {
          // function call
          vil_image_view<float> * vis_img=new vil_image_view<float>(in_img->ni(),in_img->nj());
          vis_img->fill(1.0f);
          vcl_vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
          if (vis_order.empty())
          {
              vcl_cout<<" None of the blocks are visible from this viewpoint"<<vcl_endl;
              return true;
          }
          boxm2_change_detection_with_uncertainity_functor cd_wu_functor(in_img->ni(),in_img->nj());

          vcl_vector<boxm2_block_id>::iterator id;
          for (id = vis_order.begin(); id != vis_order.end(); ++id)
          {
              boxm2_block *     blk  = cache->get_block(*id);
              boxm2_data_base *  alph = cache->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
              boxm2_data_base *  mog  = cache->get_data_base(*id,data_type);
              vcl_vector<boxm2_data_base*> datas;
              datas.push_back(alph);
              datas.push_back(mog);

              boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
              scene_info_wrapper->info=scene->get_blk_metadata(*id);

              cd_wu_functor.set_data(datas,in_img,exp_img);
              cast_ray_per_block<boxm2_change_detection_with_uncertainity_functor>(cd_wu_functor,scene_info_wrapper->info,blk,cam,in_img->ni(),in_img->nj());
          }
          cd_wu_functor.finish();
          // store scene smaprt pointer
          pro.set_output_val<vil_image_view_base_sptr>(0, exp_img);
      }
      return true;
}
