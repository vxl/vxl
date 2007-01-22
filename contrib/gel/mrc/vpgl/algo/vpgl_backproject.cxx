//:
// \file
#include <vcl_cmath.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vpgl/algo/vpgl_invmap_cost_function.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vnl/algo/vnl_amoeba.h>

//: backproject an image point onto a world plane

bool vpgl_backproject::bproj_plane(vpgl_rational_camera<double> const& rcam,
                                   vnl_double_2 const& image_point,
                                   vnl_double_4 const& plane,
                                   vnl_double_3 const& initial_guess,
                                   vnl_double_3& world_point)
{
  vpgl_invmap_cost_function cf(image_point, plane, &rcam);
  vnl_double_2 x1(0.0, 0.0);

  cf.set_params(initial_guess, x1);
  vnl_amoeba amoeba(cf);
  amoeba.set_max_iterations(1000);
  vnl_vector<double> x(&x1[0], 2);
  amoeba.minimize(x);
  cf.point_3d(x, world_point);
  vnl_double_2 final_proj = rcam.project(world_point);
  double err = vcl_sqrt(cf.f(x));
  if (err>.05) // greater than a 20th of a pixel
    return false;
  return true;
}

bool vpgl_backproject::bproj_plane(vpgl_rational_camera<double> const& rcam,
                                   vgl_point_2d<double> const& image_point,
                                   vgl_plane_3d<double> const& plane,
                                   vgl_point_3d<double> const& initial_guess,
                                   vgl_point_3d<double>& world_point)
{
  //simply convert to vnl interface
  vnl_double_2 ipt;
  vnl_double_3 ig, wp;
  vnl_double_4 pl;
  ipt[0]=image_point.x(); ipt[1]=image_point.y();
  pl[0]=plane.a(); pl[1]=plane.b(); pl[2]=plane.c(); pl[3]=plane.d();
  ig[0]=initial_guess.x();  ig[1]=initial_guess.y();  ig[2]=initial_guess.z();
  bool success = vpgl_backproject::bproj_plane(rcam, ipt, pl, ig, wp);
  world_point.set(wp[0], wp[1], wp[2]);
  return success;
}
