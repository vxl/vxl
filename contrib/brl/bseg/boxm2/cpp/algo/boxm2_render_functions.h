#ifndef boxm2_render_functions_h_
#define boxm2_render_functions_h_

// Render block functions (make use of the render functor classes)
//
#include "boxm2_render_exp_image_functor.h"
#include "boxm2_render_exp_depth_functor.h"
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>

void boxm2_render_expected_image( boxm2_scene_info * linfo,
                                  boxm2_block * blk_sptr,
                                  std::vector<boxm2_data_base*> & datas,
                                  const vpgl_camera_double_sptr& cam ,
                                  vil_image_view<float> *expected,
                                  vil_image_view<float> * vis,
                                  unsigned int roi_ni,
                                  unsigned int roi_nj,
                                  unsigned int roi_ni0=0,
                                  unsigned int roi_nj0=0, const std::string& data_type = "boxm2_mog3_grey");

void boxm2_render_cone_exp_image(boxm2_scene_info * linfo,
                                boxm2_block * blk_sptr,
                                std::vector<boxm2_data_base*> & datas,
                                const vpgl_camera_double_sptr& cam ,
                                vil_image_view<float> *expected,
                                vil_image_view<float> * vis,
                                unsigned int roi_ni,
                                unsigned int roi_nj,
                                unsigned int roi_ni0=0,
                                unsigned int roi_nj0=0);

void boxm2_render_expected_depth( boxm2_scene_info * linfo,
                                  boxm2_block * blk_sptr,
                                  boxm2_data_base * data,
                                  const vpgl_camera_double_sptr& cam,
                                  vil_image_view<float> *expected,
                                  vil_image_view<float> * vis,
                                  vil_image_view<float> * len_img,
                                  unsigned int roi_ni,
                                  unsigned int roi_nj,
                                  unsigned int roi_ni0=0,
                                  unsigned int roi_nj0=0);

void boxm2_render_silhouette( boxm2_scene_info * linfo,
                              boxm2_block * blk_sptr,
                              boxm2_data_base * alpha,
                              const vpgl_camera_double_sptr& cam ,
                              vil_image_view<float> *silhouette,
                              vil_image_view<float> * vis,
                              unsigned int roi_ni,
                              unsigned int roi_nj,
                              unsigned int roi_ni0=0,
                              unsigned int roi_nj0=0);

void boxm2_render_depth_of_max_prob( boxm2_scene_info * linfo,
                                     boxm2_block * blk_sptr,
                                     boxm2_data_base * data,
                                     const vpgl_camera_double_sptr& cam,
                                     vil_image_view<float> *expected,
                                     vil_image_view<float> * vis,
                                     vil_image_view<float> * len_img,
                                     unsigned int roi_ni,
                                     unsigned int roi_nj,
                                     unsigned int roi_ni0=0,
                                     unsigned int roi_nj0=0);


#endif  //boxm2_render_functions_h_
