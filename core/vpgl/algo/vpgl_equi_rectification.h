// This is core/vpgl/algo/vpgl_equi_rectification.h
#ifndef vpgl_equi_rectification_h_
#define vpgl_equi_rectification_h_
//:
// \file
// \brief Rectify a pair of images by applying equal and opposite warp transforms to each image
// \author J.L. Mundy
// \date February 10, 2019
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
// Equi-affine rectification applies the required warp equally to each image, rather than applying
// the required row and column scaling solely to image1.  As a first step, rotations are applied
// to each image, R0 and R1 so that the epipolar lines of aF are horizontal, i.e.,parallel to image rows.
// The image row coordinates are then scaled so that rows are in correspondence. The transform is as follows.
//       _           _
//      |  1   0   0  |
//  V = |  0   sv  tv |
//      |  0   0   1  |
//       -           -
// The values of sv and tv are found by minimizing the square distance between the transformed image1 points
// and the corrsponding points in image0. That is ||V*R1[X1]- R0[X0]||^2 is minimized.
// To determine the equal and opposite transformations it is necessary to define the square root of the
// transformation matrix.
//
//                                    _                      _
//                                   |  1    0         0      |
//                                   |                tv      |
//   V = V^1/2 * V^1/2, where V^1/2 =|  0  sv^1/2  ---------- |
//                                   |            (1 + sv^1/2)|
//                                   |  0   0          1      |
//                                    -                      -
// The transformation is then equalized for each image, i.e. V^1/2 * R1  and (V^1/2)^-1 * R0.
// A similar proceedure is applied to scale the column coordinates, so that overall disparity
// is minimized.
//
// The approach to rectification for perspective cameras follows that of
// Fusiello, Andrea. (1999). Tutorial on Rectification of Stereo Images.
// The algorithm proceeds in three steps:
//
// 1) find the rotation about the camera center that takes the "left" camera (P1) rotation to the "right" camera (P0)
// rotation
//    R10 = R0 * R1^t
// where P0 = K0[R0 | t0], P1 = K1[R1 | t1]. Here, "right" means on the right side of the essential matrix
//
// 2) Define a rotation Rr that takes epipole_0 to infinity, i.e. epipole_0 -> (1, 0, 0)^t
//
// 3) Compute ofsets and scale homographies S0, S1
//
// The final homographies are then
// H0 = K0 * S0 * Rr * K0^-1
// H1 = K1 * S1 * Rr * R10* K1^-1
//
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_affine_fundamental_matrix.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

class vpgl_equi_rectification
{
public:
  //: given the affine fundamental matrix and a set of correspondences between
  // the two images, image0 and image1, pts0, pts1, determine the corresponding
  // affine homographies, H0 and H1, that warp each image into a rectified pair.
  // A rectified pair has rows in correspondence,i.e., a point in one image has
  // its corresponding point on the same row in the other image. Equivalently,
  // an epipolar line in image 1 is the same row as for a given point in image 0.
  // min_scale limits the amount of warping to avoid aliasing
  static bool
  rectify_pair(const vpgl_affine_fundamental_matrix<double> & aF,
               const std::vector<vnl_vector_fixed<double, 3>> & img_pts0,
               const std::vector<vnl_vector_fixed<double, 3>> & img_pts1,
               vnl_matrix_fixed<double, 3, 3> & H0,
               vnl_matrix_fixed<double, 3, 3> & H1,
               double min_scale = 0.1);

  //: for convenience, from cameras, but only the fundamental matrix is used in the rectification
  static bool
  rectify_pair(const vpgl_affine_camera<double> & A0,
               const vpgl_affine_camera<double> & A1,
               const std::vector<vnl_vector_fixed<double, 3>> & img_pts0,
               const std::vector<vnl_vector_fixed<double, 3>> & img_pts1,
               vnl_matrix_fixed<double, 3, 3> & H0,
               vnl_matrix_fixed<double, 3, 3> & H1,
               double min_scale = 0.1)
  {
    vpgl_affine_fundamental_matrix<double> aF(A0, A1);
    return rectify_pair(aF, img_pts0, img_pts1, H0, H1, min_scale);
  }

  //: the rectification of images perspective cameras
  //  epipoles are defined by the essential matrixof the two cameras
  //  the point sets are correspondences between the two images and are used to define
  //  the column skew transform and row offset
  static bool
  rectify_pair(const vpgl_perspective_camera<double> & P0,
               const vpgl_perspective_camera<double> & P1,
               const std::vector<vnl_vector_fixed<double, 3>> & img_pts0,
               const std::vector<vnl_vector_fixed<double, 3>> & img_pts1,
               vnl_matrix_fixed<double, 3, 3> & H0,
               vnl_matrix_fixed<double, 3, 3> & H1);

private:
  //: affine column transformation with skew used by both rectification methods
  static bool
  column_transform(const std::vector<vnl_vector_fixed<double, 3>> & img_pts0,
                   const std::vector<vnl_vector_fixed<double, 3>> & img_pts1,
                   const vnl_matrix_fixed<double, 3, 3> & H0,
                   const vnl_matrix_fixed<double, 3, 3> & H1,
                   vnl_matrix_fixed<double, 3, 3> & Usqt,
                   vnl_matrix_fixed<double, 3, 3> & Usqt_inv,
                   double min_scale = 0.5);

  //: no public constructor - static methods only
  vpgl_equi_rectification() = delete;
};

#endif // vpgl_equi_rectification_h_
