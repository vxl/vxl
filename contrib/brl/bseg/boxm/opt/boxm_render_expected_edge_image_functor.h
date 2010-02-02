#ifndef boxm_render_expected_edge_image_functor_h
#define boxm_render_expected_edge_image_functor_h
//:
// \file
#include <boxm/boxm_apm_traits.h>
#include <boxm/opt/boxm_raytrace_function.h>
#include <boxm/opt/boxm_rt_sample.h>
#include <boxm/boxm_simple_grey_processor.h>
#include <boxm/boxm_mob_grey_processor.h>
#include <vnl/vnl_math.h>
#include <vil/vil_transform.h>
#include <sdet/sdet_img_edge.h>

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
   // if (expected_(i,j) < exp)
    expected_(i,j) =  vnl_math_max(expected_(i,j), exp);

    return true;
  }

  bool scene_read_only_;
  bool is_aux_;

 private:
  vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected_;
};


//: Functor class to normalize expected image
template<class T_data>
class normalize_expected_functor_edge
{
 public:
   normalize_expected_functor_edge(float n_normal, unsigned int dof) : n_normal_(n_normal), dof_(dof) {vcl_cout << "Degree of freedom " << dof << vcl_endl; }

   void operator()(float mask, float &pix) const
   {
    pix = sdet_img_edge::convert_edge_statistics_to_probability(pix,n_normal_,dof_);
   }
  
   float n_normal_;
   unsigned int dof_;
};


template <class T_loc, class T_data>
void boxm_render_edge_image_rt(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                          vpgl_camera_double_sptr cam,
                          vil_image_view<float> &expected,
                          vil_image_view<float> & mask,
                          float n_normal, 
                          unsigned int num_samples, //degree of freedom
                          int bin = -1)
                          
{
  typedef boxm_aux_traits<BOXM_AUX_NULL>::sample_datatype sample_datatype;
  boxm_aux_scene<T_loc, T_data,boxm_aux_edge_sample<sample_datatype> > aux_scene(&scene,boxm_aux_traits<BOXM_AUX_NULL>::storage_subdir(), boxm_aux_scene<T_loc, T_data,boxm_aux_edge_sample<sample_datatype> >::LOAD);
  typedef boxm_render_expected_edge_image_functor<BOXM_EDGE_FLOAT,boxm_aux_edge_sample<sample_datatype> > expfunctor;
  boxm_raytrace_function<expfunctor,T_loc, T_data, boxm_aux_edge_sample<sample_datatype> > raytracer(scene,aux_scene,cam.ptr(),expected.ni(),expected.nj());
  expfunctor exp_functor(expected,mask,expected.ni(),expected.nj(),true,false);
  raytracer.run(exp_functor);

  normalize_expected_functor_edge<T_data> norm_fn(n_normal,num_samples-1);
  vil_transform2<float,float, normalize_expected_functor_edge<T_data> >(mask,expected,norm_fn);
}

#endif
