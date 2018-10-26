#ifndef boxm_expected_edge_functor_h_
#define boxm_expected_edge_functor_h_
//:
// \file
#include <boxm/boxm_scene.h>
#include <boxm/algo/rt/boxm_render_expected_edge_tangent_image_functor.h>

#include <vpgl/vpgl_camera_double_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_transform.h>

#include <brip/brip_vil_float_ops.h>

//: Functor class to compute (1-x)
// assumes that max range value is "1"
// only sensible for real types
class boxm_exp_edge_vil_not_functor
{
 public:
  float operator()(float x)       const { return x<1.0f?(1.0f-x):0.0f; }
  double operator()(double x)     const { return x<1.0?1.0-x:0.0; }
};

template <class T_loc, class T_data>
class boxm_expected_edge_functor
{
 public:
  boxm_expected_edge_functor(boxm_scene<boct_tree<T_loc, T_data > > &scene)
    : scene_(scene) {}
  ~boxm_expected_edge_functor() = default;

  bool apply(const vpgl_camera_double_sptr& cam, vil_image_view<float> *img_eei)
  {
    boxm_render_edge_tangent_image_rt(scene_,cam,*img_eei);
    // now take the inverse of this image, pixels which contain edges will have values closer to 1 and others will be zero
    boxm_exp_edge_vil_not_functor nt;
    vil_transform(*img_eei, nt);

    // now blur this image
    *img_eei = brip_vil_float_ops::gaussian(*img_eei, 1.0f);

    return true;
  }

 private:
  boxm_scene<boct_tree<T_loc, T_data > > scene_;
};

#undef BOXM_EXPECTED_EDGE_INSTANTIATE
#define BOXM_EXPECTED_EDGE_INSTANTIATE(T1,T2) \
template class boxm_expected_edge_functor<T1, T2 >

#endif
