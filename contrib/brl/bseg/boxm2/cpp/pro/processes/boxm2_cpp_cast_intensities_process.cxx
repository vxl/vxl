// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_cast_intensities_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for casting intensities of an image along the rays of its camera into the world.
//
// \author Ozge C. Ozcanli
// \date May 04, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_image_view.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/cpp/algo/boxm2_cast_intensities_functor.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_cast_intensities_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_cast_intensities_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_cast_intensities_process_globals;

  //process takes 5 inputs
  // 0) scene
  // 1) cache
  // 2) camera
  // 3) image
  // 4) image identifier
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  input_types_[3] = "vil_image_view_base_sptr";
  input_types_[4] = "vcl_string";  //image identifier
  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_cast_intensities_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_cast_intensities_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr in_img=pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr float_image=boxm2_util::prepare_input_image(in_img);
  std::string identifier = pro.get_input<std::string>(i++);

  if (auto * input_image=dynamic_cast<vil_image_view<float> * > (float_image.ptr()))
  {
    std::vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
    if (vis_order.empty())
    {
      std::cout<<" None of the blocks are visible from this viewpoint"<<std::endl;
      return true;
    }

    bool success=true;
    boxm2_cast_intensities_functor pass;

    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      std::cout<<"Block id "<<(*id)<<' ';
      boxm2_block *   blk = cache->get_block(scene,*id);

      //: first make sure that the database is removed from memory if it already exists
      cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(identifier));
      //: now retrieve it with get_data_base_new method so that even if it exists on disc, a fresh one will be created
      boxm2_data_base *  alph = cache->get_data_base_new(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(identifier), false);

      auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
      scene_info_wrapper->info=scene->get_blk_metadata(*id);
      pass.init_data(alph,input_image);
      success=success && cast_ray_per_block<boxm2_cast_intensities_functor>(pass,
                                                                            scene_info_wrapper->info,
                                                                            blk,
                                                                            cam,
                                                                            input_image->ni(),
                                                                            input_image->nj());
    }
    return true;
  }
  else
    return false;
}
