// This is core/vil/tests/test_algo_fft.cxx
#include <testlib/testlib_test.h>
#include <vcl_complex.h>
#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_fft.h>

MAIN( test_algo_fft )
{
  START( "vil_algo_fft" );

  vil_image_view<vcl_complex<double> > img0 (4, 8, 2);

  // fill in image
  for (unsigned i=0; i<img0.ni(); i++)
  for (unsigned j=0; j<img0.nj(); j++)
  for (unsigned p=0; p<img0.nplanes(); p++)
      img0 (i, j, p) =
          vcl_complex<double> (111+i+10*j+100*p+0.1, 111+i+10*j+100*p+0.2);

  // copy image
  vil_image_view<vcl_complex<double> > img1;
  img1.deep_copy (img0);

  // FFT and inverse FFT
  vil_fft_2d_fwd (img1);
  vil_fft_2d_bwd (img1);

  // make sure original image is recovered
  double d = vil_math_ssd_complex (img0, img1, double());
  d = vcl_sqrt (d / (double) img0.size());
  TEST_NEAR ("FFT and inverse FFT recovers image", d, 0.0, 1e-5);

  // FFT of constant image
  vcl_complex<double> v = vcl_complex<double>(1.1, 2.2);
  img0.fill (v);
  vil_fft_2d_fwd (img0);
  TEST_NEAR("FFT coeff. 0,0 is mean", vcl_abs (v - img0(0,0,0)), 0.0, 1e-5);
  TEST_NEAR("FFT coeff. 0,1 is 0", vcl_abs (img0(0,1,0)), 0.0, 1e-5);
  TEST_NEAR("FFT coeff. 1,0 is 0", vcl_abs (img0(1,0,0)), 0.0, 1e-5);

  SUMMARY();
}
