#include "vpgl_backproject.h"
//:
// \file
#include <vpgl/algo/vpgl_invmap_cost_function.h>
#include "vgl/vgl_point_2d.h"
#include "vgl/vgl_point_3d.h"
#include "vgl/vgl_plane_3d.h"
#include <vnl/algo/vnl_amoeba.h>
#include "vgl/vgl_intersection.h"
#include "vpgl/vpgl_generic_camera.h"
#include "vnl/vnl_random.h"

bool
vpgl_backproject::bproj_plane(const vpgl_generic_camera<double> & gcam,
                              const vnl_double_2 & image_point,
                              const vnl_double_4 & plane,
                              const vnl_double_3 & /*initial_guess*/,
                              vnl_double_3 & world_point,
                              double /*error_tol*/,
                              double /*relative_diameter*/)
{
  vgl_ray_3d<double> ray;
  vgl_point_3d<double> ipt;
  vgl_plane_3d<double> gplane(plane[0], plane[1], plane[2], plane[3]);
  ray = gcam.ray(image_point[0], image_point[1]);
  if (!vgl_intersection<double>(ray, gplane, ipt))
    return false;
  world_point[0] = ipt.x();
  world_point[1] = ipt.y();
  world_point[2] = ipt.z();
  return true;
}

bool
vpgl_backproject::bproj_plane(const vpgl_proj_camera<double> & pcam,
                              const vnl_double_2 & image_point,
                              const vnl_double_4 & plane,
                              const vnl_double_3 & /*initial_guess*/,
                              vnl_double_3 & world_point,
                              double /*error_tol*/,
                              double /*relative_diameter*/)
{
  vgl_ray_3d<double> ray = pcam.backproject_ray(vgl_homg_point_2d<double>(image_point[0], image_point[1]));
  vgl_plane_3d<double> gplane(plane[0], plane[1], plane[2], plane[3]);

  vgl_point_3d<double> ipt;
  if (!vgl_intersection<double>(ray, gplane, ipt))
    return false;
  world_point[0] = ipt.x();
  world_point[1] = ipt.y();
  world_point[2] = ipt.z();
  return true;
}

//: Backproject an image point onto a plane, start with initial_guess
bool
vpgl_backproject::bproj_plane(const vpgl_camera<double> & cam,
                              const vnl_double_2 & image_point,
                              const vnl_double_4 & plane,
                              const vnl_double_3 & initial_guess,
                              vnl_double_3 & world_point,
                              double error_tol,
                              double relative_diameter)
{
  // special case of a generic camera
  if (cam.type_name() == "vpgl_generic_camera")
  {
    auto gcam = dynamic_cast<const vpgl_generic_camera<double> &>(cam);
    return vpgl_backproject::bproj_plane(
      gcam, image_point, plane, initial_guess, world_point, error_tol, relative_diameter);
  }
  // general case
  vpgl_invmap_cost_function cf(image_point, plane, cam);
  vnl_double_2 x1(0.000, 0.0000);
  cf.set_params(initial_guess, x1);
  vnl_amoeba amoeba(cf);
  amoeba.set_max_iterations(100000);
  amoeba.set_relative_diameter(relative_diameter);
  amoeba.set_zero_term_delta(0.025);
  vnl_vector<double> x(&x1[0], 2);
  amoeba.minimize(x);
  x1 = x;
  cf.point_3d(x1, world_point);
  double u = 0, v = 0, X = world_point[0], Y = world_point[1], Z = world_point[2];
  cam.project(X, Y, Z, u, v);
  vnl_double_2 final_proj;
  final_proj[0] = u;
  final_proj[1] = v;
  double err = (final_proj - image_point).magnitude();
  // was: double err = std::sqrt(cf.f(x));
  if (err > error_tol) // greater than a 20th of a pixel
  {
    std::cerr << "ERROR: backprojection error = " << err << std::endl;
    return false;
  }
  return true;
}


// Only the direction of the vector is important so it can be
// normalized to a unit vector. Two rays can be constructed, one through
// point and one through a point formed by adding the vector to the point
bool
vpgl_backproject::bproj_point_vector(const vpgl_proj_camera<double> & cam,
                                     const vgl_point_2d<double> & point,
                                     const vgl_vector_2d<double> & vect,
                                     vgl_plane_3d<double> & plane)
{
  vgl_homg_point_2d<double> hp(point);
  // get a second point
  vgl_vector_2d<double> vu = vect / vect.length();
  vgl_point_2d<double> point_plus_vect = point + vu;
  vgl_homg_point_2d<double> hp1(point_plus_vect);
  vgl_homg_line_2d<double> hline(hp, hp1);
  vgl_homg_plane_3d<double> hpl = cam.backproject(hline);
  plane = vgl_plane_3d<double>(hpl);
  // might add checks for ideal plane later
  return true;
}
