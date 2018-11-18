// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_batch_update_opt2_processes.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  Processes to update the scene using a set of data blocks in a batch mode, migrated from the version in boxm to boxm2
//
// \author Ozge C. Ozcanli
// \date May 12, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/boxm2_util.h>

#include <boxm2/cpp/algo/boxm2_batch_functors.h>
#include <boxm2/cpp/algo/boxm2_batch_opt2_functors.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>

#include <vil/vil_math.h>
#include <bsta/bsta_gauss_sf1.h>

#include <vil/vil_save.h>

//: create a normalized intensity value in each cell using the segment lengths of all the rays that intersect the cell
namespace boxm2_cpp_create_aux_data_opt2_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_create_aux_data_opt2_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_create_aux_data_opt2_process_globals;

  //process takes 7 inputs
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
  input_types_[5] = "float"; // alternative appearance model of shadow, prior
  input_types_[6] = "float"; // alternative appearance model of shadow, sigma
  // process has 0 output:
  std::vector<std::string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  brdb_value_sptr pp = new brdb_value_t<float>(0.0f);
  pro.set_input(5, pp);
  brdb_value_sptr ps = new brdb_value_t<float>(0.0f);
  pro.set_input(6, ps);
  return good;
}

bool boxm2_cpp_create_aux_data_opt2_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_create_aux_data_opt2_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr in_img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr float_image = boxm2_util::prepare_input_image(in_img, true);
  std::string identifier = pro.get_input<std::string>(i++);
  auto shadow_prior = pro.get_input<float>(i++);
  auto shadow_sigma = pro.get_input<float>(i++);

  if (auto * input_image=dynamic_cast<vil_image_view<float> * > (float_image.ptr()))
  {
    std::vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
    if (vis_order.empty())
    {
      std::cout<<" None of the blocks are visible from this viewpoint"<<std::endl;
      return true;
    }

    bool foundDataType = false;

    std::string data_type;
    std::string num_obs_type;
    std::vector<std::string> apps = scene->appearances();
    for (const auto & app : apps) {
      if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
      {
          data_type = app;
          foundDataType = true;
          // boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
      }
      else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
      {
          data_type = app;
          foundDataType = true;
          // boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix());
      }
      else if ( app == boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix() )
      {
          data_type = app;
          foundDataType = true;
          // boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix());
      }
    }
    if (!foundDataType) {
      std::cerr<<"boxm2_cpp_create_aux_data_process ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type\n";
      return false;
    }

    vil_image_view<float> pre_inf_img(input_image->ni(),input_image->nj());
    vil_image_view<float> vis_inf_img(input_image->ni(),input_image->nj());
    //initialize pre and vis images
    pre_inf_img.fill(0.0f);
    vis_inf_img.fill(1.0f);

    std::vector<boxm2_block_id>::iterator id;

    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      std::cout<<"Block id "<<(*id)<<' ';
      boxm2_block *     blk = cache->get_block(scene,*id);
      boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
      boxm2_data_base *  mog = cache->get_data_base(scene,*id,data_type,0,false);
      boxm2_data_base *aux0 = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(identifier));
      boxm2_data_base *aux1 = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix(identifier));

      std::vector<boxm2_data_base*> datas;
      datas.push_back(aux0);
      datas.push_back(aux1);
      datas.push_back(alph);
      datas.push_back(mog);
      auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
      scene_info_wrapper->info=scene->get_blk_metadata(*id);

      if ( data_type == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
      {
        boxm2_batch_update_pass1_functor<BOXM2_MOG3_GREY> pass1;
        pass1.init_data(datas,&pre_inf_img,&vis_inf_img);
        cast_ray_per_block<boxm2_batch_update_pass1_functor<BOXM2_MOG3_GREY> >(pass1,
                                                                               scene_info_wrapper->info,
                                                                               blk,
                                                                               cam,
                                                                               input_image->ni(),
                                                                               input_image->nj());
      }
      else if ( data_type == boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix() )
      {
        boxm2_batch_update_pass1_functor<BOXM2_GAUSS_GREY> pass1;
        pass1.init_data(datas,&pre_inf_img,&vis_inf_img);
        cast_ray_per_block<boxm2_batch_update_pass1_functor<BOXM2_GAUSS_GREY> >(pass1,
                                                                                scene_info_wrapper->info,
                                                                                blk,
                                                                                cam,
                                                                                input_image->ni(),
                                                                                input_image->nj());
      }
    }
    // compute beta denominator
    vil_image_view<float> beta_denom_img(input_image->ni(), input_image->nj());
    // pre_inf + vis * 1 // assume PI = 1 for all the colors (uniform distribution)
    vil_math_image_sum<float,float,float>(pre_inf_img,vis_inf_img,beta_denom_img);

    // compute model prior --> different than 1 only if there are alternate appearance models like shadow appearance
    float model_prior = 1.0f - shadow_prior;
    // sanity check
    if (model_prior <= 0.0f) {
      std::cerr << "error: boxm_generate_opt2_samples : alt_appearance_priors sum to " << 1.0f - model_prior << " >= 1.0!\n";
    }

    // compute alternate appearance probability for each pixel in the image
    vil_image_view<float> alt_prob_img(input_image->ni(), input_image->nj());
    alt_prob_img.fill(0.0f);
    if (shadow_prior > 0.0f) {
      bsta_gauss_sf1 single_mode(0.0f, shadow_sigma*shadow_sigma);
      for (unsigned i = 0; i < input_image->ni(); i++) {
        for (unsigned j = 0; j < input_image->nj(); j++) {
          alt_prob_img(i,j) = single_mode.prob_density((*input_image)(i,j))*shadow_prior;
        }
      }
#if 0
      vil_save(alt_prob_img, "shadow_density_img.tiff");
#endif
    }

    // now run pass 2

    vil_image_view<float> pre_img(input_image->ni(),input_image->nj());
    vil_image_view<float> vis_img(input_image->ni(),input_image->nj());
    pre_img.fill(0.0f);
    vis_img.fill(1.0f);

    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      std::cout<<"Block id "<<(*id)<<' ';
      boxm2_block *     blk = cache->get_block(scene,*id);
      boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
      boxm2_data_base *  mog = cache->get_data_base(scene,*id,data_type,0,false);
      boxm2_data_base *aux0 = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(identifier));
      boxm2_data_base *aux1 = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix(identifier));

      // generate aux in a writable mode
      boxm2_data_base *aux = cache->get_data_base_new(scene,*id, boxm2_data_traits<BOXM2_AUX>::prefix(identifier),false);

      std::vector<boxm2_data_base*> datas;
      datas.push_back(aux0);
      datas.push_back(aux1);
      datas.push_back(alph);
      datas.push_back(mog);
      datas.push_back(aux);
      auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
      scene_info_wrapper->info=scene->get_blk_metadata(*id);

      if ( data_type == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
      {
        boxm2_batch_update_opt2_pass2_functor<BOXM2_MOG3_GREY> pass2;
        pass2.init_data(datas,&pre_img,&vis_img, &beta_denom_img, model_prior, &alt_prob_img);
        cast_ray_per_block<boxm2_batch_update_opt2_pass2_functor<BOXM2_MOG3_GREY> >(pass2,
                                                                                    scene_info_wrapper->info,
                                                                                    blk,
                                                                                    cam,
                                                                                    input_image->ni(),
                                                                                    input_image->nj());
      }
      else if ( data_type == boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix() )
      {
        boxm2_batch_update_opt2_pass2_functor<BOXM2_GAUSS_GREY> pass2;
        pass2.init_data(datas,&pre_img,&vis_img, &beta_denom_img, model_prior, &alt_prob_img);
        cast_ray_per_block<boxm2_batch_update_opt2_pass2_functor<BOXM2_GAUSS_GREY> >(pass2,
                                                                                     scene_info_wrapper->info,
                                                                                     blk,
                                                                                     cam,
                                                                                     input_image->ni(),
                                                                                     input_image->nj());
      }
    }
  }

  return true;
}

