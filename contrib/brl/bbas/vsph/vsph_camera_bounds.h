// This is brl/bbas/vsph/vsph_camera_bounds.h
#ifndef vsph_camera_bounds_h_
#define vsph_camera_bounds_h_
//:
// \file
// \brief Methods for computing various bounds on cameras and scenes
// \author J. L. Mundy
// \date September 18, 2010

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

class vsph_camera_bounds
{
 public:
  //: the solid angle for a pixel.
  // Applies only to perspective camera.
  // Cone is tangent to pixel.
  // Angles in radians and steradians
  static void pixel_solid_angle(vpgl_perspective_camera<double> const& cam,
                                unsigned u, unsigned v,
                                vgl_ray_3d<double>& cone_axis,
                                double& cone_half_angle,
                                double& solid_angle);


  //: the solid angle (and cone half angle) for the pixel at the  principal point.
  // Applies only to a perspective camera.
  // Cone is tangent to the pixel.
  // Angles in radians and steradians
  static void pixel_solid_angle(vpgl_perspective_camera<double> const& cam,
                                double& cone_half_angle,
                                double& solid_angle);

  //: the solid angle for an image.
  // Applies only to perspective camera.
  // The cone axis passes through principal point, i.e. principal ray.
  // The cone is tangent to the square defined by the image diagonal.
  // Angles in radians and steradians
  static void image_solid_angle(vpgl_perspective_camera<double> const& cam,
                                vgl_ray_3d<double>& cone_axis,
                                double& cone_half_angle,
                                double& solid_angle);


  //: the solid angle for an image.
  // Applies only to perspective camera.
  // The cone axis passes through principal point, i.e. principal ray.
  // The cone is tangent to the square defined by the image diagonal.
  // Code axis is not returned.
  // Angles in radians and steradians
  static void image_solid_angle(vpgl_perspective_camera<double> const& cam,
                                double& cone_half_angle,
                                double& solid_angle);
  //: the solid angle for a scene bounding box.
  // The cone is tangent to the 3-d scene box.
  // Angles in radians and steradians
  static bool box_solid_angle(vpgl_perspective_camera<double> const& cam,
                              vgl_box_3d<double> const& box,
                              vgl_ray_3d<double>& cone_axis,
                              double& cone_half_angle,
                              double& solid_angle);

  //: The angular interval of the rotation about the principal axis that bounds a pixel at the smallest image radius, i.e., 1/2 the smallest image dimension.
  //  The angular wedge is tangent to the pixel
  static double rotation_angle_interval(vpgl_perspective_camera<double> const& cam);

  //: conversion between solid angle and cone half angle
  static double solid_angle(double cone_half_angle);

  //: conversion between solid angle and cone half angle
  static double cone_half_angle(double solid_angle);

  //: The relative rotation and translation between camera \p c0 and \p c1
  //  In more detail, the relative transformation aligns c1 with the
  //  identity camera
  static void relative_transf(vpgl_perspective_camera<double> const& c0,
                              vpgl_perspective_camera<double> const& c1,
                              vgl_rotation_3d<double>& rel_rot,
                              vgl_vector_3d<double>& rel_trans);

  //: For cameras with nearly parallel rays a pixel backprojects to a cylinder. The cylinder circumscribes the pixel. Returns false if (u, v) are out of bounds.
  static bool pixel_cylinder(vpgl_generic_camera<double> const& cam,
                             unsigned u, unsigned v,
                             vgl_ray_3d<double>& cylinder_axis,
                             double& cylinder_radius);

  //: The (world coordinates) 2d Bounding Box for a camera at a specific Z plane
  // \returns false if plane and camera do not intersect
  static bool planar_bounding_box(vpgl_perspective_camera<double> const& c,
                                  vgl_box_2d<double>& bbox,
                                  double z_plane=0.0);

  //: The union of the planar bounding boxes at specific z plane for each camera passed in
  static bool planar_bounding_box(std::vector<vpgl_perspective_camera<double> > const& cams,
                                  vgl_box_2d<double>& bbox,
                                  double z_plane=0.0);

 private:
  //: constructor private - class contains static methods only
  vsph_camera_bounds() = delete;
  //: destructor private - class contains static methods only
  ~vsph_camera_bounds() = delete;
};

//: scan the principal ray over a cone defined by the half apex angle.
//  It is assumed the cone axis is the positive z direction.
//  Returns a 3-d rotation from the positive z direction to the current
//  iterator rotation state. The cone is scanned in uniform steps
//  defined by the increment. The actual number of samples is returned
//  by n_samples. The input is the goal number of samples.
class principal_ray_scan
{
 public:
  principal_ray_scan(double cone_half_angle, unsigned& n_samples);
  ~principal_ray_scan() = default;
  //: number of scan states
  unsigned n_states() const {return theta_.size();}
  //: reset the scan state
  void reset();
  //:the next scan state. Returns false if done
  bool next();
  //: the camera rotation corresponding to the current state of the principal ray.
  // \p alpha is an additional rotation about the principal ray, $-\pi<\alpha<=\pi$, default 0.
  vgl_rotation_3d<double> rot(double alpha = 0.0) const {return rot(index_, alpha);}
  //: rotation for a given scan index
  vgl_rotation_3d<double> rot(unsigned i, double alpha = 0.0) const;
  //: spherical elevation angle for current state
  double theta() const {return theta_[index_];}
  //: spherical elevation angle for a given scan index
  double theta(unsigned i) const {return theta_[i];}
  //: spherical azimuth angle for current state
  double phi() const {return phi_[index_];}
  //: spherical azimuth angle for a given scan index
  double phi(unsigned i) const {return phi_[i];}
  //: point on the unit sphere for current scan state
  vgl_point_3d<double> pt_on_unit_sphere() const {return pt_on_unit_sphere(index_);}
  //: point on the unit sphere for a given scan index
  vgl_point_3d<double> pt_on_unit_sphere(unsigned i) const;

 private:
  principal_ray_scan() = default;
  int index_;
  std::vector<double> theta_;
  std::vector<double> phi_;
};

#endif // vsph_camera_bounds_h_
