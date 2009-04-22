#ifndef test_utils_h
#define test_utils_h

#include <boct/boct_tree.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_rational_camera.h>

typedef boct_tree<short,vgl_point_3d<double> > tree_type;

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
                           vpgl_perspective_camera<double>& cam);

vpgl_rational_camera<double>
perspective_to_rational(vpgl_perspective_camera<double>& cam_pers);

vpgl_camera_double_sptr generate_camera_top(vgl_box_3d<double>& world);


//bool generate_test_boxes(double box_min_x, double box_min_y, double box_min_z,
//                                    double box_dim_x, double box_dim_y, double box_dim_z,
//                                    double world_dim_x, double world_dim_y, double world_dim_z,
//                                    vcl_vector<vgl_box_3d<double> >& boxes, bool gen_2box = true);
//
//void gen_texture_map(vgl_box_3d<double> box,
//					   vcl_vector<vcl_vector<float> >& intens_map_bt,
//					   vcl_vector<vcl_vector<float> >& intens_map_side1,
//					   vcl_vector<vcl_vector<float> >& intens_map_side2,
//					   bool gen_rand, float app_val);
#endif