//: run batch update
namespace boxm2_cpp_batch_update_opt2_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_batch_update_opt2_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_update_opt2_process_globals;

  //process takes 4 inputs
  // 0) scene
  // 1) cache
  // 2) stream cache
  // 3) the pre-computed sigma normalizer table, for fast access to normalizer values given number of images
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "boxm2_stream_cache_sptr";
  input_types_[3] = "bsta_sigma_normalizer_sptr";
  // process has no output:
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: moved over from boxm
bool boxm2_cpp_batch_update_opt2_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_update_opt2_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
      std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
      return false;
  }
  // get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  boxm2_stream_cache_sptr str_cache = pro.get_input<boxm2_stream_cache_sptr>(i++);
  bsta_sigma_normalizer_sptr n_table = pro.get_input<bsta_sigma_normalizer_sptr>(i++);

  std::string data_type;
  bool foundDataType = false;
  std::vector<std::string> apps = scene->appearances();
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      // boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    }
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      std::cout << "In boxm2_cpp_batch_update_process ERROR: datatype BOXM2_MOG3_GREY_16 not implemented!\n";
      return false;
    }
    else if ( app == boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      // boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix());
    }
  }
  if (!foundDataType) {
    std::cout<<"boxm2_cpp_batch_update_process ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }

  // assumes that the data of each image has been created in the data models previously
  int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
  // iterate the scene block by block and write to output
  std::vector<boxm2_block_id> blk_ids = scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator id;
  id = blk_ids.begin();
  for (id = blk_ids.begin(); id != blk_ids.end(); ++id) {
    boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
    boxm2_data_base *  mog = cache->get_data_base(scene,*id,data_type,0,false);

    if ( data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != std::string::npos )
    {
      boxm2_batch_update_opt2_functor<BOXM2_MOG3_GREY> data_functor;
      data_functor.init_data(alph, mog, str_cache, n_table);
      int data_buff_length = (int) (alph->buffer_length()/alphaTypeSize);
      boxm2_data_serial_iterator<boxm2_batch_update_opt2_functor<BOXM2_MOG3_GREY> >(data_buff_length,data_functor);
    }
    if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != std::string::npos )
    {
      boxm2_batch_update_opt2_functor<BOXM2_GAUSS_GREY> data_functor;
      data_functor.init_data(alph, mog, str_cache, n_table);
      int data_buff_length = (int) (alph->buffer_length()/alphaTypeSize);
      boxm2_data_serial_iterator<boxm2_batch_update_opt2_functor<BOXM2_GAUSS_GREY> >(data_buff_length,data_functor);
    }
  }

  return true;
}
