#ifndef boxm_sun_vis_prob_functor_h
#define boxm_sun_vis_prob_functor_h
//:
//\file
#include <iostream>
#include <boxm/boxm_apm_traits.h>
#include <boxm/basic/boxm_raytrace_function.h>
#include <boxm/sample/boxm_opt2_sample.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <boxm/basic/boxm_seg_length_functor.h>
#include "boxm_pre_infinity_functor.h"
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <bpgl/algo/bpgl_camera_from_box.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


template <boxm_apm_type APM, class T_aux>
class boxm_sun_vis_prob_functor
{
 public:
  //: "default" constructor
  boxm_sun_vis_prob_functor(unsigned sun_ni, unsigned sun_nj)
  : sun_ni_(sun_ni), sun_nj_(sun_nj)
  {
    alpha_integral_ = vil_image_view<float>(sun_ni_, sun_nj_);
    alpha_integral_.fill(0.0f);

    //only reads info from the scene
    scene_read_only_=true;
    //needs to write aux
    is_aux_=true;
  }

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1, boxm_sample<APM> &cell_value, T_aux & aux_val)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();

    // accumulate segment length
    aux_val.seg_len_ += seg_len;

    // current sun visibility
    const float sun_vis = std::exp(-alpha_integral_(i,j));

    // update sun vis
    aux_val.vis_ +=  sun_vis * seg_len;

    // update alpha integral along sun ray i, j
    alpha_integral_(i,j) += cell_value.alpha * seg_len;

    return true;
  }

 public:
  bool scene_read_only_;
  bool is_aux_;

 private:
  unsigned sun_ni_;
  unsigned sun_nj_;
  vil_image_view<float> alpha_integral_;
};

template <class T_loc, class T_data, boxm_aux_type AUX_T>
void boxm_sun_vis_prob(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                       vgl_vector_3d<double> sun_angle,
                       std::string sun_occ_name)
{
  // scene bounding box
  vgl_box_3d<double> world_bb = scene.get_world_bbox();
  unsigned max_level = scene.max_level();
  unsigned sun_ni = 1<<max_level;//twice as many
  unsigned sun_nj = sun_ni;
  vpgl_affine_camera<double> sun_cam =
  bpgl_camera_from_box::affine_camera_from_box(world_bb, -sun_angle, sun_ni, sun_nj);

  vpgl_camera_double_sptr cam = new vpgl_affine_camera<double> (sun_cam);

  typedef typename boxm_aux_traits<AUX_T>::sample_datatype sample_datatype;
  boxm_aux_scene<T_loc, T_data,  sample_datatype> aux_scene(&scene,sun_occ_name, boxm_aux_scene<T_loc, T_data,  sample_datatype>::CLONE);

  std::cout<<"Sun visibility"<<std::endl;
  // functor to accumulate sun visibility
  typedef boxm_sun_vis_prob_functor<T_data::apm_type, sample_datatype> sun_vis;

  // set up the raytrace function
  boxm_raytrace_function<sun_vis,T_loc, T_data, sample_datatype> raytracer_vis(scene,aux_scene,cam.ptr(),sun_ni,sun_nj);

  // construct the sun visibility functor need only sun ray space dimensions
  sun_vis sun_vis_functor(sun_ni, sun_nj);

  // run the functor over the octree
  raytracer_vis.run(sun_vis_functor);
  std::cout<<"Sun Occlusion Done."<<std::endl;
}

#endif // boxm_sun_vis_prob_functor_h
