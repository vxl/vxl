/*
  fsm
*/
#include <vcl_iostream.h>

#include <vul/vul_sprintf.h>

#include <vil/vil_byte.h>
#include <vil/vil_image.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_as.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_convolve.h>

int main(int argc, char **argv) {
  int const N=5;
  float kernel[2*N+1];
#if 0
  for (int i=-N; i<=N; ++i)
    kernel[i+N] = 0;
  kernel[N]                = 4;
  kernel[N-1]=kernel[N+1] = 2;
  kernel[N-2]=kernel[N+2] = 1;
#else
  for (int i=-N; i<=N; ++i)
    kernel[i+N] = 1/(1 + float(i*i)/N);
#endif
  { // normalize
    float sum = 0;
    for (int i=-N; i<=N; ++i)
      sum += kernel[i+N];
    for (int i=-N; i<=N; ++i)
      kernel[i+N] /= sum;
  }
  vcl_cerr << "kernel:" << vcl_endl;
  for (int i=-N; i<=N; ++i)
    vcl_cerr << kernel[i+N] << vcl_endl;

  vil_convolve_boundary_option option[] = {
    vil_convolve_no_extend,
    vil_convolve_zero_extend,
    vil_convolve_constant_extend,
    vil_convolve_periodic_extend,
    vil_convolve_reflect_extend,
    vil_convolve_trim
  };

  for (int i=1; i<argc; ++i) {
    vcl_cerr << "loading image \'" << argv[i] << "\'" << vcl_endl;
    vil_image I = vil_load(argv[i]);
    if (!I) {
      vcl_cerr << "load failed" << vcl_endl;
      continue;
    }

    vil_memory_image_of<vil_byte> bytes( vil_image_as_byte(I) );
    unsigned w = bytes.width();
    unsigned h = bytes.height();
    vil_memory_image_of<float> tmp   (w, h);
    vil_memory_image_of<float> smooth(w, h);
    vil_convolve_signal_1d<float const> K(kernel, 0, N, 2*N+1);

    for (unsigned j=0; j<sizeof(option)/sizeof(option[0]); ++j) {
      vcl_cerr << "convolve x..." << vcl_flush;
      vil_convolve_1d_x(K,
                        vil_convolve_signal_2d<vil_byte const>(bytes.row_array(), 0, 0, w,  0, 0, h),
                        (float*)0,
                        vil_convolve_signal_2d<float         >(tmp  .row_array(), 0, 0, w,  0, 0, h),
                        option[j], option[j]);
      vcl_cerr << "done" << vcl_endl;

      vcl_cerr << "convolve y..." << vcl_flush;
      vil_convolve_1d_y(K,
                        vil_convolve_signal_2d<float const>(tmp   .row_array(), 0, 0, w,  0, 0, h),
                        (float*)0,
                        vil_convolve_signal_2d<float      >(smooth.row_array(), 0, 0, w,  0, 0, h),
                        option[j], option[j]);
      vcl_cerr << "done" << vcl_endl;

      vcl_cerr << "save..." << vcl_flush;
      vil_save(vil_image_as_byte(smooth), vul_sprintf("%s.%d.pnm", argv[i], j).c_str(), "pnm");
      vcl_cerr << "done" << vcl_endl;
    }
  }
  return 0;
}
