// This is core/vil1/vil1_convolve.txx
#ifndef vil1_convolve_txx_
#define vil1_convolve_txx_
// \author fsm
#include "vil1_convolve.h"
#include <vcl_iostream.h>

template <class I1, class I2, class AC, class O>
void vil1_convolve_separable(I1 const kernel[], unsigned N,
                             vil1_memory_image_of<I2>& buf,
                             vil1_memory_image_of<AC>& tmp,
                             vil1_memory_image_of<O>& out)
{
  vil1_convolve_signal_1d<I1 const> K(kernel, 0, N/2, N);
  vil1_memory_image_of<I2> const& bufc = buf;
  vil1_memory_image_of<AC> const& tmpc = tmp;

  unsigned w = buf.width();
  unsigned h = buf.height();

  vcl_cerr << "convolve x..." << vcl_flush;
  vil1_convolve_1d_x(K,
                     vil1_convolve_signal_2d<I2 const>(bufc.row_array(), 0, 0, w,  0, 0, h),
                     (AC*)0,
                     vil1_convolve_signal_2d<AC     >(tmp.row_array(), 0, 0, w,  0, 0, h),
                     vil1_convolve_trim, vil1_convolve_trim);
  vcl_cerr << "done\n"
           << "convolve y...";
  vil1_convolve_1d_y(K,
                     vil1_convolve_signal_2d<AC const>(tmpc.row_array(), 0, 0, w,  0, 0, h),
                     (AC*)0,
                     vil1_convolve_signal_2d<O       >(out.row_array(), 0, 0, w,  0, 0, h),
                     vil1_convolve_trim, vil1_convolve_trim);
  vcl_cerr << "done\n";
}

template <class I1, class I2, class AC, class O>
vil1_image vil1_convolve_separable(vil1_image const & in,
                                   I1 const* kernel,
                                   int N, I2*, AC*, O*)
{
  // Copy input image, unless it's already a memory image
  // of the appropriate format.
  vil1_memory_image_of<I2> inbuf(in);

  vil1_memory_image_of<AC> tmp(in.width(), in.height());

#if 0
  // Make the output image, on disk if necessary
  vil1_image out = vil1_new(in.width(), in.height(), in);
#endif

  // Make memory image for output, don't read the pixels.
  vil1_memory_image_of<O> outbuf(in.width(), in.height());

  // convolve
  vil1_convolve_separable(kernel, N, inbuf, tmp, outbuf);

#if 0
  // Put outbuf into output image if necessary
  if (!out.get_property("memory"))
    out.put_section(outbuf.get_buffer(), 0,0, out.width(), out.height());
  return out;
#endif
   return outbuf;
}

//--------------------------------------------------------------------------------

#undef VIL1_CONVOLVE_INSTANTIATE
#define VIL1_CONVOLVE_INSTANTIATE(T) \
template struct vil1_convolve_signal_1d<T >; \
template struct vil1_convolve_signal_2d<T >; \
template void vil1_convolve_separable(float const kernel[], unsigned N, \
                                      vil1_memory_image_of<T >& buf, \
                                      vil1_memory_image_of<double>& tmp, \
                                      vil1_memory_image_of<float>& out); \
template vil1_image vil1_convolve_separable(vil1_image const& in, \
                                            float const* kernel, \
                                            int N, T*, double*, float*)

#endif
