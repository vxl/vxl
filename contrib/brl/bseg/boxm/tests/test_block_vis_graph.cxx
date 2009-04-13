
#include <testlib/testlib_test.h>

#include <boxm/boxm_scene.h>
#include <boxm/boxm_block_vis_graph_iterator.h>
#include <boct/boct_tree.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpl/vpl.h>

typedef boct_tree<short,vgl_point_3d<double>,void> tree_type;

const int IMAGE_U = 200;
const int IMAGE_V = 200;
const int boxm_x_scale = 900;
const int boxm_y_scale = 900;
const double boxm_focal_length = 1.0;
const double boxm_camera_dist= 200;

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
  persp_cam.look_at(vgl_homg_point_3d<double>(centroid));
  vpgl_rational_camera<double>* rat_cam = new vpgl_rational_camera<double>(perspective_to_rational(persp_cam));
  return rat_cam;
}

MAIN( test_block_vis_graph )
{
  START ("CREATE SCENE");
  short nlevels=5;

  // create scene
  bgeo_lvcs lvcs(33.33,44.44,10.0, bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
  vgl_point_3d<double> origin(10,10,20);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<double> world_dim(30,30,30);
  boxm_scene<tree_type> scene(lvcs, origin, block_dim, world_dim);
  scene.set_paths("./boxm_scene", "block");

  vgl_box_3d<double> world;
  world.add(origin);
  world.add(vgl_point_3d<double>(origin.x()+world_dim.x(), origin.y()+world_dim.y(), origin.z()+world_dim.z()));
  vpgl_camera_double_sptr camera = generate_camera_top(world);
  boxm_block_vis_graph_iterator<tree_type> block_vis_iter(camera, &scene, IMAGE_U, IMAGE_V);

  vcl_vector<boxm_block<tree_type>*> blocks = block_vis_iter.frontier();
  //TEST("Number of blocks iterator visits", num_blocks, x*y*z);
  SUMMARY();
}
