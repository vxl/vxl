// This is mul/mil/tests/test_normalise_image_2d.cxx
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <mil/mil_normalise_image_2d.h>
#include <testlib/testlib_test.h>

void test_normalise_image_2d()
{
  vcl_cout << "********************************\n";
  vcl_cout << " Testing mil_normalise_image_2d\n";
  vcl_cout << "********************************\n";

  // test grey conversions

  int nx=5;
  int ny=5;
  mil_image_2d_of<float> orig_image(nx,ny),
                         mean_norm_image,
                         correct_mean_norm_image(nx,ny),
                         var_norm_image,
                         correct_var_norm_image(nx,ny);

  // Create original image
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      orig_image(x,y)=1.25f*x;

  orig_image.print_summary(vcl_cout);
  orig_image.print_all(vcl_cout);

  // test mean normalisation
  mil_mean_norm_image_2d(mean_norm_image, orig_image);

  mean_norm_image.print_summary(vcl_cout);
  mean_norm_image.print_messy_all(vcl_cout);

  // create correct mean norm image
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      correct_mean_norm_image(x,y)=1.25f*x-2.5f;

  // Calc Total difference over all pixels
  double diff1=0;
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      diff1+=vcl_fabs( mean_norm_image(x,y)-
                       correct_mean_norm_image(x,y) );

  TEST_NEAR("test mean normalisation",diff1,0,1e-6);

  // test variance normalisation
  mil_var_norm_image_2d(var_norm_image, orig_image);

  var_norm_image.print_summary(vcl_cout);
  var_norm_image.print_messy_all(vcl_cout);

  // create correct variance norm image
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      correct_var_norm_image(x,y)=(x-2.f)/ (float)vcl_sqrt(2.0);

  double diff2=0;
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      diff2+=vcl_fabs( var_norm_image(x,y)-
                       correct_var_norm_image(x,y) );

  TEST_NEAR("test variance normalisation",diff2,0,1e-6);
}

TESTLIB_DEFINE_MAIN(test_normalise_image_2d);
