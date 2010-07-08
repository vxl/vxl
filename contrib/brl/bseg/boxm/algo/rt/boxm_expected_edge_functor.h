#ifndef boxm_expected_edge_functor_h_
#define boxm_expected_edge_functor_h_

#include <boxm/boxm_scene.h>
#include <boxm/algo/rt/boxm_render_expected_edge_tangent_image_functor.h>

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_polygon.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>

#include <vcl_vector.h>

template <class T_loc, class T_data>
class boxm_expected_edge_functor {

public:
  boxm_expected_edge_functor(boxm_scene<boct_tree<T_loc, T_data > > &scene) 
    : scene_(scene) {}
  ~boxm_expected_edge_functor(){}

  bool apply(const vpgl_camera_double_sptr& cam, vil_image_view<float> *img_eei) { boxm_render_edge_tangent_image_rt(scene_,cam,*img_eei); return true; }

private:
  boxm_scene<boct_tree<T_loc, T_data > > scene_;

};

#undef BOXM_EXPECTED_EDGE_INSTANTIATE
#define BOXM_EXPECTED_EDGE_INSTANTIATE(T1,T2) \
template class boxm_expected_edge_functor<T1, T2 >

#endif