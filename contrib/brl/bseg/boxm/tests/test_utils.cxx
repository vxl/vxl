#include "test_utils.h"
#include <vgl/vgl_line_3d_2_points.h>
#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vgl/algo/vgl_rotation_3d.h>
void generate_persp_camera(double focal_length,
                           vgl_point_2d<double>& pp,  //principal point
                           double x_scale, double y_scale,
                           vgl_point_3d<double>& camera_center,
                           vpgl_perspective_camera<double>& cam)
{
  vpgl_calibration_matrix<double> K(focal_length,pp, x_scale, y_scale);
  cam.set_calibration(K);
  cam.set_camera_center(camera_center);
}

vpgl_rational_camera<double>
perspective_to_rational(vpgl_perspective_camera<double>& cam_pers)
{
  vnl_matrix_fixed<double,3,4> cam_pers_matrix = cam_pers.get_matrix();
  vcl_vector<double> neu_u,den_u,neu_v,den_v;
  double x_scale = 1.0,
         x_off = 0.0,
         y_scale = 1.0,
         y_off = 0.0,
         z_scale = 1.0,
         z_off = 0.0,
         u_scale = 1.0,
         u_off = 0.0,
         v_scale = 1.0,
         v_off = 0.0;

  for (int i=0; i<20; i++) {
    neu_u.push_back(0.0);
    neu_v.push_back(0.0);
    den_u.push_back(0.0);
    den_v.push_back(0.0);
  }

  int vector_map[] = {9,15,18,19};

  for (int i=0; i<4; i++) {
    neu_u[vector_map[i]] = cam_pers_matrix(0,i);
    neu_v[vector_map[i]] = cam_pers_matrix(1,i);
    den_u[vector_map[i]] = cam_pers_matrix(2,i);
    den_v[vector_map[i]] = cam_pers_matrix(2,i);
  }

  vpgl_rational_camera<double> cam_rat(neu_u,den_u,neu_v,den_v,
                                       x_scale,x_off,y_scale,y_off,z_scale,z_off,
                                       u_scale,u_off,v_scale,v_off);
  return cam_rat;
}

vpgl_camera_double_sptr generate_camera_top(vgl_box_3d<double>& world)
{
  vgl_point_2d<double> principal_point(IMAGE_U*0.5, IMAGE_V*0.5);

  vgl_point_3d<double> centroid = world.centroid();
  vgl_point_3d<double> camera_center(centroid.x(), centroid.y(), centroid.z()+boxm_camera_dist);

  vcl_vector<vpgl_camera_double_sptr> rat_cameras;

  vpgl_perspective_camera<double> persp_cam;
  generate_persp_camera(boxm_focal_length,principal_point, boxm_x_scale, boxm_y_scale, camera_center, persp_cam);
        double r[] = { 1, 0, 0,
              0, -1, 0,
              0, 0, -1 };

  vnl_matrix_fixed<double,3,3> R(r);
  persp_cam.set_rotation(vgl_rotation_3d<double>(R));

  //persp_cam.look_at(vgl_homg_point_3d<double>(centroid));
  vpgl_rational_camera<double>* rat_cam = new vpgl_rational_camera<double>(perspective_to_rational(persp_cam));
  return rat_cam;
}
vpgl_camera_double_sptr generate_camera_top_persp(vgl_box_3d<double>& world)
{
  vgl_point_2d<double> principal_point(IMAGE_U*0.5, IMAGE_V*0.5);

  vgl_point_3d<double> centroid = world.centroid();
  vgl_point_3d<double> camera_center(centroid.x(), centroid.y(), centroid.z()+boxm_camera_dist);

  vcl_vector<vpgl_camera_double_sptr> rat_cameras;

  vpgl_perspective_camera<double> * persp_cam= new vpgl_perspective_camera<double>();
  generate_persp_camera(boxm_focal_length,principal_point, boxm_x_scale, boxm_y_scale, camera_center, *persp_cam);
  persp_cam->look_at(vgl_homg_point_3d<double>(centroid));
  return persp_cam;
}

vcl_vector<vpgl_camera_double_sptr> generate_cameras_diagonal(vgl_box_3d<double>& world)
{
  vgl_point_2d<double> principal_point(IMAGE_U/2., IMAGE_V/2.);

  vgl_line_3d_2_points<double> diag(world.min_point(), world.max_point());

  vcl_vector<vgl_point_3d<double> > centers;
  unsigned i=0;
  for (double t=0; t<=1.0; t+=0.1) {
    vgl_point_3d<double> centroid = diag.point_t(t);
    centers.push_back(centroid);
    vcl_cout << centers[i++] << vcl_endl;
  }

//  vgl_box_2d<double> bb;
  vcl_vector<vpgl_camera_double_sptr> cameras;
  for (unsigned i=0; i<centers.size(); i++)
  {
    vgl_point_3d<double> camera_center  = centers[i];
    vpgl_perspective_camera<double> persp_cam;
    generate_persp_camera(boxm_focal_length,principal_point, boxm_x_scale, boxm_y_scale, camera_center, persp_cam);
    persp_cam.look_at(vgl_homg_point_3d<double>(camera_center.x()+10, camera_center.y()+10, camera_center.z()));
    cameras.push_back(new vpgl_perspective_camera<double>(persp_cam));
  }
  return cameras;
}
