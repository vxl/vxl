#ifndef bpgl_3d_from_disparity_h_
#define bpgl_3d_from_disparity_h_

#include <vil/vil_image_view.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>


/**
 * Given two cameras corresponding to left, right images of a stereo pair, and
 * a disparity map, return per-pixel 3D points in the form of a 3-plane image.
 * It is assumed that the images are rectified such that img1(i,j) <--> img2(i + disparity(i,j), j)
 * for example, given the pixel location (100, 200) in image1 and a disparity value of -20 at that location
 * in the disparity image then the corresponding pixel location in image2 is (80, 200).
 * valid camera types, CAM_T, are vpgl_affine_camera<T> and vpgl_perspective_camera<T>
 * For perspective cameras derived from structure from motion (SfM) it is possible that world points
 * are behind one or both of the cameras. In this case the sense of the disparity is reversed. That is,
 *             dZ
 *            ---- > 0
 *             dD
 * In this case, scene depth is inverted and to obtain correct scene depth, disparity sense(sign) must be reversed.
 * The required sign reveral is done interior to these functions
**/
template<typename T, typename CAM_T>
vil_image_view<T> bpgl_3d_from_disparity(
    CAM_T const& cam1,
    CAM_T const& cam2,
    vil_image_view<T> const& disparity);

template<typename T, typename CAM_T>
vil_image_view<T> bpgl_3d_from_disparity_with_scalar(
    CAM_T const& cam1,
    CAM_T const& cam2,
    vil_image_view<T> const& disparity,
    vil_image_view<T> const& scalar);

#endif
