// @author fsm
#include <vcl_iostream.h>

#include <vul/vul_sprintf.h>

#include <vxl_config.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vil1/vil1_image_as.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_convolve.h>

int main(int argc, char **argv)
{
  int const N=5;
  double kernel[2*N+1];
#if 0
  for (int i=-N; i<=N; ++i)
    kernel[i+N] = 0;
  kernel[N]                = 4;
  kernel[N-1]=kernel[N+1] = 2;
  kernel[N-2]=kernel[N+2] = 1;
#else
  for (int i=-N; i<=N; ++i)
    kernel[i+N] = 1/(1 + double(i*i)/N);
#endif
  { // normalize
    double sum = 0;
    for (int i=-N; i<=N; ++i)
      sum += kernel[i+N];
    for (int i=-N; i<=N; ++i)
      kernel[i+N] /= sum;
  }
  vcl_cerr << "kernel:\n";
  for (int i=-N; i<=N; ++i)
    vcl_cerr << kernel[i+N] << vcl_endl;

  vil1_convolve_boundary_option option[] = {
    vil1_convolve_no_extend,
    vil1_convolve_zero_extend,
    vil1_convolve_constant_extend,
    vil1_convolve_periodic_extend,
    vil1_convolve_reflect_extend,
    vil1_convolve_trim
  };

  for (int i=1; i<argc; ++i) {
    vcl_cerr << "loading image \'" << argv[i] << "\'\n";
    vil1_image I = vil1_load(argv[i]);
    if (!I) {
      vcl_cerr << "load failed\n";
      continue;
    }

    vil1_memory_image_of<vxl_byte> bytes( vil1_image_as_byte(I) );
    unsigned w = bytes.width();
    unsigned h = bytes.height();
    vil1_memory_image_of<double> tmp   (w, h);
    vil1_memory_image_of<float>  smooth(w, h);
    vil1_convolve_signal_1d<double const> K(kernel, 0, N, 2*N+1);
    vil1_memory_image_of<double> const& tmpc = tmp;
    vil1_memory_image_of<vxl_byte> const& bytesc = bytes;

    for (unsigned j=0; j<sizeof(option)/sizeof(option[0]); ++j) {
      vcl_cerr << "convolve x..." << vcl_flush;
      vil1_convolve_1d_x(K,
                         vil1_convolve_signal_2d<vxl_byte const>(bytesc.row_array(), 0, 0, w,  0, 0, h),
                         (double*)0,
                         vil1_convolve_signal_2d<double        >(tmp   .row_array(), 0, 0, w,  0, 0, h),
                         option[j], option[j]);
      vcl_cerr << "done\n"
               << "convolve y..." << vcl_flush;
      vil1_convolve_1d_y(K,
                         vil1_convolve_signal_2d<double const>(tmpc  .row_array(), 0, 0, w,  0, 0, h),
                         (double*)0,
                         vil1_convolve_signal_2d<float       >(smooth.row_array(), 0, 0, w,  0, 0, h),
                         option[j], option[j]);
      vcl_cerr << "done\n"
               << "save..." << vcl_flush;
      vil1_save(vil1_image_as_byte(smooth), vul_sprintf("%s.%d.pnm", argv[i], j).c_str(), "pnm");
      vcl_cerr << "done\n" << vcl_flush;
    }
  }
  return 0;
}
