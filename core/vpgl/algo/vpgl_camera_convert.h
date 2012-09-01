// This is core/vpgl/algo/vpgl_camera_convert.h
#ifndef vpgl_camera_convert_h_
#define vpgl_camera_convert_h_
//:
// \file
// \brief Several routines for converting camera types
// \author J.L. Mundy
// \date July 18, 2005
//
// Should template this class.

#include <vcl_vector.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

//: Basic least squares solution for a general projective camera given corresponding world and image points.
class vpgl_proj_camera_convert
{
 public:
  //:Find a projective camera that best approximates the rational camera at the world center (lon (deg), lat (deg), elev (meters) )
  static  bool convert(vpgl_rational_camera<double> const& rat_cam,
                       vgl_point_3d<double> const& world_center,
                       vpgl_proj_camera<double>& camera);

  //:An auxiliary matrix that transforms (normalizes) world points prior to projection by a projective camera.  (lon, lat, elevation)->[-1, 1].
  static vgl_h_matrix_3d<double>
    norm_trans(vpgl_rational_camera<double> const& rat_cam);

 private:
  //:default constructor (is private)
  vpgl_proj_camera_convert();
};


//:Various methods for computing a perspective camera
class vpgl_perspective_camera_convert
{
 public:

  //: Convert from a rational camera
  // Put the resulting camera into camera, return true if successful.
  // The approximation volume defines the region of space (lon (deg), lat (deg), elev (meters))
  //  where the perspective approximation is valid. Norm trans is a pre-multiplication
  // of the perspective camera to account for scaling the lon, lat and elevation
  // to the range [-1, 1]
  static bool convert( vpgl_rational_camera<double> const& rat_cam,
                       vgl_box_3d<double> const& approximation_volume,
                       vpgl_perspective_camera<double>& camera,
                       vgl_h_matrix_3d<double>& norm_trans);

  //: Convert from rational camera using a local Euclidean coordinate system.
  static bool convert_local( vpgl_rational_camera<double> const& rat_cam,
                             vgl_box_3d<double> const& approximation_volume,
                             vpgl_perspective_camera<double>& camera,
                             vgl_h_matrix_3d<double>& norm_trans);

 private:
  vpgl_perspective_camera_convert();
};

//:Various methods for converting to a generic camera
class vpgl_generic_camera_convert
{
 public:

  //: Convert a local rational camera to a generic camera
  static bool convert( vpgl_local_rational_camera<double> const& rat_cam,
                       int ni, int nj,
                       vpgl_generic_camera<double> & gen_cam, unsigned level = 0);

  //: Convert a local rational camera to a generic camera, using user-specified z bounds
  //  Note that the z values are relative to the local coordinate system
  static bool convert( vpgl_local_rational_camera<double> const& rat_cam,
                       int ni, int nj,
                       vpgl_generic_camera<double> & gen_cam,
                       double local_z_min, double local_z_max, unsigned level = 0);

  //: Convert a proj_camera to a generic camera
  static bool convert( vpgl_proj_camera<double> const& prj_cam,
                       int ni, int nj,
                       vpgl_generic_camera<double> & gen_cam, unsigned level = 0);

  //: Convert a perspective_camera to a generic camera
  static bool convert( vpgl_perspective_camera<double> const& per_cam,
                       int ni, int nj,
                       vpgl_generic_camera<double> & gen_cam, unsigned level = 0);
#if 0
  {
    vpgl_perspective_camera<double> nc_cam(per_cam);
    vpgl_proj_camera<double>* prj_cam_ptr =
      dynamic_cast<vpgl_proj_camera<double>*>(&nc_cam);
    if (!prj_cam_ptr) return false;
    return convert(*prj_cam_ptr, ni, nj, gen_cam,level);
  }
#endif

  static bool convert_with_margin( vpgl_perspective_camera<double> const& per_cam,
                                   int ni, int nj,
                                   vpgl_generic_camera<double> & gen_cam, int margin, unsigned level = 0);

  //: Convert an affine_camera to a generic camera
  static bool convert( vpgl_affine_camera<double> const& aff_cam,
                       int ni, int nj, vpgl_generic_camera<double> & gen_cam, unsigned level = 0);

  //::convert an abstract camera to generic camera
  static bool convert( vpgl_camera_double_sptr const& camera, int ni, int nj,
                       vpgl_generic_camera<double> & gen_cam, unsigned level = 0);

  //: Convert a geocam (transformtaion matrix read from a geotiff header + an lvcs) to a generic camera
  static bool convert( vpgl_geo_camera& geocam, int ni, int nj, double height,
                       vpgl_generic_camera<double> & gen_cam, unsigned level = 0);

  // === utility methods ===
 private:
  //: interpolate rays to fill next higher resolution pyramid layer
  static bool
    upsample_rays(vcl_vector<vgl_ray_3d<double> > const& ray_nbrs,
                  vgl_ray_3d<double> const& ray,
                  vcl_vector<vgl_ray_3d<double> >& interp_rays);
  //: interpolate a span of rays base on a linear interpolation. n_grid is the step distance from r1. r0 and r1 are one unit apart.
  static vgl_ray_3d<double> interp_pair(vgl_ray_3d<double> const& r0,
                                        vgl_ray_3d<double> const& r1,
                                        double n_grid);
  vpgl_generic_camera_convert();
};

#endif // vpgl_camera_convert_h_
