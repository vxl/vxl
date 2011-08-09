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
                                  unsigned int roi_nj0, vcl_string data_type)
{
  if ( data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != vcl_string::npos )
  {
    boxm2_render_exp_image_functor<BOXM2_MOG3_GREY> render_functor;
    render_functor.init_data(datas,expected,vis);
    cast_ray_per_block<boxm2_render_exp_image_functor<BOXM2_MOG3_GREY> >
      (render_functor,linfo,blk_sptr,cam,roi_ni,roi_nj,roi_ni0,roi_nj0);
  } 
  else if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != vcl_string::npos )
  {
    boxm2_render_exp_image_functor<BOXM2_GAUSS_GREY> render_functor;
    render_functor.init_data(datas,expected,vis);
    cast_ray_per_block<boxm2_render_exp_image_functor<BOXM2_GAUSS_GREY> >
      (render_functor,linfo,blk_sptr,cam,roi_ni,roi_nj,roi_ni0,roi_nj0);
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
