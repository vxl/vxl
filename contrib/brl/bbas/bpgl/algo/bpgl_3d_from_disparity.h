#ifndef bpgl_3d_from_disparity_h_
#define bpgl_3d_from_disparity_h_

#include <vil/vil_image_view.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>


/**
 * Given two affine cameras corresponding to left, right images of a stereo pair, and
 * a disparity map, return per-pixel 3D points in the form of a 3-plane image.
 * It is assumed that the images are rectified such that img1(i,j) <--> img2(i - disparity(i,j), j)
**/
template<typename DISP_T>
vil_image_view<float> bpgl_3d_from_disparity(vpgl_affine_camera<double> const& cam1,
                                              vpgl_affine_camera<double> const& cam2,
                                              vil_image_view<DISP_T> const& disparity);


#endif
