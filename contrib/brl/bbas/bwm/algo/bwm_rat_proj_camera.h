#ifndef bwm_rat_proj_camera_h_
#define bwm_rat_proj_camera_h_
//:
// \file

#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_compute.h>

class bwm_rat_proj_camera
{
 public:
  bwm_rat_proj_camera(){}

  bwm_rat_proj_camera(vpgl_rational_camera<double>& rat_cam);

  ~bwm_rat_proj_camera() {}

  void set_rat_cam(vpgl_rational_camera<double>& rat_cam) {rat_cam_ = rat_cam;}

  void compute();

  //: Project a point in world coordinates onto the image plane.
  //virtual vgl_homg_point_2d<double> project( const vgl_homg_point_3d<double>& world_point ) const;

  //: Non-homogeneous version of the above.
  //vgl_homg_point_2d<double> project( const vgl_point_3d<double>& world_point ) const {
  //  return project( vgl_homg_point_3d<double>( world_point ) ); }

  //: Find the 3d ray that goes through the camera center and the provided image point.
  //vgl_homg_line_3d_2_points<double> backproject( const vgl_homg_point_2d<double>& image_point ) const;
  vpgl_perspective_camera<double> get_persp_cam() const { return persp_cam_;}
  vgl_h_matrix_3d<double> norm_trans() const { return t_; }

  void shift_camera(vgl_point_3d<double> center);

 private:
  vpgl_rational_camera<double> rat_cam_;
  vpgl_perspective_camera<double> persp_cam_;
  vgl_h_matrix_3d<double> t_;
};

#endif
