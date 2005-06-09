// This is core/vil/tests/test_image_view_maths.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_copy.h>
#include <vil/vil_math.h>
#include <vil/vil_print.h>

static void test_image_view_maths_byte()
{
  vcl_cout << "******************************\n"
           << " Testing vil_image_view_maths\n"
           << "******************************\n";

  int n=10, m=8;

  vil_image_view<vxl_byte> imA(n,m,1);
  vil_image_view<vxl_byte> imB(n,m,1);

  double sum2 = 0;
  for (unsigned int j=0;j<imA.nj();++j)
    for (unsigned int i=0;i<imA.ni();++i)
    {
      imA(i,j) = 1+i+j*n; sum2+= imA(i,j)*imA(i,j);
      imB(i,j) = 1+j+i*n;
    }

  double sum;
  vil_math_sum(sum,imA,0);
  TEST_NEAR("Sum",sum,0.5*n*m*(n*m+1),1e-8);

  double mean;
  vil_math_mean(mean,imA,0);
  TEST_NEAR("mean",mean,0.5*(n*m+1),1e-8);

  double sum_sq;
  vil_math_sum_squares(sum,sum_sq,imA,0);
  TEST_NEAR("Sum of squares",sum_sq,sum2,1e-8);

  TEST_NEAR("Sum of squared differences", vil_math_ssd(imA, imB, double()),93960.0, 1e-8);

  vil_image_view<vxl_byte> imC = vil_copy_deep(imA);
  vil_math_scale_values(imC,2.0);
  TEST_NEAR("Values scaled",imC(3,5),imA(3,5)*2,1e-8);

  imC = vil_copy_deep(imA);
  vil_math_scale_and_offset_values(imC,2.0,7);
  TEST_NEAR("Values scaled+offset",imC(3,5),imA(3,5)*2+7,1e-8);

  vil_image_view<float> im_sum,im_sum_sqr;
  vil_math_image_sum(imA,imB,im_sum);
  TEST("Width of im_sum",im_sum.ni(),imA.ni());
  TEST("Height of im_sum",im_sum.nj(),imA.nj());
  TEST_NEAR("vil_math_image_sum : im_sum(5,7)",
            im_sum(5,7),float(imA(5,7))+float(imB(5,7)),1e-6);

  vil_image_view<float> im_product;
  vil_math_image_product(imA,imB,im_product);
  TEST("Width of im_sum",im_product.ni(),imA.ni());
  TEST("Height of im_sum",im_product.nj(),imA.nj());
  TEST_NEAR("vil_math_image_product : im_product(5,7)",
            im_product(5,7),float(imA(5,7))*float(imB(5,7)),1e-6);


  vil_image_view<float> im_ratio;
  vil_math_image_ratio(imA,imB,im_ratio);
  TEST("Width of im_ratio",im_ratio.ni(),imA.ni());
  TEST("Height of im_ratio",im_ratio.nj(),imA.nj());
  TEST_NEAR("vil_math_image_product : im_ratio(5,7)",
            im_ratio(5,7),float(imA(5,7))/float(imB(5,7)),1e-6);

  vil_image_view<float> im3(5,6,3),im_rms;
  im3.fill(1.7f);
  im3(2,3,0)=2.0;
  im3(2,3,1)=3.0;
  im3(2,3,2)=4.0;
  vil_math_rms(im3,im_rms);
  TEST_NEAR("im_rms(1,1)",im_rms(1,1),1.7,1e-6);
  TEST_NEAR("im_rms(2,3)",im_rms(2,3),vcl_sqrt(29.0/3.0),1e-6);

  vil_image_view<float> im_diff;
  vil_math_image_difference(imA,imB,im_diff);
  TEST_NEAR("im_diff(5,2)",im_diff(5,2),float(imA(5,2))-float(imB(5,2)),1e-6);

  vil_image_view<float> im_abs_diff;
  vil_math_image_abs_difference(imA,im_sum,im_abs_diff);
  TEST_NEAR("im_abs_diff(3,7)",im_abs_diff(3,7),vcl_fabs(float(imA(3,7))-float(im_sum(3,7))),1e-6);

  float is45 = im_sum(4,5);
  vil_math_add_image_fraction(im_sum,0.77,imA,0.23);
  TEST_NEAR("add_fraction",im_sum(4,5),0.77*is45+0.23*imA(4,5),1e-5);

  double sumA,sum_sqrA;
  vil_math_sum_squares(sumA,sum_sqrA,imA,0);
  vil_math_integral_image(imA,im_sum);
  TEST_NEAR("integral_image",im_sum(n,m),sumA,1e-6);

  vil_math_integral_sqr_image(imA,im_sum,im_sum_sqr);
  TEST_NEAR("integral_sqr_image (sum)",im_sum(n,m),sumA,1e-6);
  TEST_NEAR("integral_sqr_image (sum sqr)",im_sum_sqr(n,m),sum_sqrA,1e-6);

  vil_image_view<float> f_image(5,5);
  f_image.fill(1.0);
  f_image(3,3)=17;
  vcl_cout<<"Testing vil_math_normalise\n";
  vil_math_normalise(f_image);
  double f_mean,f_var;
  vil_math_mean_and_variance(f_mean,f_var,f_image,0);
  TEST_NEAR("Mean",f_mean,0,1e-6);
  TEST_NEAR("Var",f_var,1.0,1e-6);

  // Test range truncation
  vil_image_view<float> trun_image(5,5);
  trun_image.fill(17);
  vil_math_truncate_range(trun_image,0.0f,3.5f);
  TEST_NEAR("vil_math_truncate_range Max test",trun_image(2,3),3.5,1e-6);
  vil_math_truncate_range(trun_image,4.7f,8.5f);
  TEST_NEAR("vil_math_truncate_range Min test",trun_image(0,1),4.7,1e-6);


  // extra test for normalisation

  int nx=5;
  int ny=5;
  vil_image_view<float> orig_image(nx,ny),
                         var_norm_image,
                         correct_var_norm_image(nx,ny);

  // Create original image
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      orig_image(x,y)=1.25f*x;

  vil_print_all( vcl_cout, orig_image );

  var_norm_image=vil_copy_deep(orig_image);
  vil_math_normalise( var_norm_image);

  vil_print_all( vcl_cout, var_norm_image );

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

  // Testing square-root function (float)
  vil_image_view<float> fim_sqrt(5,5);
  fim_sqrt.fill(4.0f);
  fim_sqrt(1,2)=-1.0f;
  fim_sqrt(3,4)=9.0f;
  vil_math_sqrt(fim_sqrt);
  TEST_NEAR("vil_math_sqrt (a)",fim_sqrt(1,1),2.0,1e-6);
  TEST_NEAR("vil_math_sqrt (b)",fim_sqrt(3,4),3.0,1e-6);
  TEST_NEAR("vil_math_sqrt (-ives)",fim_sqrt(1,2),0.0,1e-6);

  // Testing square-root function (int)
  vil_image_view<int> iim_sqrt(5,5);
  iim_sqrt.fill(4);
  iim_sqrt(1,2)=-1;
  iim_sqrt(3,4)=9;
  vil_math_sqrt(iim_sqrt);
  TEST_NEAR("vil_math_sqrt (a)",iim_sqrt(1,1),2,1e-6);
  TEST_NEAR("vil_math_sqrt (b)",iim_sqrt(3,4),3,1e-6);
  TEST_NEAR("vil_math_sqrt (-ives)",iim_sqrt(1,2),0,1e-6);

  // Testing square-root function (vxl_byte)
  vil_image_view<vxl_byte> bim_sqrt(5,5);
  bim_sqrt.fill(vxl_byte(4));
  bim_sqrt(1,2)=8;
  bim_sqrt(3,4)=9;
  vil_math_sqrt(bim_sqrt);
  TEST_NEAR("vil_math_sqrt (4)",bim_sqrt(1,1),2,1e-6);
  TEST_NEAR("vil_math_sqrt (9)",bim_sqrt(3,4),3,1e-6);
  TEST_NEAR("vil_math_sqrt (8)",bim_sqrt(1,2),3,1e-6);

  // Testing max function (vxl_byte)
  vil_image_view<vxl_byte> bim_max1(5,5);
  vil_image_view<vxl_byte> bim_max2(5,5);
  vil_image_view<vxl_byte> bim_max_out(5,5);
  bim_max1.fill(4);
  bim_max1(2,3) = 0;
  bim_max2.fill(8);
  bim_max2(1,2) = 4;
  bim_max2(2,3) = 1;
  vil_math_image_max(bim_max1, bim_max2, bim_max_out);  
  TEST_NEAR("vil_math_image_max (a)",bim_max_out(1,2),4,1e-6);
  TEST_NEAR("vil_math_image_max (b)",bim_max_out(2,2),8,1e-6);
  TEST_NEAR("vil_math_image_max (c)",bim_max_out(2,3),1,1e-6);

  // Testing max function (float)
  vil_image_view<float> fim_max1(5,5);
  vil_image_view<float> fim_max2(5,5);
  vil_image_view<float> fim_max_out(5,5);
  fim_max1.fill(4.2f);
  fim_max1(2,3) = -1.5f;
  fim_max2.fill(25.f);
  fim_max2(1,2) = 4.25f;
  fim_max2(2,3) = -3.f;
  vil_math_image_max(fim_max1, fim_max2, fim_max_out);  
  TEST_NEAR("vil_math_image_max (d)",fim_max_out(1,2),4.25f,1e-6);
  TEST_NEAR("vil_math_image_max (e)",fim_max_out(2,2),25.f,1e-6);
  TEST_NEAR("vil_math_image_max (f)",fim_max_out(2,3),-1.5f,1e-6);


  // Testing min function (vxl_byte)
  vil_image_view<vxl_byte> bim_min1(5,5);
  vil_image_view<vxl_byte> bim_min2(5,5);
  vil_image_view<vxl_byte> bim_min_out(5,5);
  bim_min1.fill(4);
  bim_min1(2,3) = 0;
  bim_min2.fill(8);
  bim_min2(1,2) = 4;
  bim_min2(2,3) = 1;
  vil_math_image_min(bim_min1, bim_min2, bim_min_out);  
  TEST_NEAR("vil_math_image_min (a)",bim_min_out(1,2),4,1e-6);
  TEST_NEAR("vil_math_image_min (b)",bim_min_out(2,2),4,1e-6);
  TEST_NEAR("vil_math_image_min (c)",bim_min_out(2,3),0,1e-6);

  // Testing min function (float)
  vil_image_view<float> fim_min1(5,5);
  vil_image_view<float> fim_min2(5,5);
  vil_image_view<float> fim_min_out(5,5);
  fim_min1.fill(4.2f);
  fim_min1(2,3) = -1.5f;
  fim_min2.fill(25.f);
  fim_min2(1,2) = 4.25f;
  fim_min2(2,3) = -3.f;
  vil_math_image_min(fim_min1, fim_min2, fim_min_out);  
  TEST_NEAR("vil_math_image_min (d)",fim_min_out(1,2),4.2f,1e-6);
  TEST_NEAR("vil_math_image_min (e)",fim_min_out(2,2),4.2f,1e-6);
  TEST_NEAR("vil_math_image_min (f)",fim_min_out(2,3),-3.f,1e-6);
}

static void test_image_view_maths()
{
  test_image_view_maths_byte();
}

TESTMAIN(test_image_view_maths);
