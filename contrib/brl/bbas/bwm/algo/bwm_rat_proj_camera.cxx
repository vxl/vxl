#include "bwm_rat_proj_camera.h"
//:
// \file

//#include <vgl/vgl_homg_line_3d_2_points.h>
//#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>

bwm_rat_proj_camera::bwm_rat_proj_camera(vpgl_rational_camera<double>& rat_cam)
: rat_cam_(rat_cam)
{}

void bwm_rat_proj_camera::compute()
{
  vpgl_scale_offset<double> sox =
    rat_cam_.scl_off(vpgl_rational_camera<double>::X_INDX);
  vpgl_scale_offset<double> soy =
    rat_cam_.scl_off(vpgl_rational_camera<double>::Y_INDX);
  vpgl_scale_offset<double> soz =
    rat_cam_.scl_off(vpgl_rational_camera<double>::Z_INDX);

  vgl_point_3d<double> pmin(sox.offset()-sox.scale(),
                            soy.offset()-soy.scale(), 0);
  vgl_point_3d<double> pmax(sox.offset()+sox.scale(),
                            soy.offset()+soy.scale(),
                            soz.scale()/2.);

  vgl_box_3d<double> vol(pmin, pmax);
  vpgl_perspective_camera_compute::compute(rat_cam_, vol, persp_cam_, t_);
}

#if 0
//: Project a point in world coordinates onto the image plane.
vgl_homg_point_2d<double> vpgl_rat_proj_camera::project( const vgl_homg_point_3d<double>& world_point ) const
{
  //vpgl_proj_camera_compute pcc(rat_cam_);
  vgl_h_matrix_3d<double> T = pcc_.norm_trans();

  vgl_homg_point_2d<double> p = proj_cam_->project(T(world_point));
  return p;
}
#endif // 0

void bwm_rat_proj_camera::shift_camera(vgl_point_3d<double> center)
{
 // pcc_.compute(center, proj_cam_);
}
