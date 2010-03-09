#ifndef boxm_render_expected_edge_tangent_image_functor_h
#define boxm_render_expected_edge_tangent_image_functor_h
//:
// \file
#include <boxm/boxm_apm_traits.h>
#include <boxm/opt/boxm_raytrace_function.h>
#include <boxm/opt/boxm_rt_sample.h>
#include <boxm/boxm_simple_grey_processor.h>
#include <boxm/boxm_mob_grey_processor.h>
#include <boxm/boxm_edge_tangent_sample.h>
#include <boxm/boxm_inf_line_sample.h>
#include <vnl/vnl_math.h>
#include <vil/vil_transform.h>
#include <vbl/vbl_array_2d.h>
#include <sdet/sdet_img_edge.h>
#ifdef DEBUG
#include <vcl_iostream.h>
#endif

template <boxm_apm_type APM, class T_aux>
class boxm_render_expected_edge_tangent_image_functor
{
 public:
  //: "default" constructor
  boxm_render_expected_edge_tangent_image_functor(vil_image_view<typename boxm_apm_traits<APM>::apm_datatype> &expected,
                                                  unsigned int /*ni*/=0, unsigned /*nj*/=0,
                                                  bool scene_read_only=false,
                                                  bool is_aux=true)
                                                  : expected_(expected), num_obs_(expected.ni(),expected.nj())
  {
    typename boxm_apm_traits<APM>::apm_datatype nil;
    expected_.fill(nil);
    num_obs_.fill(0);
    scene_read_only_=scene_read_only;
    is_aux_=is_aux;
  }

  inline bool step_cell(unsigned int i, unsigned int j,
                        vgl_point_3d<double> s0,
                        vgl_point_3d<double> s1,
                        boxm_inf_line_sample<typename boxm_apm_traits<APM>::obs_datatype> &cell_value,
                        T_aux aux_val)
  {
    typedef typename boxm_apm_traits<APM>::obs_datatype t;

    if(cell_value.num_obs_>num_obs_(i,j))
    {
        num_obs_(i,j)=cell_value.num_obs_;
        expected_(i,j)=cell_value;
    }
    return true;
  }

  bool scene_read_only_;
  bool is_aux_;

 private:
  vil_image_view<typename boxm_apm_traits<APM>::apm_datatype> &expected_;
  vil_image_view<unsigned> num_obs_;

};

template <class T_loc, class T_data>
void boxm_render_edge_tangent_image_rt(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                       vpgl_camera_double_sptr cam,
                                      vil_image_view<typename boxm_apm_traits<BOXM_EDGE_LINE>::apm_datatype> &expected,
                                       float n_normal,
                                       unsigned int num_samples) //degree of freedom
{
  
  typedef boxm_aux_traits<BOXM_AUX_NULL>::sample_datatype sample_datatype;
  boxm_aux_scene<T_loc, T_data,boxm_edge_tangent_sample<sample_datatype> > aux_scene(&scene,boxm_aux_traits<BOXM_AUX_NULL>::storage_subdir(), boxm_aux_scene<T_loc, T_data,boxm_edge_tangent_sample<sample_datatype> >::LOAD);
  typedef boxm_render_expected_edge_tangent_image_functor<BOXM_EDGE_LINE,boxm_edge_tangent_sample<sample_datatype> > expfunctor;
  boxm_raytrace_function<expfunctor,T_loc, T_data, boxm_edge_tangent_sample<sample_datatype> > raytracer
    (scene,aux_scene, cam.ptr(),expected.ni(),expected.nj());
  expfunctor exp_functor(expected,expected.ni(),expected.nj(),true,false);
  raytracer.run(exp_functor);

}

#endif
