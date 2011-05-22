#include "boxm2_render_functions.h"
#include "boxm2_render_cone_functor.h"
#include "boxm2_cast_cone_ray_function.h"
#include <vul/vul_timer.h>

void boxm2_render_expected_image( boxm2_scene_info * linfo,
                                  boxm2_block * blk_sptr,
                                  vcl_vector<boxm2_data_base*> & datas,
                                  vpgl_camera_double_sptr cam ,
                                  vil_image_view<float> *expected,
                                  vil_image_view<float> * vis,
                                  unsigned int roi_ni,
                                  unsigned int roi_nj,
                                  unsigned int roi_ni0,
                                  unsigned int roi_nj0)
{
  boxm2_render_exp_image_functor render_functor;
  render_functor.init_data(datas,expected,vis);
  cast_ray_per_block<boxm2_render_exp_image_functor>
      (render_functor,linfo,blk_sptr,cam,roi_ni,roi_nj,roi_ni0,roi_nj0);
}
void boxm2_render_expected_image( boxm2_scene_sptr scene,
                                  boxm2_cache_sptr cache,
                                  vpgl_camera_double_sptr cam,
                                  int illumination_bin_index,
                                  vil_image_view<float> *expected,
                                  vil_image_view<float> * vis,
                                  unsigned int roi_ni,
                                  unsigned int roi_nj,
                                  unsigned int roi_ni0,
                                  unsigned int roi_nj0)
{
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
    vcl_cout<<"In boxm2_render_expected_image: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return;
  }
  if(illumination_bin_index>=0){
    vcl_stringstream s;
    s << illumination_bin_index;
    vcl_string ident = "_ill_bin_" + s.str();
    data_type += ident;
  }
  vcl_vector<boxm2_block_id> vis_order=scene->get_vis_blocks((vpgl_generic_camera<double>*)(cam.ptr()));
  vcl_vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    boxm2_block *     blk  =  cache->get_block(*id);
    boxm2_data_base *  alph = cache->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data_base *  mog  = cache->get_data_base(*id,data_type);
    vcl_vector<boxm2_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);

    boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
    scene_info_wrapper->info=scene->get_blk_metadata(*id);

    boxm2_render_expected_image(scene_info_wrapper->info,
                                blk,datas,cam,expected,
                                vis,roi_ni,roi_nj, roi_ni0, roi_nj0);
  }
}
void boxm2_render_cone_exp_image(boxm2_scene_info * linfo,
                                 boxm2_block * blk_sptr,
                                 vcl_vector<boxm2_data_base*> & datas,
                                 vpgl_camera_double_sptr cam ,
                                 vil_image_view<float> *expected,
                                 vil_image_view<float> * vis,
                                 unsigned int roi_ni,
                                 unsigned int roi_nj,
                                 unsigned int roi_ni0,
                                 unsigned int roi_nj0)
{
  vul_timer t; 
  boxm2_render_cone_functor render_functor;
  render_functor.init_data(datas,expected,vis);
  cast_cone_ray_per_block<boxm2_render_cone_functor>
      (render_functor,linfo,blk_sptr,cam,roi_ni,roi_nj,roi_ni0,roi_nj0);
  vcl_cout<<"Cone Render Time: "<<t.all()<<"ms"<<vcl_endl;
}

void boxm2_render_expected_depth( boxm2_scene_info * linfo,
                                  boxm2_block * blk_sptr,
                                  boxm2_data_base * data,
                                  vpgl_camera_double_sptr cam ,
                                  vil_image_view<float> *expected,
                                  vil_image_view<float> * vis,
                                  vil_image_view<float> * len_img,
                                  unsigned int roi_ni,
                                  unsigned int roi_nj,
                                  unsigned int roi_ni0,
                                  unsigned int roi_nj0)
{
  boxm2_render_exp_depth_functor render_functor;
  render_functor.init_data(data,expected,vis,len_img);
  cast_ray_per_block<boxm2_render_exp_depth_functor>
      (render_functor,linfo,blk_sptr,cam,roi_ni,roi_nj,roi_ni0,roi_nj0);
}
