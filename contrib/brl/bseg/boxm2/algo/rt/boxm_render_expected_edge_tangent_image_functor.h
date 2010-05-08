#ifndef boxm2_render_expected_edge_tangent_image_functor_h
#define boxm2_render_expected_edge_tangent_image_functor_h
//:
// \file
#include <boxm2/boxm_apm_traits.h>
#include <boxm2/basic/boxm_raytrace_function.h>
#include <boxm2/sample/boxm_rt_sample.h>
#include <boxm2/sample/algo/boxm_simple_grey_processor.h>
#include <boxm2/sample/algo/boxm_mob_grey_processor.h>
#include <boxm2/sample/boxm_edge_tangent_sample.h>
#include <boxm2/sample/boxm_inf_line_sample.h>
#include <vnl/vnl_math.h>
#include <vil/vil_transform.h>
#include <vbl/vbl_array_2d.h>
#include <sdet/sdet_img_edge.h>
#ifdef DEBUG
#include <vcl_iostream.h>
#endif

inline static double angle_0_360(double ang)
{
  while (ang<0) ang += 2*vnl_math::pi;
  while (ang > 2.0*vnl_math::pi) ang -= 2*vnl_math::pi;
  return ang;
}

template <boxm_apm_type APM, class T_aux>
class boxm_render_expected_edge_tangent_image_functor
{
 public:
  //: "default" constructor
  boxm_render_expected_edge_tangent_image_functor(vil_image_view<float > &expected,
                                                  vpgl_camera_double_sptr cam,
                                                  float threshold,
                                                  unsigned int /*ni*/=0, unsigned /*nj*/=0,
                                                  bool scene_read_only=false,
                                                  bool is_aux=true)
  : expected_(expected), num_obs_(expected.ni(),expected.nj()), cam_(cam), residual(threshold)
  {
    expected_.fill(-1);
    num_obs_.fill(0);
    scene_read_only_=scene_read_only;
    is_aux_=is_aux;
  }

  inline bool step_cell(unsigned int i, unsigned int j,
                        vgl_point_3d<double> s0, // FIXME - unused
                        vgl_point_3d<double> s1, // FIXME - unused
                        boxm_inf_line_sample<typename boxm_apm_traits<APM>::obs_datatype> &cell_value,
                        T_aux /*aux_val*/)
  {
    //vcl_cout<< cell_value.residual_<<' ';
    if (cell_value.num_obs_>2 && cell_value.residual_<residual && cell_value.num_obs_>num_obs_(i,j))
    {
      num_obs_(i,j)=cell_value.num_obs_;
      double u1,v1,u2,v2;
      vgl_point_3d<float>  p1=cell_value.line_clipped_.point1();
      vgl_point_3d<float>  p2=cell_value.line_clipped_.point2();
      cam_->project(p1.x(),p1.y(),p1.z(),u1,v1);
      cam_->project(p2.x(),p2.y(),p2.z(),u2,v2);
      if ((u1-u2)==0 && v1-v2==0)
      {
        expected_(i,j,0)=-1;
        expected_(i,j,1)=-1;
        expected_(i,j,2)=-1;
      }
      else
      {
        expected_(i,j,0)=float(u1+u2)/2;
        expected_(i,j,1)=float(v1+v2)/2;
        expected_(i,j,2)=float(angle_0_360(vcl_atan2(v2-v1,u2-u1)));
      }
    }
    return true;
  }

  bool scene_read_only_;
  bool is_aux_;

 private:
  vil_image_view<float > &expected_;
  vil_image_view<unsigned> num_obs_;
  vpgl_camera_double_sptr cam_;
  float residual;
};

template <class T_loc, class T_data>
void boxm_render_edge_tangent_image_rt(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                       vpgl_camera_double_sptr cam,
                                       vil_image_view<float> &expected,
                                       float n_normal, // FIXME - unused
                                       unsigned int num_samples, // FIXME - unused
                                       float threshold) // degrees of freedom
{
  typedef boxm_aux_traits<BOXM_AUX_NULL>::sample_datatype sample_datatype;
  boxm_aux_scene<T_loc, T_data,boxm_edge_tangent_sample<sample_datatype> > aux_scene(&scene,boxm_aux_traits<BOXM_AUX_NULL>::storage_subdir(), boxm_aux_scene<T_loc, T_data,boxm_edge_tangent_sample<sample_datatype> >::LOAD);
  typedef boxm_render_expected_edge_tangent_image_functor<BOXM_EDGE_LINE,sample_datatype > expfunctor;
  boxm_raytrace_function<expfunctor,T_loc, T_data> raytracer(scene, cam.ptr(),expected.ni(),expected.nj());
  expfunctor exp_functor(expected,cam,threshold,expected.ni(),expected.nj(),true,false);
  raytracer.run(exp_functor);
}

#endif
