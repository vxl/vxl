#ifndef osl_canny_smooth_txx_
#define osl_canny_smooth_txx_
//:
// \file
//
// Notes
// -  if this file is not in Templates, gcc 2.7 does not instantiate.
// -  even if this file is in Templates, SunPro 5.0 will fail to
//    instantiate symbols whose signatures contain a parameter of
//    the form "type * const *". hence the 'unpro_const' macro. fsm.
//
// \author fsm

#include "osl_canny_smooth.h"
#include <vcl_cmath.h>
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_memory_image_of.h>


template <class T>
inline float as_float(T const &v) { return float(v); }

VCL_DEFINE_SPECIALIZATION
inline float as_float(vil1_rgb<unsigned char> const &v) { return v.grey(); }

//--------------------------------------------------------------------------------

// the input image has xsize_ rows and ysize_ columns.
// the size of the convolution mask is k_size_ and we ignore a border of size
// width_ around the edge of the image.
//
// first we do
// tmp[x][y] = kernel_[0]*image_in[x-width_  ][y]
//           + kernel_[1]*image_in[x-width_+1][y]
//           + kernel_[2]*image_in[x-width_+2][y]
//           + ...
// and then we do
// smooth_[x][y] = kernel_[0]*tmp[x][y-width_  ]
//               + kernel_[1]*tmp[x][y-width_+1]
//               + kernel_[2]*tmp[x][y-width_+2]
//               + ...
template <class T>
void osl_canny_smooth_rothwell(T const * const *image_in, int xsize_, int ysize_,
                               float const *kernel_, int width_, int k_size_,
                               float * unpro_const *smooth_)
{
  // temporary buffer.
  vil1_memory_image_of<float> tmp(ysize_, xsize_);
  tmp.fill(0);

  // x direction
  for (int y=0; y<ysize_; ++y)
    for (int x=width_; x<xsize_-width_; ++x)
      for (int i=0,xx=x-width_; i<k_size_; i++,xx++)
        tmp[x][y] += as_float(image_in[xx][y]) * kernel_[i];

  // y direction
  for (int y=width_; y<ysize_-width_; ++y)
    for (int x=0; x<xsize_; ++x)
      for (int i=0,yy=y-width_; i<k_size_; i++,yy++)
        smooth_[x][y] += tmp[x][yy]*kernel_[i];
}

//
//: Computes the gradient images with the origin at (x0,y0) and of square size image_size.
//
template <class T>
void osl_canny_smooth_rothwell_adaptive(T const * const *image_, int /*xsize_*/, int /*ysize_*/,
                                        int x0, int y0, int image_size,
                                        float const *kernel_, int width_, int k_size_,
                                        float * unpro_const *dx,
                                        float * unpro_const *dy,
                                        float * unpro_const *grad)
{
  // Zero the derivative images
  for (int j=0; j<image_size; ++j)
    for (int i=0; i<image_size; ++i)  {
      dx[i][j] = 0.0;
      dy[i][j] = 0.0;
      grad[i][j] = 0.0;
    }

  // Do the different convolutions - we checked that we would not
  // go beyond the image bounds before calling Compute_adaptive_images

  // Compute the image gradients in the x direction
  for (int y=width_; y<image_size-width_; ++y)
    for (int x=width_; x<image_size-width_; ++x)
      for (int i=0,j=x-width_; i<k_size_; i++,j++)
        dx[x][y] += as_float(image_[j+x0][y+y0])*kernel_[i];

  // and in the y direction
  for (int x=width_; x<image_size-width_; ++x)
    for (int y=width_; y<image_size-width_; ++y)
      for (int i=0,j=y-width_; i<k_size_; i++,j++)
        dx[x][y] += as_float(image_[x+x0][j+y0])*kernel_[i];

  // and grad
  for (int x=width_; x<image_size-width_; ++x)
    for (int y=width_; y<image_size-width_; ++y)
      grad[x][y] = vcl_sqrt(dx[x][y]*dx[x][y] + dy[x][y]*dy[x][y]);
}


//--------------------------------------------------------------------------------

//
//: Convolves the image with the smoothing kernel.
//  sub_area_ is used to smooth pixels lying in a border of size width_.
//
//  Meaning of sub_area_[x] :
//    It is the probability that the variable will assume a value <= x
//    in the distribution function. It is the hashed area under the
//    distribution profile.
// \verbatim
//                     . .
//                   . ### .
//                 .  #####| .
//               . ########|   .
//             . ##########|     .
//         __ .############|      .__
//       __________________|___________
//                         x
// \endverbatim
//

