// This is gel/mrc/vpgl/algo/vpgl_ray.cxx
#include "vpgl_ray.h"
//:
// \file
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_intersection.h>
#include <vpgl/algo/vpgl_invmap_cost_function.h>
#include "vpgl_backproject.h"

bool vpgl_ray::ray(const vpgl_camera<double>* cam,
                   vnl_double_3 const& point_3d,
                   vnl_double_3& r)
{
  //create an image point
  double u, v;
  cam->project(point_3d[0], point_3d[1], point_3d[2], u, v);
  vnl_double_2 image_point(u, v);

  //construct a shifted plane by 1 unit in - z direction
  vnl_double_4 plane(0, 0, 1.0, -point_3d[2]+ 1.0);

  //backproject onto the shifted plane
  vnl_double_3 shifted_point;

  if (!vpgl_backproject::bproj_plane(cam, image_point, plane, point_3d,
                                     shifted_point))
    return false;
  //The ray direction is just the difference
  r = shifted_point - point_3d;
  r.normalize();
  return true;
}

bool vpgl_ray::ray(const vpgl_camera<double>*  cam,
                   vgl_point_3d<double> const& point_3d,
                   vgl_vector_3d<double>& r)
{
  vnl_double_3 p3d(point_3d.x(), point_3d.y(), point_3d.z());
  vnl_double_3 tr;
  bool success = vpgl_ray::ray(cam, p3d, tr);
  if (!success) return false;
  r.set(tr[0], tr[1], tr[2]);
  return true;
}

//construct a ray at a 3-d point with an origin lying in the origin_z plane
bool vpgl_ray::ray(const vpgl_camera<double>*  cam,
                   vgl_point_3d<double> const& point_3d,
                   double origin_z,
                   vgl_ray_3d<double>& ray)
{
  vgl_plane_3d<double> pl(0.0, 0.0, 1.0, -origin_z);
  vgl_vector_3d<double> dir;
  bool success = vpgl_ray::ray(cam, point_3d, dir);
  if (!success) return false;
  // create an infinite line along dir
  vgl_infinite_line_3d<double> infl(point_3d, dir);
  vgl_point_3d<double> origin;
  // intersect with the z plane
  if (!vgl_intersection(infl, pl, origin))
    return false;
  ray.set(origin, dir);
  return true;
}

bool vpgl_ray::ray(vpgl_rational_camera<double> const& rcam,
                   vnl_double_3 const& point_3d,
                   vnl_double_3& ray)
{
    const vpgl_camera<double>*  cam =
      static_cast<const vpgl_camera<double>* >(&rcam);
    return vpgl_ray::ray(cam, point_3d, ray);
}

bool vpgl_ray::ray(vpgl_rational_camera<double> const& rcam,
                   vgl_point_3d<double> const& point_3d,
                   vgl_vector_3d<double>& ray)
{
    const vpgl_camera<double>*  cam =
      static_cast<const vpgl_camera<double>* >(&rcam);

   return vpgl_ray::ray(cam,point_3d,ray);
}

bool vpgl_ray::ray(vpgl_rational_camera<double> const& rcam,
                   vgl_point_3d<double> const& point_3d,
                   vgl_ray_3d<double>& ray)
{
  double z_off = rcam.offset(vpgl_rational_camera<double>::Z_INDX);
  double z_scale = rcam.scale(vpgl_rational_camera<double>::Z_INDX);
  double zmax = z_off + z_scale;

    const vpgl_camera<double>*  cam =
      static_cast<const vpgl_camera<double>* >(&rcam);

   return vpgl_ray::ray(cam,point_3d,zmax,ray);
}


