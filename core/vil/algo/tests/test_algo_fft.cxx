// This is core/vil/algo/tests/test_algo_fft.cxx
#include <testlib/testlib_test.h>
#include <vcl_complex.h>
#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_fft.h>
#include <vcl_ctime.h> // seed for random number generator

static void test_algo_fft()
{
  vil_image_view<vcl_complex<double> > img0(4, 8, 2);
  unsigned int seed = (unsigned int)vcl_time(0);

  // fill in image
  for (unsigned i=0; i<img0.ni(); i++)
    for (unsigned j=0; j<img0.nj(); j++)
      for (unsigned p=0; p<img0.nplanes(); ++p, seed*=16807)
        img0(i,j,p) = vcl_complex<double>(-1e-5*seed+111.1, -1e-5*seed+222.2);

  // copy image
  vil_image_view<vcl_complex<double> > img1;
  img1.deep_copy(img0);

  // FFT and inverse FFT
  vil_fft_2d_fwd(img1);
  vil_fft_2d_bwd(img1);

  // make sure original image is recovered
  double d = vil_math_ssd_complex(img0, img1, double());
  d /= (double) img0.size();
  TEST_NEAR("FFT and inverse FFT recovers image", d, 0.0, 1e-9);

  // FFT of constant image
  vcl_complex<double> v = vcl_complex<double>(1.1, 2.2);
  img0.fill(v);
  vil_fft_2d_fwd(img0);
  TEST_NEAR("FFT coeff. 0,0 is mean", img0(0,0,0), v, 1e-9);
  TEST_NEAR("FFT coeff. 0,0 is mean", img0(0,0,1), v, 1e-9);
  // select just any pixel component (i,j,p) different from (0,0,*):
  {
    unsigned int i = seed%img0.ni(); seed *= 16807;
    unsigned int j = seed%img0.nj(); seed *= 16807;
    unsigned int p = seed%img0.nplanes();
    if (i==0 && j==0) i=1;
    TEST_NEAR("any other FFT coeff. is 0", img0(i,j,p), 0.0, 1e-9);
  }
}

TESTMAIN(test_algo_fft);