#define thePixel(b,x,y) as_float(b[x][y])

template <class T>
void osl_canny_smooth(T const * const * image_in, int xsize_, int ysize_,
                      float const *kernel_, int width_, float const *sub_area_,
                      float * unpro_const * image_out)
{
  vil1_memory_image_of<float> tmp(ysize_, xsize_);
  tmp.fill(0);

  // x direction
  for (int y=0; y<ysize_; ++y) {
    // left border of size width_
    for (int x=0; x<width_-1; ++x) {
      tmp[x][y] = thePixel(image_in,x,y)*kernel_[0];
      for (int k=1; k < width_; ++k) {
        if (x-k < 0)
          tmp[x][y] += thePixel(image_in,x+k,y)*kernel_[k];
        else
          tmp[x][y] += thePixel(image_in,x-k,y)*kernel_[k] + thePixel(image_in,x+k,y)*kernel_[k];
      }
      tmp[x][y] /= sub_area_[x+1];
    }

    // Middle pixels along x direction
    for (int x=width_-1; x<xsize_-width_+1; ++x) {
      tmp[x][y] = thePixel(image_in,x,y)*kernel_[0];
      for (int k=1; k < width_; ++k)
        tmp[x][y] += thePixel(image_in,x-k,y)*kernel_[k] + thePixel(image_in,x+k,y)*kernel_[k];
    }

    // Right border of size width_
    for (int x=xsize_-width_+1; x < xsize_; ++x) {
      tmp[x][y] = thePixel(image_in,x,y)*kernel_[0];
      for (int k=1; k < width_; ++k) {
        if (x+k >= xsize_)
          tmp[x][y] += thePixel(image_in,x-k,y)*kernel_[k];
        else
          tmp[x][y] += thePixel(image_in,x-k,y)*kernel_[k] + thePixel(image_in,x+k,y)*kernel_[k];
      }
      tmp[x][y] /= sub_area_[xsize_-x];
    }
  }


  // y direction
  for (int x=0; x < xsize_; ++x) {
    // Top border of size width_
    for (int y=0; y < width_-1; ++y) {
      image_out[x][y] = tmp[x][y]*kernel_[0];
      for (int k=1; k < width_; ++k) {
        if (y-k < 0)
          image_out[x][y] += tmp[x][y+k]*kernel_[k];
        else
          image_out[x][y] += tmp[x][y-k]*kernel_[k] + tmp[x][y+k]*kernel_[k];
      }
      image_out[x][y] /= sub_area_[y+1];
    }

    // Middle pixels along y direction
    for (int y=width_-1; y < ysize_-width_+1; ++y) {
      image_out[x][y] = tmp[x][y]*kernel_[0];
      for (int k=1; k < width_; ++k) {
        image_out[x][y] += tmp[x][y-k]*kernel_[k] + tmp[x][y+k]*kernel_[k];
      }
    }

    // Bottom border of size width_
    for (int y=ysize_-width_+1; y < ysize_; ++y) {
      image_out[x][y] = tmp[x][y]*kernel_[0];
      for (int k=1; k < width_; ++k) {
        if (y+k >= ysize_)
          image_out[x][y] += tmp[x][y-k]*kernel_[k];
        else
          image_out[x][y] += tmp[x][y-k]*kernel_[k] + tmp[x][y+k]*kernel_[k];
      }
      image_out[x][y] /= sub_area_[ysize_-y];
    }
  }
}

//--------------------------------------------------------------------------------

#define OSL_CANNY_SMOOTH_INSTANTIATE(T) \
template void osl_canny_smooth_rothwell(T const * const *image_in, int xsize_, int ysize_, \
                                        float const *kernel_, int width_, int k_size_, \
                                        float * unpro_const *smooth_); \
template void osl_canny_smooth_rothwell_adaptive(T const * const *in, int xsize_, int ysize_, \
                                                 int x0, int y0, int image_size,  \
                                                 float const *kernel_, int width_, int k_size_, \
                                                 float * unpro_const *dx, float * unpro_const *dy, float * unpro_const *grad); \
template void osl_canny_smooth(T const * const *image_in, int xsize_, int ysize_, \
                               float const *kernel_, int width_, float const *sub_area_, \
                               float * unpro_const * image_out)
//VCL_INSTANTIATE_INLINE(float as_float(T const &));

#endif // osl_canny_smooth_txx_
