// This is mul/vil2/tests/test_image_view_maths.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/vil2_copy.h>
#include <vil2/vil2_math.h>
#include <vil2/vil2_print.h>

void test_image_view_maths_byte()
{
  vcl_cout << "*******************************\n"
           << " Testing vil2_image_view_maths\n"
           << "*******************************\n";

  int n=10, m=8;

  vil2_image_view<vxl_byte> imA(n,m,1);
  vil2_image_view<vxl_byte> imB(n,m,1);

  double sum2 = 0;
  for (unsigned int j=0;j<imA.nj();++j)
    for (unsigned int i=0;i<imA.ni();++i)
    {
      imA(i,j) = 1+i+j*n; sum2+= imA(i,j)*imA(i,j);
      imB(i,j) = 1+j+i*n;
    }

  double sum;
  vil2_math_sum(sum,imA,0);
  TEST_NEAR("Sum",sum,0.5*n*m*(n*m+1),1e-8);

  double mean;
  vil2_math_mean(mean,imA,0);
  TEST_NEAR("mean",mean,0.5*(n*m+1),1e-8);

  double sum_sq;
  vil2_math_sum_squares(sum,sum_sq,imA,0);
  TEST_NEAR("Sum of squares",sum_sq,sum2,1e-8);

  TEST_NEAR("Sum of squared differences", vil2_math_ssd(imA, imB, double()),93960.0, 1e-8);

  vil2_image_view<vxl_byte> imC = vil2_copy_deep(imA);
  vil2_math_scale_values(imC,2.0);
  TEST_NEAR("Values scaled",imC(3,5),imA(3,5)*2,1e-8);

  imC = vil2_copy_deep(imA);
  vil2_math_scale_and_offset_values(imC,2.0,7);
  TEST_NEAR("Values scaled+offset",imC(3,5),imA(3,5)*2+7,1e-8);

  vil2_image_view<float> im_sum,im_sum_sqr;
  vil2_math_image_sum(imA,imB,im_sum);
  TEST("Width of im_sum",im_sum.ni(),imA.ni());
  TEST("Height of im_sum",im_sum.nj(),imA.nj());
  TEST_NEAR("im_sum(5,7)",im_sum(5,7),float(imA(5,7))+float(imB(5,7)),1e-6);

  vil2_image_view<float> im_diff;
  vil2_math_image_difference(imA,imB,im_diff);
  TEST_NEAR("im_diff(5,2)",im_diff(5,2),float(imA(5,2))-float(imB(5,2)),1e-6);

  vil2_image_view<float> im_abs_diff;
  vil2_math_image_abs_difference(imA,im_sum,im_abs_diff);
  TEST_NEAR("im_abs_diff(3,7)",im_abs_diff(3,7),vcl_fabs(float(imA(3,7))-float(im_sum(3,7))),1e-6);

  float is45 = im_sum(4,5);
  vil2_math_add_image_fraction(im_sum,0.77,imA,0.23);
  TEST_NEAR("add_fraction",im_sum(4,5),0.77*is45+0.23*imA(4,5),1e-5);

  double sumA,sum_sqrA;
  vil2_math_sum_squares(sumA,sum_sqrA,imA,0);
  vil2_math_integral_image(imA,im_sum);
  TEST_NEAR("integral_image",im_sum(n-1,m-1),sumA,1e-6);

  vil2_math_integral_sqr_image(imA,im_sum,im_sum_sqr);
  TEST_NEAR("integral_sqr_image (sum)",im_sum(n-1,m-1),sumA,1e-6);
  TEST_NEAR("integral_sqr_image (sum sqr)",im_sum_sqr(n-1,m-1),sum_sqrA,1e-6);

  vil2_image_view<float> f_image(5,5);
  f_image.fill(1.0);
  f_image(3,3)=17;
  vcl_cout<<"Testing vil2_math_normalise\n";
  vil2_math_normalise(f_image);
  double f_mean,f_var;
  vil2_math_mean_and_variance(f_mean,f_var,f_image,0);
  TEST_NEAR("Mean",f_mean,0,1e-6);
  TEST_NEAR("Var",f_var,1.0,1e-6);


  // extra test for normalisation

  int nx=5;
  int ny=5;
  vil2_image_view<float> orig_image(nx,ny),
                         var_norm_image,
                         correct_var_norm_image(nx,ny);

  // Create original image
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      orig_image(x,y)=1.25f*x;

  vil2_print_all( vcl_cout, orig_image );

   var_norm_image=vil2_copy_deep(orig_image);
  vil2_math_normalise( var_norm_image);

  vil2_print_all( vcl_cout, var_norm_image );

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

MAIN( test_image_view_maths )
{
  START( "vil2_image_view_maths" );

  test_image_view_maths_byte();

  SUMMARY();
}
