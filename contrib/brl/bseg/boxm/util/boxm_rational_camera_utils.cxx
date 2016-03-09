#include "boxm_rational_camera_utils.h"
#include <vgl/vgl_point_2d.h>

vgl_plane_3d<double>
boxm_rational_camera_utils::boxm_find_parallel_image_plane(vpgl_rational_camera<double> * rcam,
                                                           vgl_plane_3d<double> top,
                                                           vgl_plane_3d<double> bottom,
                                                           unsigned ni, unsigned nj)
{
   vgl_point_3d<double> top_guess(0,0,-top.d()/top.c());
   vgl_point_3d<double> bottom_guess(0,0,-bottom.d()/bottom.c());
   vpgl_backproject::bproj_plane(*rcam,vgl_point_2d<double>(ni/2,nj/2),top,top_guess,top_guess);
   vpgl_backproject::bproj_plane(*rcam,vgl_point_2d<double>(ni/2,nj/2),bottom,bottom_guess,bottom_guess);

   vgl_plane_3d<double> parallelplane(bottom_guess-top_guess,top_guess);

   return parallelplane;
}

void
boxm_rational_camera_utils::project_corners_rational_camera(std::vector<vgl_point_3d<double> > const& corners,
                                                            vpgl_rational_camera<double> * rcam,vgl_plane_3d<double> plane,
                                                            double* xverts, double* yverts,
                                                            float* vertdist)
{
  double a=plane.a();
  double b=plane.b();
  double c=plane.c();
  double d=plane.d();
  double denom=std::sqrt(a*a+b*b+c*c);
  for (unsigned i=0; i<corners.size(); ++i)
  {
    rcam->project(corners[i].x(), corners[i].y(), corners[i].z(), xverts[i], yverts[i]);
    vertdist[i]=float((corners[i].x()*a+corners[i].y()*b+corners[i].z()*c+d)/denom);
  }
}
