#include "vpgl_backproject.h"
//:
// \file
#include <vpgl/algo/vpgl_invmap_cost_function.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vgl/vgl_intersection.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vnl/vnl_random.h>

//: Backproject an image point onto a plane, start with initial_guess
bool vpgl_backproject::bproj_plane(const vpgl_camera<double>* cam,
                                   vnl_double_2 const& image_point,
                                   vnl_double_4 const& plane,
                                   vnl_double_3 const& initial_guess,
                                   vnl_double_3& world_point,
                                   double error_tol,
                                   double relative_diameter)
{
  // special case of a generic camera
  if (cam->type_name()=="vpgl_generic_camera")
  {
    vgl_ray_3d<double> ray;
    vgl_point_3d<double> ipt;
    vgl_plane_3d<double> gplane(plane[0], plane[1], plane[2], plane[3]);
    const auto* gcam = dynamic_cast<const vpgl_generic_camera<double>*>(cam);
    ray = gcam->ray(image_point[0], image_point[1]);
    if (!vgl_intersection<double>(ray, gplane, ipt))
      return false;
    world_point[0]=ipt.x(); world_point[1]=ipt.y(); world_point[2]=ipt.z();
    return true;
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
  double u=0, v=0, X=world_point[0], Y=world_point[1], Z=world_point[2];
  cam->project(X, Y, Z, u, v);
   vnl_double_2 final_proj;
   final_proj[0]=u; final_proj[1]=v;
  double err = (final_proj-image_point).magnitude();
  // was: double err = std::sqrt(cf.f(x));
  if (err > error_tol) // greater than a 20th of a pixel
  {
      std::cerr << "ERROR: backprojection error = " << err << std::endl;
      return false;
  }
  return true;
}

  // vgl interface

//: Backproject an image point onto a plane, start with initial_guess
bool vpgl_backproject::bproj_plane(const vpgl_camera<double>*  cam,
                                   vgl_point_2d<double> const& image_point,
                                   vgl_plane_3d<double> const& plane,
                                   vgl_point_3d<double> const& initial_guess,
                                   vgl_point_3d<double>& world_point,
                                   double error_tol,
                                   double relative_diameter)
{
  //simply convert to vnl interface
  vnl_double_2 ipt;
  vnl_double_3 ig, wp;
  vnl_double_4 pl;
  ipt[0]=image_point.x(); ipt[1]=image_point.y();
  pl[0]=plane.a(); pl[1]=plane.b(); pl[2]=plane.c(); pl[3]=plane.d();
  ig[0]=initial_guess.x();  ig[1]=initial_guess.y();  ig[2]=initial_guess.z();
  bool success = vpgl_backproject::bproj_plane(cam, ipt, pl, ig, wp, error_tol, relative_diameter);
  world_point.set(wp[0], wp[1], wp[2]);
  return success;
}


//: Backproject an image point onto a world plane
bool vpgl_backproject::bproj_plane(vpgl_rational_camera<double> const& rcam,
                                   vnl_double_2 const& image_point,
                                   vnl_double_4 const& plane,
                                   vnl_double_3 const& initial_guess,
                                   vnl_double_3& world_point,
                                   double error_tol,
                                   double relative_diameter)
{
  const auto*  cam = static_cast<const vpgl_camera<double>* >(&rcam);
  return bproj_plane(cam, image_point, plane, initial_guess, world_point, error_tol, relative_diameter);
}

//: Backproject an image point onto a world plane
bool vpgl_backproject::bproj_plane(vpgl_rational_camera<double> const& rcam,
                                   vgl_point_2d<double> const& image_point,
                                   vgl_plane_3d<double> const& plane,
                                   vgl_point_3d<double> const& initial_guess,
                                   vgl_point_3d<double>& world_point,
                                   double error_tol,
                                   double relative_diameter)
{
  const auto* const cam = static_cast<const vpgl_camera<double>* >(&rcam);
  return bproj_plane(cam, image_point, plane, initial_guess, world_point, error_tol, relative_diameter);
}

//Only the direction of the vector is important so it can be
//normalized to a unit vector. Two rays can be constructed, one through
//point and one through a point formed by adding the vector to the point
bool
vpgl_backproject::bproj_point_vector(vpgl_proj_camera<double> const& cam,
                               vgl_point_2d<double> const& point,
                               vgl_vector_2d<double> const& vect,
                               vgl_plane_3d<double>& plane)
{
  vgl_homg_point_2d<double> hp(point);
  //get a second point
  vgl_vector_2d<double> vu = vect/vect.length();
  vgl_point_2d<double> point_plus_vect = point + vu;
  vgl_homg_point_2d<double> hp1(point_plus_vect);
  vgl_homg_line_2d<double> hline(hp, hp1);
  vgl_homg_plane_3d<double> hpl = cam.backproject(hline);
  plane = vgl_plane_3d<double>(hpl);
  //might add checks for ideal plane later
  return true;
}


//: Use backprojection to determine direction to camera from 3-d point
bool vpgl_backproject::direction_to_camera(vpgl_local_rational_camera<double> const& cam,
                                           vgl_point_3d<double> const& point,
                                           vgl_vector_3d<double> &to_camera,
                                           double error_tol,
                                           double relative_diameter)
{
  // assumes that camera is above point of interest
  // project point to image, and backproject to another z-plane, vector points to sensor
  vgl_point_2d<double> img_pt = cam.project(point);
  constexpr double z_off = 10.0;
  vgl_plane_3d<double> plane_high(0,0,1,-(point.z()+z_off));
  vgl_point_3d<double> point_high;
  vgl_point_3d<double> guess(point.x(),point.y(),point.z() + z_off);
  if (!bproj_plane(cam, img_pt, plane_high, guess, point_high, error_tol, relative_diameter)) {
    return false;
  }
  // this assumes camera z > point.z
  to_camera = point_high - point;
  // normalize vector
  normalize(to_camera);

  return true;

}
