/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vil_smooth.h"
#endif
#include "vil_smooth.h"

#include <vcl/vcl_iostream.h>

#include <vil/vil_memory_image_of.h>
#include <vil/vil_image_as.h>
#include <vil/vil_convolve.h>

vil_image vil_smooth(float const kernel[], unsigned N,
		     vil_image const &inp)
{
  vil_convolve_signal_1d<float const> K(kernel, 0, N/2, N);

  unsigned w = inp.width();
  unsigned h = inp.height();

  // convert to byte image. 
  vil_memory_image_of<vil_byte> buf(vil_image_as_byte(inp));
  
  //
  vil_memory_image_of<float> tmp(w, h);
  
  cerr << "convolve x..." << flush;
  vil_convolve_1d_x(K,
		    vil_convolve_signal_2d<vil_byte const>(buf.row_array(), 0, 0, w,  0, 0, h),
		    (float*)0,
		    vil_convolve_signal_2d<float         >(tmp.row_array(), 0, 0, w,  0, 0, h),
		    vil_convolve_trim, vil_convolve_trim);
  cerr << "done" << endl;

  //
  vil_memory_image_of<float> out(w, h);

  cerr << "convolve y..." << flush;
  vil_convolve_1d_y(K,
		    vil_convolve_signal_2d<float const>(tmp.row_array(), 0, 0, w,  0, 0, h),
		    (float*)0,
		    vil_convolve_signal_2d<float      >(out.row_array(), 0, 0, w,  0, 0, h),
		    vil_convolve_trim, vil_convolve_trim);
  cerr << "done" << endl;

  return out;
}
