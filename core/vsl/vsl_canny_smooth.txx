/*
  fsm@robots.ox.ac.uk
*/

// Notes
// 1. if this file is not in Templates, gcc 2.7 does not instantiate.
// 2. even if this file is in Templates, SunPro 5.0 will fail to 
//    instantiate symbols whose signatures contain a parameter of
//    the form "type * const *". hence the 'unpro_const' macro. fsm.

#include "vsl_canny_smooth.h"
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_cmath.h>
#include <vcl/vcl_iostream.h>
#include <vil/vil_rgb_byte.h>


template <class T>
inline float as_float(T const &v) { return float(v); }

VCL_DEFINE_SPECIALIZATION
inline float as_float(vil_rgb_byte const &v) { return 0.299*v.r + 0.587*v.g + 0.114*v.b; }

//--------------------------------------------------------------------------------

// the input image has _xsize rows and _ysize columns.
// the size of the convolution mask is _k_size and we ignore a border of size
// _width around the edge of the image.
//
// first we do
// tmp[x][y] = _kernel[0]*image_in[x-_width  ][y] 
//           + _kernel[1]*image_in[x-_width+1][y]
//           + _kernel[2]*image_in[x-_width+2][y]
//           + ...
// and then we do
// _smooth[x][y] = _kernel[0]*tmp[x][y-_width  ]
//               + _kernel[1]*tmp[x][y-_width+1]
//               + _kernel[2]*tmp[x][y-_width+2]
//               + ...
template <class T>
void vsl_canny_smooth_rothwell(T const * const *image_in, int _xsize, int _ysize, 
			       float const *_kernel, int _width, int _k_size,
			       float * unpro_const *_smooth)
{
  // temporary buffer.
  vil_memory_image_of<float> tmp(_ysize, _xsize);
  tmp.fill(0);
  
  // x direction
  for (int y=0; y<_ysize; ++y) 
    for (int x=_width; x<_xsize-_width; ++x) 
      for (int i=0,xx=x-_width; i<_k_size; i++,xx++) 
	tmp[x][y] += as_float(image_in[xx][y]) * _kernel[i];
  
  // y direction
  for (int y=_width; y<_ysize-_width; ++y) 
    for (int x=0; x<_xsize; ++x) 
      for (int i=0,yy=y-_width; i<_k_size; i++,yy++) 
	_smooth[x][y] += tmp[x][yy]*_kernel[i];
}


//
// -- Computes the gradient images with the origin at (x0,y0) and of square
// size image_size.
//
template <class T>
void vsl_canny_smooth_rothwell_adaptive(T const * const *_image, int /*_xsize*/, int /*_ysize*/, 
					int x0, int y0, int image_size, 
					float const *_kernel, int _width, int _k_size,
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
  for (int y=_width; y<image_size-_width; ++y) 
    for (int x=_width; x<image_size-_width; ++x) 
      for (int i=0,j=x-_width; i<_k_size; i++,j++) 
	dx[x][y] += as_float(_image[j+x0][y+y0])*_kernel[i];
  
  // and in the y direction
  for (int x=_width; x<image_size-_width; ++x) 
    for (int y=_width; y<image_size-_width; ++y) 
      for (int i=0,j=y-_width; i<_k_size; i++,j++) 
	dx[x][y] += as_float(_image[x+x0][j+y0])*_kernel[i];
  
  // and grad
  for (int x=_width; x<image_size-_width; ++x) 
    for (int y=_width; y<image_size-_width; ++y) 
      grad[x][y] = sqrt(dx[x][y]*dx[x][y] + dy[x][y]*dy[x][y]);
}


//--------------------------------------------------------------------------------

//
// -- Convolves the image with the smoothing kernel.
//  _sub_area is used to smooth pixels lying in a border of size _width.
//
//  Meaning of _sub_area[x] :
//    It is the probability that the variable will assume a value <= x
//    in the distribution function. It is the hashed area under the 
//    distribution profile.
// @{
// \begin{verbatim}
//                     . .
//                   . ### .
//                 .  #####| .
//               . ########|   .
//             . ##########|     .
//         __ .############|      .__
//       __________________|___________
//                         x
// \end{verbatim}
// @}
//

#define thePixel(b,x,y) as_float(b[x][y])

