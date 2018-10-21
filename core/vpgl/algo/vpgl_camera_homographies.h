// This is core/vpgl/algo/vpgl_camera_homographies.h
#ifndef vpgl_camera_homographies_h_
#define vpgl_camera_homographies_h_
//:
// \file
// \brief Various miscellaneous methods involving cameras
// \author J.L. Mundy
// \date January 2, 2008
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim

#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/vgl_plane_3d.h>

class vpgl_camera_homographies
{
 public:
  // The following four methods compute a homography with respect to a
  // world plane. The homography is computed by transforming the plane
  // to the X-Y plane and applying the inverse transformation to the camera.
  // Columns 0, 1, and 3 of the transformed camera matrix are extracted
  // to form the homography.

  //: create a plane projective transformation from the camera image plane to the specified plane
  static  vgl_h_matrix_2d<double>
    homography_from_camera(vpgl_proj_camera<double> const& cam,
                           vgl_plane_3d<double> const& plane);

  //: create a plane projective transformation from the camera image plane to the specified plane
  static  vgl_h_matrix_2d<double>
    homography_from_camera(vpgl_perspective_camera<double> const& cam,
                           vgl_plane_3d<double> const& plane);


  //: create a plane projective transformation from the specified plane to the camera image plane
  static  vgl_h_matrix_2d<double>
    homography_to_camera(vpgl_proj_camera<double> const& cam,
                         vgl_plane_3d<double> const& plane);

  //: create a plane projective transformation from the specified plane to the camera image plane
  static  vgl_h_matrix_2d<double>
    homography_to_camera(vpgl_perspective_camera<double> const& cam,
                         vgl_plane_3d<double> const& plane);

  //: transform a camera so that its world coordinate system has the specified plane as the world X-Y plane
  static vpgl_perspective_camera<double>
    transform_camera_to_plane(vpgl_perspective_camera<double> const& cam,
                              vgl_plane_3d<double> const& plane);

  //: transform a camera so that its world coordinate system has the specified plane as the world X-Y plane
  static vpgl_proj_camera<double>
    transform_camera_to_plane(vpgl_proj_camera<double> const& cam,
                              vgl_plane_3d<double> const& plane);

  //: transform a point cloud so that its coordinate system has the specified plane as the world X-Y plane
  static std::vector<vgl_point_3d<double> >
  transform_points_to_plane(vgl_plane_3d<double> const& plane,
                            vgl_point_3d<double> const& ref_point,
                            std::vector<vgl_point_3d<double> > const& pts );
 private:
  //: no public constructor - static methods only
  vpgl_camera_homographies() = default;
};

#endif // vpgl_camera_homographies_h_
