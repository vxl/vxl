#ifndef vil_convolve_txx_
#define vil_convolve_txx_
// This is vxl/vil/vil_convolve.txx

/*
  fsm@robots.ox.ac.uk
*/
#include "vil_convolve.h"
#include <vcl_iostream.h>

template <class pixel_type, class float_type>
void
vil_convolve_separable(float const kernel[], unsigned N,
                                 vil_memory_image_of<pixel_type>& buf,
                                 vil_memory_image_of<float_type>& tmp,
                                 vil_memory_image_of<float_type>& out
                                 )
{
  vil_convolve_signal_1d<float const> K(kernel, 0, N/2, N);

  unsigned w = buf.width();
  unsigned h = buf.height();

  vcl_cerr << "convolve x..." << vcl_flush;
  vil_convolve_1d_x(K,
                    vil_convolve_signal_2d<pixel_type const>(buf.row_array(), 0, 0, w,  0, 0, h),
                    (float_type*)0,
                    vil_convolve_signal_2d<float_type         >(tmp.row_array(), 0, 0, w,  0, 0, h),
                    vil_convolve_trim, vil_convolve_trim);
  vcl_cerr << "done" << vcl_endl;

  vcl_cerr << "convolve y..." << vcl_flush;
  vil_convolve_1d_y(K,
                    vil_convolve_signal_2d<float_type const>(tmp.row_array(), 0, 0, w,  0, 0, h),
                    (float_type*)0,
                    vil_convolve_signal_2d<float_type      >(out.row_array(), 0, 0, w,  0, 0, h),
                    vil_convolve_trim, vil_convolve_trim);
  vcl_cerr << "done" << vcl_endl;
}

template <class pixel_type, class float_type>
vil_image vil_convolve_separable(vil_image const & in,
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

#if 0
  // Make the output image, on disk if necessary
  vil_image out = vil_new(in.width(), in.height(), in);
#endif

  // Make memory image for output, don't read the pixels.
  vil_memory_image_of<float_type> outbuf(in.width(), in.height());

  // convolve
  vil_convolve_separable(kernel, N, inbuf, tmp, outbuf);

#if 0
  // Put outbuf into output image if necessary
  if (!out.get_property("memory"))
    out.put_section(outbuf.get_buffer(), 0,0, out.width(), out.height());
  return out;
#endif
   return outbuf;
}

//--------------------------------------------------------------------------------

#undef VIL_CONVOLVE_INSTANTIATE
#define VIL_CONVOLVE_INSTANTIATE(T)

#endif
