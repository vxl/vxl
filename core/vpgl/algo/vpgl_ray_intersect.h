// This is core/vpgl/algo/vpgl_ray_intersect.h
#ifndef vpgl_ray_intersect_h_
#define vpgl_ray_intersect_h_
//:
// \file
// \brief Find the 3-d point closest to the intersection of rays from >= 2 cameras
// \author J. L. Mundy
// \date July 30, 2007
//
// given a sequence of image points (image_pts[i]), as viewed through a
// corresponding sequence of cameras (cams[i]), this class attempts to
// find the point in space (intersection_vert), such that the cumulative
// error of the image of intersection_vert wrt each image_pts[i] is
// minimized.  More precisely, it finds intersection_vert for which the
// following quantity is attempted to be minimized (for n cameras and
// image points):
//
// $\sqrt(\sum_{i=1}^{n}(  (u_i-image_pts[i][1])^2
//                       + (v_i-image_pts[i][2])^2))$
//
//  where (u_i, v_i) is the image of intersection_vert through
//  camera cams[i], and image_pts[i][1 and 2] are the "u" and "v"
//  coordinates of image_pts[i].
//

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_camera.h>


template<typename T>
class vpgl_ray_intersect
{
 public:

  vpgl_ray_intersect(unsigned dim);
  ~vpgl_ray_intersect() = default;

  //: Intersect the rays. return false if intersection fails
  // Note image points are not homogeneous because require
  // finite points to measure projection error
  bool intersect(std::vector<const vpgl_camera<T>* > const& cams,
                 std::vector<vgl_point_2d<T> > const& image_pts,
                 vgl_point_3d<T> const& initial_intersection,
                 vgl_point_3d<T>& intersection);

 protected:
  //members
  unsigned dim_;
  std::vector<const vpgl_camera<T>* > f_cameras_; //cameras
  std::vector<vgl_point_2d<T> > f_image_pts_; //image points
};


#endif // vpgl_ray_intersect_h_