template <class T>
void vsl_canny_smooth(T const * const * image_in, int _xsize, int _ysize, 
		      float const *_kernel, int _width, float const *_sub_area,
		      float * unpro_const * image_out) 
{
  vil_memory_image_of<float> tmp(_ysize, _xsize);
  tmp.fill(0);
  
  // x direction
  for (int y=0; y<_ysize; ++y) {
    // left border of size _width
    for (int x=0; x<_width-1; ++x) {
      tmp[x][y] = thePixel(image_in,x,y)*_kernel[0];
      for (int k=1; k < _width; ++k) {
	if (x-k < 0)
	  tmp[x][y] += thePixel(image_in,x+k,y)*_kernel[k];
	else
	  tmp[x][y] += thePixel(image_in,x-k,y)*_kernel[k] + thePixel(image_in,x+k,y)*_kernel[k];
      }
      tmp[x][y] /= _sub_area[x+1];
    }

    // Middle pixels along x direction
    for (int x=_width-1; x<_xsize-_width+1; ++x) {
      tmp[x][y] = thePixel(image_in,x,y)*_kernel[0];
      for (int k=1; k < _width; ++k) 
	tmp[x][y] += thePixel(image_in,x-k,y)*_kernel[k] + thePixel(image_in,x+k,y)*_kernel[k];
    }

    // Right border of size _width
    for (int x=_xsize-_width+1; x < _xsize; ++x) {
      tmp[x][y] = thePixel(image_in,x,y)*_kernel[0];
      for (int k=1; k < _width; ++k) {
	if (x+k >= _xsize)
	  tmp[x][y] += thePixel(image_in,x-k,y)*_kernel[k];
	else
	  tmp[x][y] += thePixel(image_in,x-k,y)*_kernel[k] + thePixel(image_in,x+k,y)*_kernel[k];
      }
      tmp[x][y] /= _sub_area[_xsize-x];
    }
  }



  // y direction
  for (int x=0; x < _xsize; ++x) {
    // Top border of size _width
    for (int y=0; y < _width-1; ++y) {
      image_out[x][y] = tmp[x][y]*_kernel[0];
      for (int k=1; k < _width; ++k) {
	if (y-k < 0)
	  image_out[x][y] += tmp[x][y+k]*_kernel[k];
	else
	  image_out[x][y] += tmp[x][y-k]*_kernel[k] + tmp[x][y+k]*_kernel[k];
      }
      image_out[x][y] /= _sub_area[y+1];
    }

    // Middle pixels along y direction
    for (int y=_width-1; y < _ysize-_width+1; ++y) {
      image_out[x][y] = tmp[x][y]*_kernel[0];
      for (int k=1; k < _width; ++k) {
	image_out[x][y] += tmp[x][y-k]*_kernel[k] + tmp[x][y+k]*_kernel[k];
      }
    }

    // Bottom border of size _width
    for (int y=_ysize-_width+1; y < _ysize; ++y) {
      image_out[x][y] = tmp[x][y]*_kernel[0];
      for (int k=1; k < _width; ++k) {
	if (y+k >= _ysize)
	  image_out[x][y] += tmp[x][y-k]*_kernel[k];
	else
	  image_out[x][y] += tmp[x][y-k]*_kernel[k] + tmp[x][y+k]*_kernel[k];
      }
      image_out[x][y] /= _sub_area[_ysize-y];
    }
  }
}

//--------------------------------------------------------------------------------

#define VSL_CANNY_SMOOTH_INSTANTIATE(T) \
VCL_INSTANTIATE_INLINE(float as_float(T const &)); \
template void vsl_canny_smooth_rothwell(T const * const *image_in, int _xsize, int _ysize, \
					float const *_kernel, int _width, int _k_size, \
					float * unpro_const *_smooth); \
template void vsl_canny_smooth_rothwell_adaptive(T const * const *in, int _xsize, int _ysize, \
						 int x0, int y0, int image_size,  \
						 float const *_kernel, int _width, int _k_size, \
						 float * unpro_const *dx, float * unpro_const *dy, float * unpro_const *grad); \
template void vsl_canny_smooth(T const * const *image_in, int _xsize, int _ysize, \
			       float const *_kernel, int _width, float const *_sub_area, \
			       float * unpro_const * image_out);