// compute a ray in local Cartesian coordinates for a local rational cam
bool vpgl_ray::ray(vpgl_local_rational_camera<double> const& lrcam,
                   const double u, const double v,
                   vgl_point_3d<double>& origin,
                   vgl_vector_3d<double>& dir)
{
  // find the horizontal plane at the top of the 3-d region
  // of valid RPC projection
  double z_off = lrcam.offset(vpgl_rational_camera<double>::Z_INDX);
  double z_scale = lrcam.scale(vpgl_rational_camera<double>::Z_INDX);
  double zmax = z_off + z_scale;

  // find the point of intersection of the back-projected ray with zmax
  vgl_plane_3d<double> top_plane(0.0, 0.0, 1.0, -zmax);
  vgl_point_2d<double> image_point(u, v);
  vgl_point_3d<double> initial_guess(0.0, 0.0, zmax);
  vpgl_local_rational_camera<double>* lrcam_ptr =
    const_cast<vpgl_local_rational_camera<double>*>(&lrcam);
  vpgl_camera<double>* cam = static_cast<vpgl_camera<double>*>(lrcam_ptr);
  if (!vpgl_backproject::bproj_plane(cam, image_point, top_plane,
                                     initial_guess, origin))
    return false;

  // find the point of intersection of the back-projected ray with the
  // plane at mid elevation.
  //
  vgl_plane_3d<double> mid_plane(0.0, 0.0, 1.0, -z_off);
  vgl_point_3d<double> mid_initial_guess(0.0, 0.0, z_off), mid_point;
  if (!vpgl_backproject::bproj_plane(cam, image_point, mid_plane,
                                     mid_initial_guess, mid_point))
    return false;

  dir = mid_point - origin;
  dir/=dir.length();//unit vector

  return true;
}

//: compute a ray in local Cartesian coordinates at a given (u, v)
bool vpgl_ray::ray(vpgl_local_rational_camera<double> const& lrcam,
                   const double u, const double v,
                   vgl_ray_3d<double>& ray)
{
  vgl_point_3d<double> origin; vgl_vector_3d<double> dir;
  bool success = vpgl_ray::ray(lrcam, u, v, origin, dir);
  if (!success) return false;
  ray.set(origin, dir);
  return true;
}

// compute a ray in local Cartesian coordinates for a local rational cam
bool vpgl_ray::plane_ray(vpgl_local_rational_camera<double> const& lrcam,
                         const vgl_point_2d<double> image_point1,
                         const vgl_point_2d<double> image_point2,
                         vgl_plane_3d<double>& plane)
{
  // find the horizontal plane at the top of the 3-d region
  // of valid RPC projection
  double z_off = lrcam.offset(vpgl_rational_camera<double>::Z_INDX);
  double z_scale = lrcam.scale(vpgl_rational_camera<double>::Z_INDX);
  double zmax = z_off + z_scale;

  // find the point of intersection of the back-projected ray with zmax
  vgl_plane_3d<double> top_plane(0.0, 0.0, 1.0, -zmax);
  //vgl_point_2d<double> image_point(u, v);
  vgl_point_3d<double> initial_guess(0.0, 0.0, zmax);
  vgl_point_3d<double> point1,point2;
  vpgl_local_rational_camera<double>* lrcam_ptr =
    const_cast<vpgl_local_rational_camera<double>*>(&lrcam);
  vpgl_camera<double>* cam = static_cast<vpgl_camera<double>*>(lrcam_ptr);
  if (!vpgl_backproject::bproj_plane(cam, image_point1, top_plane,
                                     initial_guess, point1))
    return false;
  if (!vpgl_backproject::bproj_plane(cam, image_point2, top_plane,
                                     initial_guess, point2))
    return false;

  // find the point of intersection of the back-projected ray with the
  // plane at mid elevation.
  //
  vgl_plane_3d<double> mid_plane(0.0, 0.0, 1.0, -z_off);
  vgl_point_3d<double> mid_initial_guess(0.0, 0.0, z_off), mid_point1;
  if (!vpgl_backproject::bproj_plane(cam, image_point1, mid_plane,
                                     mid_initial_guess, mid_point1))
    return false;

  plane=vgl_plane_3d<double>(point1,point2,mid_point1);

  return true;
}

bool vpgl_ray::ray(vpgl_perspective_camera<double> const& cam,
                   vgl_point_3d<double> const& world_pt,
                   vgl_ray_3d<double>& ray)
{
  if (cam.is_behind_camera(vgl_homg_point_3d<double>(world_pt.x(), world_pt.y(), world_pt.z())))
    return false;
  ray = vgl_ray_3d<double>(cam.camera_center(), world_pt);
  return true;
}
