#ifndef bwm_algo_t_
#define bwm_algo_t_

#include <vcl_vector.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>

#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>

#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_rational_camera.h>

#include <vil/vil_image_resource_sptr.h>

class bwm_algo
{
 public:
  static void get_vertices_xy(vsol_polygon_2d_sptr poly2d, float **x, float **y);

  static void get_vertices_xy(vsol_polygon_2d_sptr poly2d, double **x, double **y);

  static void get_vertices_xy(vsol_polyline_2d_sptr poly2d, float **x, float **y);

  static void get_vertices_xyz(vsol_polygon_3d_sptr poly3d, double **x, double **y, double **z);

  static vsol_polygon_3d_sptr move_points_to_plane(vsol_polygon_3d_sptr polygon);

  static vsol_polygon_3d_sptr move_points_to_plane(vcl_vector<vsol_point_3d_sptr> points);


  static vgl_point_3d<double> fit_sphere_to_corner(vgl_point_3d<double> P1, vgl_vector_3d<double> N1,
                                                   vgl_point_3d<double> P2, vgl_vector_3d<double> N2,
                                                   vgl_point_3d<double> P3, vgl_vector_3d<double> N3,
                                                   double r);

  static bool find_intersection_points(vgl_homg_point_2d<double> img_point1,
                                       vgl_homg_point_2d<double> img_point2,
                                       vsol_polygon_3d_sptr poly3d,
                                       vgl_point_3d<double>& point1,
                                       vgl_point_3d<double>& l1, vgl_point_3d<double>& l2,
                                       vgl_point_3d<double>& point2,
                                       vgl_point_3d<double>& l3, vgl_point_3d<double>& l4);

  static vpgl_rational_camera<double>* extract_nitf_camera(vil_image_resource_sptr img);
  static vpgl_rational_camera<double>* extract_nitf_camera(vcl_string img_path);
};

#endif
