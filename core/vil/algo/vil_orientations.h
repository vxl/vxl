#ifndef vil_orientations_h_
#define vil_orientations_h_
//:
// \file
// \brief Functions to compute orientations and gradient magnitude
// \author Tim Cootes

#include <vil/vil_image_view.h>
#include <vxl_config.h>
#include <vil/algo/vil_sobel_3x3.h>

//: Compute orientation (in radians) and gradient magnitude at each pixel
//  Images assumed to be single plane
// \relatesalso vil_image_view
void
vil_orientations(const vil_image_view<float> & grad_i,
                 const vil_image_view<float> & grad_j,
                 vil_image_view<float> & orient_im,
                 vil_image_view<float> & grad_mag);

//: Compute discrete orientation and gradient magnitude at each pixel
//  Computes orientation at each pixel and scales to range [0,n_orientations-1].
//
//  Orientation of i corresponds to angles in range [(i-0.5)dA,(i+0.5)dA]
//  where dA=2*pi/n_orientations.
//
//  Images assumed to be single plane
// \relatesalso vil_image_view
void
vil_orientations(const vil_image_view<float> & grad_i,
                 const vil_image_view<float> & grad_j,
                 vil_image_view<vxl_byte> & orient_im,
                 vil_image_view<float> & grad_mag,
                 unsigned n_orientations = 256);

//: Compute discrete orientation and gradient magnitude at edge pixels
//  Computes orientation at each pixel and scales to range [0,n_orientations].
//  If gradient magnitude is less than grad_threshold, then orientation
//  of zero is set, meaning undefined orientation.
//
//  Orientation of i>0 corresponds to angles in range [(i-1.5)dA,(i-0.5)dA]
//  where dA=2*pi/n_orientations.
//
//  Images assumed to be single plane
// \relatesalso vil_image_view
void
vil_orientations_at_edges(const vil_image_view<float> & grad_i,
                          const vil_image_view<float> & grad_j,
                          vil_image_view<vxl_byte> & orient_im,
                          vil_image_view<float> & grad_mag,
                          float grad_threshold,
                          unsigned n_orientations = 255);

//: Compute orientation and gradient magnitude using sobel to get gradients
// \relatesalso vil_image_view
template <class T>
inline void
vil_orientations_from_sobel(const vil_image_view<T> & src_image,
                            vil_image_view<float> & orient_im,
                            vil_image_view<float> & grad_mag)
{
  vil_image_view<float> grad_i, grad_j;
  vil_sobel_3x3(src_image, grad_i, grad_j);
  vil_orientations(grad_i, grad_j, orient_im, grad_mag);
}

//: Compute discrete orientation and gradient using sobel operations
//  Computes orientation at each pixel and scales to range [0,n_orientations-1].
//
//  Orientation of i corresponds to angles in range [(i-0.5)dA,(i+0.5)dA]
//  where dA=2*pi/n_orientations.
//
//  Images assumed to be single plane
// \relatesalso vil_image_view
template <class T>
inline void
vil_orientations_from_sobel(const vil_image_view<T> & src_image,
                            vil_image_view<vxl_byte> & orient_im,
                            vil_image_view<float> & grad_mag,
                            unsigned n_orientations = 256)
{
  vil_image_view<float> grad_i, grad_j;
  vil_sobel_3x3(src_image, grad_i, grad_j);
  vil_orientations(grad_i, grad_j, orient_im, grad_mag, n_orientations);
}

#endif // vil_orientations_h_
