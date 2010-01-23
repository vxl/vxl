#ifndef boxm_render_expected_edge_image_functor_h
#define boxm_render_expected_edge_image_functor_h
//:
// \file
#include <boxm/boxm_apm_traits.h>
#include <boxm/opt/boxm_raytrace_function.h>
#include <boxm/opt/boxm_rt_sample.h>
#include <boxm/boxm_simple_grey_processor.h>
#include <boxm/boxm_mob_grey_processor.h>
#include <vil/vil_transform.h>

template <boxm_apm_type APM, class T_aux>
class boxm_render_expected_edge_image_functor
{
 public:
  //: "default" constructor
  boxm_render_expected_edge_image_functor(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected,
                                          vil_image_view<float> &mask_vis,
                                          unsigned int ni,unsigned nj,
                                          bool scene_read_only=false,
                                          bool is_aux=true)
    : expected_(expected)
  {
    typename boxm_apm_traits<APM>::obs_datatype nil(0);
    expected_.fill(nil);
    scene_read_only_=scene_read_only;
    is_aux_=is_aux;
  }

  inline bool step_cell(unsigned int i, unsigned int j,
                        vgl_point_3d<double> s0,
                        vgl_point_3d<double> s1,
                        boxm_edge_sample<typename boxm_apm_traits<APM>::obs_datatype> &cell_value,
                        T_aux aux_val)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();

    // TODO: revise with segment length
    const float exp  = cell_value.edge_prob_;

    // keep the max value in expected image
    if (expected_(i,j) < exp)
      expected_(i,j) =  exp;

    return true;
  }

  bool scene_read_only_;
  bool is_aux_;

 private:
  vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected_;
};


//: Functor class to normalize expected image
template<class T_data>
class normalize_expected_edge_functor_rt
{
 public:
  normalize_expected_edge_functor_rt(bool use_black_background) : use_black_background_(use_black_background) {}

  void operator()(float mask, typename T_data::obs_datatype &pix) const
  {
    if (!use_black_background_) {
      pix += mask*0.5f;
    }
  }
  bool use_black_background_;
};


template <class T_loc, class T_data>
void boxm_render_edge_image_rt(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                               vpgl_camera_double_sptr cam,
                               vil_image_view<float> &expected,
                               vil_image_view<float> & mask,
                               int bin = -1)
                             //bool use_black_background = false)
{
  typedef float sample_datatype;
  boxm_aux_scene<T_loc, T_data,boxm_edge_sample<sample_datatype> > aux_scene(&scene,boxm_aux_traits<BOXM_AUX_NULL>::storage_subdir(), boxm_aux_scene<T_loc, T_data,boxm_edge_sample<sample_datatype> >::LOAD);
  typedef boxm_render_expected_edge_image_functor<BOXM_EDGE_FLOAT,boxm_edge_sample<sample_datatype> > expfunctor;
  boxm_raytrace_function<expfunctor,T_loc, T_data,boxm_edge_sample<sample_datatype> > raytracer(scene,aux_scene,cam.ptr(),expected.ni(),expected.nj());
  expfunctor exp_functor(expected,mask,expected.ni(),expected.nj(),true,false);
  raytracer.run(exp_functor);

  //normalize_expected_edge_functor_rt<T_data> norm_fn(use_black_background);
  //vil_transform2<float,typename T_data::obs_datatype, normalize_expected_edge_functor_rt<T_data> >(mask,expected,norm_fn);
}

#endif
