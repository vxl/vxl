// This is gel/mrc/vpgl/algo/vpgl_ray_intersect.h
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

#include <vcl_vector.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_least_squares_function.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_camera.h>
//
//:
// The image offsets of rational cameras typically must be adjusted to
// compensate for errors in geographic alignment. This algorithm finds
// a set of minium translations that registers the input set of images.
// After registration, the images have geographically corresponding rational
// cameras. That is, a visible 3-d point will project into its corresponding
// image location in all the images.

class vpgl_ray_intersect_lsqr : public vnl_least_squares_function
{
 public:
  //: Constructor
  vpgl_ray_intersect_lsqr(vcl_vector<vpgl_camera<double>* > const& cams,
                          vcl_vector<vgl_point_2d<double> > const& image_pts,
                          unsigned num_residuals);

  //: Destructor
  virtual ~vpgl_ray_intersect_lsqr() {}

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  fx has been sized appropriately before the call.
  virtual void f(vnl_vector<double> const& intersection_point,
                 vnl_vector<double>& image_errors);

#if 0
  //: Called after each LM iteration to print debugging etc.
  virtual void trace(int iteration, vnl_vector<double> const& x, vnl_vector<double> const& fx);
#endif

 protected:
  vpgl_ray_intersect_lsqr();//not valid
  vcl_vector<vpgl_camera<double>* > f_cameras_; //cameras
  vcl_vector<vgl_point_2d<double> > f_image_pts_; //image points
};

class vpgl_ray_intersect
{
 public:

  vpgl_ray_intersect(unsigned dim);
  ~vpgl_ray_intersect(){}

  //: Intersect the rays. return false if intersection fails
  // Note image points are not homogeneous because require
  // finite points to measure projection error
  bool intersect(vcl_vector<vpgl_camera<double>* > const& cams,
                 vcl_vector<vgl_point_2d<double> > const& image_pts,
                 vgl_point_3d<double> const& initial_intersection,
                 vgl_point_3d<double>& intersection);

 protected:
  //members
  unsigned dim_;
  vcl_vector<vpgl_camera<double>* > f_cameras_; //cameras
  vcl_vector<vgl_point_2d<double> > f_image_pts_; //image points
};


#endif // vpgl_ray_intersect_h_
