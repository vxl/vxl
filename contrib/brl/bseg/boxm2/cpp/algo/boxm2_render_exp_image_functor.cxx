#include "boxm2_render_exp_image_functor.h"

void boxm2_render_exp_image(boxm2_scene_info * linfo,
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
