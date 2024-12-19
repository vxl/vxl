// This is core/vpgl/vpgl_radial_tangential_distortion.h
#ifndef vpgl_radial_tangential_distortion_h_
#define vpgl_radial_tangential_distortion_h_
//:
// \file
// \brief The standard distortion model including radial and tangential distortion
// \author J.L. Mundy
// \date March 2, 2019
//
//   It is typical to account for both radial and tangential distortion in camera calibration
//   The general form is as follows
// (see opencv
// https://docs.opencv.org/3.0-beta/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html?highlight=projectpoints)
//
//          (1 + k1*r^2 + k2*r^4 + k3*r^6)
//   x' = x ------------------------------  + 2p1(x*y) + p2(r^2 + 2x^2)
//          (1 + k4*r^2 + k5*r*6 + k6*r*6)
//
//
//          (1 + k1*r^2 + k2*r^4 + k3*r^6)
//   y' = y ------------------------------  + p1(r^2 + 2y^2) 2p2(x*y) +
//          (1 + k4*r^2 + k5*r*6 + k6*r*6)
//
//  note that (x, y) are focal plane coordinates and (x', y') are distorted focal plane coordinates.
//
//     u        x'
//     v   = K  y' , where K is the calibration matrix
//     1        1
//
//                                      wx             X
// To apply lens distortion,    compute wy = [ R | t ] Y
//                                      w              Z
//                                                     1
// compute x'(x, y)  and y'(x, y) from above, and then apply the calibration matrix to determine u, v.
//
// To undistort an image, apply the inverse of the calibration matrix to compute x' and y'
// non-linearly solve the distortion equations for x and y (the undistorted projection onto the image plane)
// Determine undistorted (u, v) from (x, y) as,
//
//     u        x
//     v   = K  y , where K is the calibration matrix
//     1        1
// Note that in other vpgl lens distortion implementations, e.g. vpgl_poly_radial_distortion only k coeficients are
// specified and in terms of image coordinates not focal plane coordinates, as assumed in the standard definition.
// However, by by specifying a center of distortion the correction can work similarly, but with modified coefficients.
//
#include "vpgl_lens_distortion.h"
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include "vpgl_calibration_matrix.h"

template <class T>
class vpgl_radial_tangential_distortion : public vpgl_lens_distortion<T>
{
public:
  //: Constructor
  vpgl_radial_tangential_distortion(const std::vector<T> k, T p1, T p2)
    : center_(vgl_point_2d<T>(T(0), T(0)))
    , k_(k)
    , p1_(p1)
    , p2_(p2)
  {}

  vpgl_radial_tangential_distortion(const vgl_point_2d<T> & center, std::vector<T> k, T p1, T p2)
    : center_(center)
    , k_(k)
    , p1_(p1)
    , p2_(p2)
  {}

  //: Distort a projected point on the image plane
  //  Calls the pure virtual radial distortion function
  vgl_homg_point_2d<T>
  distort(const vgl_homg_point_2d<T> & point) const override;

  //: Return the original point that was distorted to this location (inverse of distort)
  // \param init is an initial guess at the solution for the iterative solver
  // if \p init is NULL then \p point is used as the initial guess
  // calls the radial undistortion function
  vgl_homg_point_2d<T>
  undistort(const vgl_homg_point_2d<T> & point, const vgl_homg_point_2d<T> * init = nullptr) const override;

  //: implementation of distortion in the pixel coordinate system
  vgl_homg_point_2d<T>
  distort_pixel(const vgl_homg_point_2d<T> & pixel, const vpgl_calibration_matrix<T> & K) const override
  {
    vgl_homg_point_2d<T> cen_pixel(pixel.x() - center_.x(), pixel.y() - center_.y());
    vgl_homg_point_2d<T> ret = vpgl_lens_distortion<T>::distort_pixel(cen_pixel, K);
    return ret;
  }

  //: implementation of undistortion in the pixel coordinate system
  vgl_homg_point_2d<T>
  undistort_pixel(const vgl_homg_point_2d<T> & distorted_pixel, const vpgl_calibration_matrix<T> & K) const override
  {
    vgl_homg_point_2d<T> ret = vpgl_lens_distortion<T>::undistort_pixel(distorted_pixel, K);
    ret.set(ret.x() + center_.x(), ret.y() + center_.y());
    return ret;
  }

  //: do nothing
  //: Set a translation to apply before of after distortion
  // This is needed when distorting an image to translate the resulting image
  // such that all points have positive indices
  void
  set_translation(const vgl_vector_2d<T> & offset, bool /*after*/ = true) override
  {
    center_ += offset;
  }

protected:
  vgl_vector_2d<T>
  apply_distortion(const vgl_vector_2d<T> & x) const;
  //: The center of distortion
  vgl_point_2d<T> center_;

  //: the k coefficients (radial distortion)
  std::vector<T> k_;

  //: the p1, p2 coefficients (tangential distortion)
  T p1_;
  T p2_;
};

#endif // vpgl_radial_tangential_distortion_h_
