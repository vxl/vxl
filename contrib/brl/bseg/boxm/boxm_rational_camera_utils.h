#ifndef boxm_rational_camera_utils_h
#define boxm_rational_camera_utils_h

#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_backproject.h>

class boxm_rational_camera_utils
{
 public:
  static vgl_plane_3d<double>  boxm_find_parallel_image_plane(vpgl_rational_camera<double> * rcam,
                                                              vgl_plane_3d<double> top,
                                                              vgl_plane_3d<double> bottom,
                                                              unsigned ni, unsigned nj);
  static void project_corners_rational_camera(vcl_vector<vgl_point_3d<double> > const& corners,
                                              vpgl_rational_camera<double> * rcam,vgl_plane_3d<double> plane,
                                              double* xverts, double* yverts,
                                              float* vertdist);
};

class abs_functor
{
 public:
  abs_functor(){}

  float operator()(float x) const { return vcl_fabs(x); }
};

#endif
