/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_smooth.h"

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cmath.h>
#include <vcl/vcl_vector.h>

#include <vil/vil_memory_image_of.h>
#include <vil/vil_image_as.h>
#include <vil/vil_convolve.h>
#include <vil/vil_rgb.h>
#include <vil/vil_new.h>
#include <vil/vil_buffer.h>

template <class pixel_type, class float_type>
void
vil_convolve_separable_symmetric(float const kernel[], unsigned N,
				 vil_memory_image_of<pixel_type> & buf,
				 vil_memory_image_of<float_type>& tmp,
				 vil_memory_image_of<float_type>& out
				 )
{
  vil_convolve_signal_1d<float const> K(kernel, 0, N/2, N);

  unsigned w = buf.width();
  unsigned h = buf.height();
    
  cerr << "convolve x..." << flush;
  vil_convolve_1d_x(K,
		    vil_convolve_signal_2d<pixel_type const>(buf.row_array(), 0, 0, w,  0, 0, h),
		    (float_type*)0,
		    vil_convolve_signal_2d<float_type         >(tmp.row_array(), 0, 0, w,  0, 0, h),
		    vil_convolve_trim, vil_convolve_trim);
  cerr << "done" << endl;

  cerr << "convolve y..." << flush;
  vil_convolve_1d_y(K,
		    vil_convolve_signal_2d<float_type const>(tmp.row_array(), 0, 0, w,  0, 0, h),
		    (float_type*)0,
		    vil_convolve_signal_2d<float_type      >(out.row_array(), 0, 0, w,  0, 0, h),
		    vil_convolve_trim, vil_convolve_trim);
  cerr << "done" << endl;
}

template <class pixel_type, class float_type>
vil_image vil_convolve_separable_symmetric(vil_image const & in, 
					   float const* kernel,
					   int N,
					   pixel_type*,
					   float_type*
					   )
{
  // Copy input image, unless it's already a memory image 
  // of the appropriate format.
  vil_memory_image_of<pixel_type> inbuf(in);

  vil_memory_image_of<float_type> tmp(in.width(), in.height());

  // Make the output image, on disk if necessary
  vil_image out = vil_new(in.width(), in.height(), in);

  // Make memory image for output, don't read the pixels.
  vil_memory_image_of<float_type> outbuf(out);

  // convolve
  vil_convolve_separable_symmetric(kernel, N, inbuf, tmp, outbuf);
  
  // Put outbuf into output image if necessary
  if (!out.get_property("memory"))
    out.put_section(outbuf.get_buffer(), 0,0, out.width(), out.height());
  
  return out;
}

vil_image vil_smooth_gaussian(vil_image const & in, double sigma)
{
  // Create 1-D mask:
  double cutoff = 0.01;
  double lc = -2 * vcl_log(cutoff); // cutoff guaranteed > 0
  int radius = (lc<=0) ? 0 : 1 + int(vcl_sqrt(lc)*sigma); // sigma guaranteed >= 0
  int size = radius + 1; // only need half mask, because it is symmetric
  vil_buffer<float> mask(size);
  double halfnorm = 0.5;
  mask[0] = 1.0;
  for (int x=1; x<=size; ++x) {
    mask[x] = vcl_exp(-0.5*x*x/(sigma*sigma)); 
    halfnorm += mask[x]; 
  }
  for (int x=0; x<=size; ++x)
    mask[x] /= 2*halfnorm; // normalise mask
  
  // Call convolver
  if (vil_pixel_type(in) == VIL_BYTE)
    return vil_convolve_separable_symmetric(in, mask.data(), size, (vil_byte*)0, (float*)0);

 // if (vil_pixel_type(in) == VIL_RGB_BYTE)
//    return vil_convolve_separable_symmetric(in, mask.begin(), size, (vil_rgb_byte*)0, (vil_rgb<float>*)0);
  
  return 0;
}

#define inst(pixel_type, float_type) \
template \
void vil_convolve_separable_symmetric(float const kernel[], unsigned N, \
				 vil_memory_image_of<pixel_type> & buf, \
				 vil_memory_image_of<float_type>& tmp, \
				 vil_memory_image_of<float_type>& out); \
template \
vil_image vil_convolve_separable_symmetric(vil_image const &, float const*, int, pixel_type*, float_type* )

inst(unsigned char, float);
inst(int, float);
