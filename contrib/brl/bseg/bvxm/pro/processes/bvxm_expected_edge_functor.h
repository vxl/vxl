#ifndef bvxm_expected_edge_functor_h_
#define bvxm_expected_edge_functor_h_

#include <iostream>
#include <vector>
#include <bvxm/grid/bvxm_voxel_slab.h>

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_polygon.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


class bvxm_expected_edge_functor
{
 public:
  bvxm_expected_edge_functor(const bvxm_voxel_slab<float>* slab, const vgl_box_3d<double>& box,
                             const vgl_vector_3d<float>& vox_dim, const vgl_vector_3d<float>& world_dim)
    : slab_(slab), box_(box), vox_dim_(vox_dim), world_dim_(world_dim) {}
  ~bvxm_expected_edge_functor()= default;
  bool apply(const vpgl_camera_double_sptr& cam, vil_image_view<float> *img_eei);

 private:

  std::vector<vgl_point_3d<double> > convert_3d_box_to_3d_points(const vgl_box_3d<double> box_3d);
  vgl_polygon<double> convert_3d_box_to_2d_polygon(const vgl_box_3d<double> box_3d, const vpgl_perspective_camera<double> *cam);

  const bvxm_voxel_slab<float>* slab_;
  vgl_box_3d<double> box_;
  vgl_vector_3d<float> vox_dim_;
  vgl_vector_3d<float> world_dim_;
};

#endif
