#ifndef boxm_change_3d_functor_h
#define boxm_change_3d_functor_h
//:
// \file
#include <iostream>
#include <boxm/boxm_apm_traits.h>
#include <boxm/basic/boxm_raytrace_function.h>
#include <boxm/sample/boxm_scalar_sample.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


template <boxm_apm_type APM>
class boxm_change_3d_functor
{
 public:
  //: "default" constructor
  boxm_change_3d_functor(vil_image_view<float> const& change_image, float prob_ratio)
    : change_image_(change_image),
      vis_img_(change_image.ni(),change_image.nj(), 1),
      alpha_integral_(change_image.ni(),change_image.nj(), 1),
      prob_ratio_(prob_ratio)
  {
    vis_img_.fill(1.0f);
    alpha_integral_.fill(0.0f);

    //only reads info from the scene
    scene_read_only_=true;
    //needs to write aux
    is_aux_=true;
  }

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1, boxm_sample<APM>& cell_value, boxm_scalar_sample<float>& scalar)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();

    // update segment length sum
    scalar.seg_len_ += seg_len;

    // update alpha integral
    alpha_integral_(i,j) += cell_value.alpha * seg_len;

    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = std::exp(-alpha_integral_(i,j));

    // compute weight for this cell
    const float Omega = vis_img_(i,j) - vis_prob_end;

    float back_prob = change_image_(i,j);
    float fore_prob = 1.0f/(1.0f + prob_ratio_*back_prob);

    // set change prob in auxiliary voxel
    scalar.scalar_sum_ += Omega * fore_prob * seg_len;

    // update visibility probability
    vis_img_(i,j) = vis_prob_end;

    return true;
  }

 public:
  bool scene_read_only_;
  bool is_aux_;

 private:
  vil_image_view<float> change_image_;
  vil_image_view<float> alpha_integral_;
  vil_image_view<float> vis_img_;
  float prob_ratio_;
};

template <class T_loc, class T_data>
void boxm_change_3d(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                    vpgl_camera_double_sptr cam,
                    vil_image_view<float> &change_image,
                    std::string chng_name,
                    float prob_ratio)
{
  typedef boxm_scalar_sample<float> sample_datatype;
  boxm_aux_scene<T_loc, T_data,  sample_datatype> aux_scene(&scene,chng_name, boxm_aux_scene<T_loc, T_data,  sample_datatype>::CLONE);

  std::cout<<"Chng visibility"<<std::endl;
  // functor to compute 3-d change prob
  typedef boxm_change_3d_functor<T_data::apm_type> change_functor;

  // set up the raytrace function
  boxm_raytrace_function<change_functor,T_loc, T_data, sample_datatype> raytracer_chng(scene,aux_scene,cam.ptr(),change_image.ni(),change_image.nj());

  // construct the change probability functor
  change_functor chng_func(change_image, prob_ratio);

  // run the functor over the octree
  raytracer_chng.run(chng_func);

  std::cout<<"Change 3-d Done."<<std::endl;
}

#endif // boxm_change_3d_functor_h
