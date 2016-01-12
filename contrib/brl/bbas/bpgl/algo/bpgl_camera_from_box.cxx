#include "bpgl_camera_from_box.h"
#include "bpgl_project.h"
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgl/algo/vgl_rotation_3d.h>

vpgl_perspective_camera<double> bpgl_camera_from_box::
persp_camera_from_box(vgl_box_3d<double> const & box,
                      vgl_point_3d<double> const& camera_center,
                      unsigned ni,
                      unsigned nj)
{
  double cu = ni/2.0, cv = nj/2.0;
  vpgl_calibration_matrix<double> K(1.0, vgl_point_2d<double>(cu, cv));
  vgl_rotation_3d<double> R;
  vpgl_perspective_camera<double> C(K, camera_center, R);
  vgl_point_3d<double> cnt = box.centroid();
  vgl_homg_point_3d<double> cnth(cnt.x(), cnt.y(), cnt.z());
  C.look_at(cnth);
  vgl_box_2d<double> b2d = bpgl_project::project_bounding_box(C, box);
  vgl_point_2d<double> cnt2 = b2d.centroid();
  double w = b2d.width(), h = b2d.height();
  if(w==0||h==0) return C;
  double su = ni/w, sv = nj/h;
  //correction for perspective effect - projection of 3-d centroid
  //is not the same as the centroid of the projection.
  double ccu = cnt2.x()*(1-su)+su*cu;
  double ccv = cnt2.y()*(1-sv)+sv*cv;
  K = vpgl_calibration_matrix<double>(1.0, vgl_point_2d<double>(ccu, ccv), su, sv);
  C.set_calibration(K);
  return C;
}

vpgl_affine_camera<double> bpgl_camera_from_box::
affine_camera_from_box(vgl_box_3d<double> const & box,
                       vgl_vector_3d<double> const& ray,
                       unsigned ni,
                       unsigned nj)
{
  double du = 1.0, dv = 1.0;
  vgl_point_3d<double> cnt = box.centroid();
  vgl_vector_3d<double> up(0,0,1);
  vpgl_affine_camera<double> C(ray, up, cnt, ni/2.0, nj/2.0, du, dv);
  vgl_box_2d<double> b2d = bpgl_project::project_bounding_box(C, box);
  double w = b2d.width(), h = b2d.height();
  if(w==0||h==0) return C;
  double su = ni/w, sv = nj/h;
  C = vpgl_affine_camera<double>(ray, up, cnt, ni/2.0, nj/2.0, su, sv);
  vgl_point_3d<double> min_pt = box.min_point();
  vgl_point_3d<double> max_pt = box.max_point();
  double len = (max_pt-min_pt).length();
  C.set_viewing_distance(10.0*len);
  return C;
}
