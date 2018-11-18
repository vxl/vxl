// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_image_density_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process to compute the density for each pixel in an image given a scene model
//
// \author Ozge C. Ozcanli
// \date July 07, 2011

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
#include <boxm2/cpp/algo/boxm2_image_density_functor.h>

#include <vil/vil_save.h>
#include <bsta/bsta_gauss_sf1.h>

//directory utility
#include <vcl_where_root_dir.h>


namespace boxm2_cpp_image_density_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 2;
  std::size_t lthreads[2]={8,8};
}

bool boxm2_cpp_image_density_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_image_density_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vil_image_view_base_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vcl_string";// if identifier string is empty, then only one appearance model

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "float";  // the cumulative normalized density measure for the image

  bool good = pro.set_input_types(input_types_) &&
    pro.set_output_types(output_types_);
  // in case the 4th input is not set
  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  pro.set_input(4, idx);
  return good;
}

bool boxm2_cpp_image_density_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_image_density_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr float_image=boxm2_util::prepare_input_image(img);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
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
    std::cout<<"BOXM2_CPP_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }

  if (identifier.size() > 0) {
    data_type += "_" + identifier;
  }
  if (auto * input_image=dynamic_cast<vil_image_view<float> * > (float_image.ptr()))
  {
    std::vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
    if (vis_order.empty())
    {
      std::cout<<" None of the blocks are visible from this viewpoint"<<std::endl;
      return true;
    }

    // function call
    auto * density_img=new vil_image_view<float>(input_image->ni(),input_image->nj());
    auto * vis_img=new vil_image_view<float>(input_image->ni(),input_image->nj());
    density_img->fill(0.0f);
    vis_img->fill(1.0f);

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

      if ( data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != std::string::npos )
      {
        boxm2_image_density_functor<BOXM2_MOG3_GREY> render_functor;
        render_functor.init_data(datas, input_image, density_img, vis_img);
        cast_ray_per_block<boxm2_image_density_functor<BOXM2_MOG3_GREY> >
          (render_functor,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
      }
      else if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != std::string::npos )
      {
        std::cout << "Rendering using gauss grey!\n";
        std::cout.flush();
        boxm2_image_density_functor<BOXM2_GAUSS_GREY> render_functor;
        render_functor.init_data(datas, input_image, density_img, vis_img);
        cast_ray_per_block<boxm2_image_density_functor<BOXM2_GAUSS_GREY> >
          (render_functor,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
      }
    }
    // store scene smart pointer
    pro.set_output_val<vil_image_view_base_sptr>(0, density_img);
    //: use vis_inf to leave pixels with rays that do not intersect the model
    float sum = 0.0f;
    for (unsigned i = 0; i < input_image->ni(); i++)
      for (unsigned j = 0; j < input_image->nj(); j++)
        if ((*vis_img)(i,j) < 0.9999) // if it is 1.0 then the ray didn't intersect the world
          sum += (*density_img)(i,j);
    float image_mean = sum/(input_image->ni()*input_image->nj());
    pro.set_output_val<float>(1,image_mean);
  }

  return true;
}


namespace boxm2_cpp_image_density_masked_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 2;
  std::size_t lthreads[2]={8,8};
}

bool boxm2_cpp_image_density_masked_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_image_density_masked_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vil_image_view_base_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "float";
  input_types_[5] = "float";
  input_types_[6] = "float"; //threshold
  input_types_[7] = "vcl_string";// if identifier string is empty, then only one appearance model

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "float";  // the cumulative normalized density measure for the image

  bool good = pro.set_input_types(input_types_) &&
    pro.set_output_types(output_types_);
  // in case the 8th input is not set
  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  pro.set_input(7, idx);
  return good;
}

bool boxm2_cpp_image_density_masked_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_image_density_masked_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr float_image=boxm2_util::prepare_input_image(img);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  auto shadow_prior = pro.get_input<float>(i++);
  auto shadow_sigma = pro.get_input<float>(i++);
  auto shadow_thres = pro.get_input<float>(i++);
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
    std::cout<<"BOXM2_CPP_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }

  if (identifier.size() > 0) {
    data_type += "_" + identifier;
  }
  if (auto * input_image=dynamic_cast<vil_image_view<float> * > (float_image.ptr()))
  {
    std::vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
    if (vis_order.empty())
    {
      std::cout<<" None of the blocks are visible from this viewpoint"<<std::endl;
      return true;
    }

    // function call
    auto * density_img=new vil_image_view<float>(input_image->ni(),input_image->nj());
    auto * vis_img=new vil_image_view<float>(input_image->ni(),input_image->nj());
    density_img->fill(0.0f);
    vis_img->fill(1.0f);

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

      if ( data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != std::string::npos )
      {
        boxm2_image_density_functor<BOXM2_MOG3_GREY> render_functor;
        render_functor.init_data(datas, input_image, density_img, vis_img);
        cast_ray_per_block<boxm2_image_density_functor<BOXM2_MOG3_GREY> >
          (render_functor,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
      }
      else if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != std::string::npos )
      {
        std::cout << "Rendering using gauss grey!\n";
        std::cout.flush();
        boxm2_image_density_functor<BOXM2_GAUSS_GREY> render_functor;
        render_functor.init_data(datas, input_image, density_img, vis_img);
        cast_ray_per_block<boxm2_image_density_functor<BOXM2_GAUSS_GREY> >
          (render_functor,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
      }
    }
    // now mask the density image

    //: compute alternate appearance probability for each pixel in the image
    vil_image_view<float> alt_prob_img(input_image->ni(), input_image->nj());
    alt_prob_img.fill(0.0f);
    if (shadow_prior > 0.0f) {
      bsta_gauss_sf1 single_mode(0.0f, shadow_sigma*shadow_sigma);
      for (unsigned i = 0; i < input_image->ni(); i++) {
        for (unsigned j = 0; j < input_image->nj(); j++) {
          alt_prob_img(i,j) = 2*single_mode.prob_density((*input_image)(i,j))*shadow_prior;
        }
      }
      vil_save(alt_prob_img, "shadow_density_img.tiff");
    }
    //: now threshold the density to generate a mask image
    for (unsigned i = 0; i < input_image->ni(); i++)
      for (unsigned j = 0; j < input_image->nj(); j++)
        if (alt_prob_img(i,j) < shadow_thres)  // shadow regions have high density values,
          alt_prob_img(i,j) = 1.0f;
        else
          alt_prob_img(i,j) = 0.0f;

    std::cout << "saving shadow density mask image\n";
    vil_save(alt_prob_img, "shadow_density_mask_img.tiff");

    // store scene smart pointer
    pro.set_output_val<vil_image_view_base_sptr>(0, density_img);

    //: use vis_inf to leave pixels with rays that do not intersect the model
    float sum = 0.0f;
    for (unsigned i = 0; i < input_image->ni(); i++)
      for (unsigned j = 0; j < input_image->nj(); j++)
        if ((*vis_img)(i,j) < 0.9999 && alt_prob_img(i,j) > 0.9) // if it is 1.0 then the ray didn't intersect the world
          sum += (*density_img)(i,j);
    float image_mean = sum/(input_image->ni()*input_image->nj());
    pro.set_output_val<float>(1,image_mean);
  }

  return true;
}
