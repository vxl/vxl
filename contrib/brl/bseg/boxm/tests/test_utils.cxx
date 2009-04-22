#include "test_utils.h"



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


//bool generate_test_boxes(double box_min_x, double box_min_y, double box_min_z,
//                                    double box_dim_x, double box_dim_y, double box_dim_z,
//                                    double world_dim_x, double world_dim_y, double world_dim_z,
//                                    vcl_vector<vgl_box_3d<double> >& boxes, bool gen_2box = true)
//{
//  // create the big box at the bottom
//  double max_x = box_min_x + box_dim_x;
//  double max_y = box_min_y + box_dim_y;
//  double max_z = box_min_z + box_dim_z;
//  if ((max_x > world_dim_x) || (max_y > world_dim_y) || (max_z > world_dim_z)) {
//    vcl_cerr << "generate_boxes() -- the box is out of world boundaries!\n";
//    return false;
//  }
//
//  vgl_box_3d<double> box(box_min_x, box_min_y, box_min_z, max_x, max_y, max_z);
//  boxes.push_back(box);
//
//  if (gen_2box) {
//    // create the top boxe
//    vgl_point_3d<double> centroid = box.centroid();
//    // make the top box 2/3 of the size of the previous one
//    double dimx = (box.max_x() - box.min_x())/2;
//    double dimy = (box.max_y() - box.min_y())/2;
//    double dimz = (box.max_z() - box.min_z())/2;
//    centroid.set(centroid.x(), centroid.y(), box.max_z() + dimz/2.0);
//    vgl_box_3d<double> top_box = vgl_box_3d<double> (centroid, dimx, dimy, dimz, vgl_box_3d<double>::centre);
//    // translate it a bit
//    vgl_point_3d<double> top_centroid = top_box.centroid();
//    top_box.set_centroid(vgl_point_3d<double>(top_centroid.x()+dimx/3., top_centroid.y()+dimx/3., top_centroid.z()));
//    // check if the box in the world completely
//    max_x = top_box.max_x();
//    max_y = top_box.max_y();
//    max_z = top_box.max_z();
//    // stop if the new box is getting out of the boundaries
//    if ((max_x > world_dim_x) || (max_y > world_dim_y) || (max_z > world_dim_z))
//      return false;
//    boxes.push_back(top_box);
//  }
//  else {
//    vgl_box_3d<double> top_box = vgl_box_3d<double>();
//    boxes.push_back(top_box);
//  }
//
//  return true;
//}
//
//
//
//void gen_texture_map(vgl_box_3d<double> box,
//					   vcl_vector<vcl_vector<float> >& intens_map_bt,
//					   vcl_vector<vcl_vector<float> >& intens_map_side1,
//					   vcl_vector<vcl_vector<float> >& intens_map_side2,
//					   bool gen_rand, float app_val)
//{
//  // generate intensity maps
//  unsigned upw = (unsigned)vcl_ceil(box.width()/8)+1;
//  unsigned uph = (unsigned)vcl_ceil(box.height()/8)+1;
//  unsigned upd = (unsigned)vcl_ceil(box.depth()/8)+1;
//
//  intens_map_bt.resize(upw);
//  intens_map_side1.resize(upw);
//  intens_map_side2.resize(upw);
//
//  if (verbose)
//    vcl_cout << box.width() << ' ' << box.depth() << ' ' << box.height() << vcl_endl;
//
//  if (gen_rand) {
//    for (unsigned i=0; i<upw;i++) {
//      intens_map_bt[i].resize(uph);
//      for (unsigned j=0; j<uph;j++) {
//        intens_map_bt[i][j] = (float)((rand() % 85)/255.0);
//      }
//    }
//
//    for (unsigned i=0; i<upw;i++) {
//      intens_map_side1[i].resize(upd);
//      for (unsigned j=0; j<upd;j++) {
//        intens_map_side1[i][j] = (float)((rand() % 85)/255.0 + 0.4);
//      }
//    }
//
//    for (unsigned i=0; i<uph;i++) {
//      intens_map_side2[i].resize(upd);
//      for (unsigned j=0; j<upd;j++) {
//        intens_map_side2[i][j] = (float)((rand() % 85)/255.0 + 0.7);
//        if (intens_map_side2[i][j] > 1.0f)
//          intens_map_side2[i][j] = 0.99f;
//      }
//    }
//  } else {
//    for (unsigned i=0; i<upw;i++) {
//      intens_map_bt[i].resize(uph);
//      for (unsigned j=0; j<uph;j++) {
//        intens_map_bt[i][j] = app_val;
//      }
//    }
//
//    for (unsigned i=0; i<upw;i++) {
//      intens_map_side1[i].resize(upd);
//      for (unsigned j=0; j<upd;j++) {
//        intens_map_side1[i][j] = app_val;
//      }
//    }
//
//    for (unsigned i=0; i<uph;i++) {
//      intens_map_side2[i].resize(upd);
//      for (unsigned j=0; j<upd;j++) {
//        intens_map_side2[i][j] = app_val;
//        if (intens_map_side2[i][j] > 1.0f)
//          intens_map_side2[i][j] = 0.99f;
//      }
//    }
//  }
//}
//
