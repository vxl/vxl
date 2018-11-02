#ifndef boxm_generate_edge_sample_functor_h
#define boxm_generate_edge_sample_functor_h
//:
// \file
#include <iostream>
#include <boxm/boxm_apm_traits.h>
#include <boxm/basic/boxm_raytrace_function.h>
#include <boxm/sample/boxm_edge_sample.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T, class T_aux>
class boxm_generate_edge_sample_functor
{
 public:
  boxm_generate_edge_sample_functor(vil_image_view<float> &observation,
                                    unsigned int /*ni*/=0,unsigned /*nj*/=0)
    : obs_(observation)
  {
    scene_read_only_=true;
    is_aux_=true;
  }

  inline bool step_cell(unsigned int i, unsigned int j,
                        vgl_point_3d<double> s0, vgl_point_3d<double> s1,
                        T &cell_value, // FIXME: unused...
                        T_aux &aux_val)
  {
    const float seg_len = (float)(s1 - s0).length();
    aux_val.obs_ += obs_(i,j) * seg_len;
    aux_val.seg_len_ += seg_len;
    return true;
  }

 public:
  bool scene_read_only_;
  bool is_aux_;

 private:
  vil_image_view<float> &obs_;
};

template <class T_loc, class T_data>
void boxm_generate_edge_sample_rt(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                  vpgl_camera_double_sptr cam,
                                  vil_image_view<float> &obs,
                                  std::string iname)
{
    typedef boxm_aux_edge_sample<float> aux_datatype;
    boxm_aux_scene<T_loc, T_data, aux_datatype> aux_scene(&scene,iname, boxm_aux_scene<T_loc,T_data,aux_datatype>::CLONE);
    typedef boxm_generate_edge_sample_functor<boxm_edge_sample<float>,aux_datatype> func;
    boxm_raytrace_function<func,T_loc, T_data, aux_datatype> raytracer(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
    std::cerr << "PROCESSING EDGE IMAGE\n";
    func functor(obs,obs.ni(),obs.nj());
    raytracer.run(functor);

    //aux_scene.clean_scene();
    std::cerr << "DONE.\n";
}

#endif // boxm_generate_edge_sample_functor_h
