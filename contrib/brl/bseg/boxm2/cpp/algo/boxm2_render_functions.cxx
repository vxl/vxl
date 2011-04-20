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
