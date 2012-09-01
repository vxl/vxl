// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_image_density_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process to compute the density for each pixel in an image given a scene model
//
// \author Ozge C. Ozcanli
// \date July 07, 2011

#include <vcl_fstream.h>
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
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 2;
  vcl_size_t lthreads[2]={8,8};
}

bool boxm2_cpp_image_density_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_image_density_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vil_image_view_base_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vcl_string";// if identifier string is empty, then only one appearance model

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "float";  // the cumulative normalized density measure for the image

  bool good = pro.set_input_types(input_types_) &&
    pro.set_output_types(output_types_);
  // in case the 4th input is not set
  brdb_value_sptr idx = new brdb_value_t<vcl_string>("");
  pro.set_input(4, idx);
  return good;
}

bool boxm2_cpp_image_density_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_image_density_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr float_image=boxm2_util::prepare_input_image(img);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
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
    else if ( apps[i] == boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
    }
  }
  if (!foundDataType) {
    vcl_cout<<"BOXM2_CPP_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }

  if (identifier.size() > 0) {
    data_type += "_" + identifier;
  }
  if (vil_image_view<float> * input_image=dynamic_cast<vil_image_view<float> * > (float_image.ptr()))
  {
    vcl_vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
    if (vis_order.empty())
    {
      vcl_cout<<" None of the blocks are visible from this viewpoint"<<vcl_endl;
      return true;
    }

    // function call
    vil_image_view<float> * density_img=new vil_image_view<float>(input_image->ni(),input_image->nj());
    vil_image_view<float> * vis_img=new vil_image_view<float>(input_image->ni(),input_image->nj());
    density_img->fill(0.0f);
    vis_img->fill(1.0f);

     vcl_vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      vcl_cout<<"Block Id "<<(*id)<<vcl_endl;
      boxm2_block *     blk  =  cache->get_block(*id);
      boxm2_data_base *  alph = cache->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
      boxm2_data_base *  mog  = cache->get_data_base(*id,data_type);
      vcl_vector<boxm2_data_base*> datas;
      datas.push_back(alph);
      datas.push_back(mog);
      boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
      scene_info_wrapper->info=scene->get_blk_metadata(*id);

      if ( data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != vcl_string::npos )
      {
        boxm2_image_density_functor<BOXM2_MOG3_GREY> render_functor;
        render_functor.init_data(datas, input_image, density_img, vis_img);
        cast_ray_per_block<boxm2_image_density_functor<BOXM2_MOG3_GREY> >
          (render_functor,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
      }
      else if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != vcl_string::npos )
      {
        vcl_cout << "Rendering using gauss grey!\n";
        vcl_cout.flush();
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
  const unsigned n_inputs_ = 8;
  const unsigned n_outputs_ = 2;
  vcl_size_t lthreads[2]={8,8};
}

bool boxm2_cpp_image_density_masked_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_image_density_masked_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
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
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "float";  // the cumulative normalized density measure for the image

  bool good = pro.set_input_types(input_types_) &&
    pro.set_output_types(output_types_);
  // in case the 8th input is not set
  brdb_value_sptr idx = new brdb_value_t<vcl_string>("");
  pro.set_input(7, idx);
  return good;
}

bool boxm2_cpp_image_density_masked_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_image_density_masked_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr float_image=boxm2_util::prepare_input_image(img);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  float shadow_prior = pro.get_input<float>(i++);
  float shadow_sigma = pro.get_input<float>(i++);
  float shadow_thres = pro.get_input<float>(i++);
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
    else if ( apps[i] == boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
    }
  }
  if (!foundDataType) {
    vcl_cout<<"BOXM2_CPP_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }

  if (identifier.size() > 0) {
    data_type += "_" + identifier;
  }
  if (vil_image_view<float> * input_image=dynamic_cast<vil_image_view<float> * > (float_image.ptr()))
  {
    vcl_vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
    if (vis_order.empty())
    {
      vcl_cout<<" None of the blocks are visible from this viewpoint"<<vcl_endl;
      return true;
    }

    // function call
    vil_image_view<float> * density_img=new vil_image_view<float>(input_image->ni(),input_image->nj());
    vil_image_view<float> * vis_img=new vil_image_view<float>(input_image->ni(),input_image->nj());
    density_img->fill(0.0f);
    vis_img->fill(1.0f);

     vcl_vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      vcl_cout<<"Block Id "<<(*id)<<vcl_endl;
      boxm2_block *     blk  =  cache->get_block(*id);
      boxm2_data_base *  alph = cache->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
      boxm2_data_base *  mog  = cache->get_data_base(*id,data_type);
      vcl_vector<boxm2_data_base*> datas;
      datas.push_back(alph);
      datas.push_back(mog);
      boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
      scene_info_wrapper->info=scene->get_blk_metadata(*id);

      if ( data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != vcl_string::npos )
      {
        boxm2_image_density_functor<BOXM2_MOG3_GREY> render_functor;
        render_functor.init_data(datas, input_image, density_img, vis_img);
        cast_ray_per_block<boxm2_image_density_functor<BOXM2_MOG3_GREY> >
          (render_functor,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
      }
      else if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != vcl_string::npos )
      {
        vcl_cout << "Rendering using gauss grey!\n";
        vcl_cout.flush();
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

    vcl_cout << "saving shadow density mask image\n";
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

