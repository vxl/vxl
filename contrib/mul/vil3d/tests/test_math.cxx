// This is mul/vil3d/tests/test_math.cxx
#include <iostream>
#include <iomanip>
#include <testlib/testlib_test.h>
// not used? #include <vcl_compiler.h>
#include <vil3d/vil3d_math.h>


static void test_image_view_maths_float()
{
  std::cout << "********************************\n"
           << " Testing vil3d_image_view_maths\n"
           << "********************************\n";

  unsigned ni=5, nj=6, nk = 7;

  vil3d_image_view<float> imA(ni,nj,nk);

  double sum0 = 0,sum_sq0=0;
  for (unsigned int k=0;k<imA.nk();++k)
    for (unsigned int j=0;j<imA.nj();++j)
      for (unsigned int i=0;i<imA.ni();++i)
      {
        imA(i,j,k) = 1.f+i+j*ni+3*k; sum0+=imA(i,j,k); sum_sq0+= imA(i,j,k)*imA(i,j,k);
      }

  double sum;
  vil3d_math_sum(sum,imA,0);
  TEST_NEAR("Sum",sum,sum0,1e-8);

  double mean;
  vil3d_math_mean(mean,imA,0);
  TEST_NEAR("mean",mean,sum0/(ni*nj*nk),1e-8);

  double sum_sq;
  vil3d_math_sum_squares(sum,sum_sq,imA,0);
  TEST_NEAR("Sum",sum,sum0,1e-8);
  TEST_NEAR("Sum of squares",sum_sq,sum_sq0,1e-8);
}


static void test_math_value_range()
{
  std::cout << "********************************\n"
           << " Testing vil3d_math_value_range\n"
           << "********************************\n";

  // Create a test image with values 1...1000
  unsigned ni=10, nj=10, nk=10;
  vil3d_image_view<int> img(ni, nj, nk);
  for (unsigned i=0; i<ni; ++i)
  {
    for (unsigned j=0; j<nj; ++j)
    {
      for (unsigned k=0; k<nk; ++k)
      {
        int val = k*nj*ni + j*ni + i +1; // NB Data values not already sorted!
        img(i,j,k) = val;
      }
    }
  }

  // Check the min/max values
  int min=2000;
  int max=-2000;
  vil3d_math_value_range(img, min, max);
  TEST("vil3d_math_value_range(): min", min==1, true);
  TEST("vil3d_math_value_range(): max", max==1000, true);

  int val=2000;

  // Test a likely pair of percentiles
  vil3d_math_value_range_percentile(img, 0.05, val);
  TEST("vil3d_math_value_range_percentile():  5  %", val==50, true);
  vil3d_math_value_range_percentile(img, 0.95, val);
  TEST("vil3d_math_value_range_percentile(): 95  %", val==950, true);

  // Test a likely pair of percentiles
  vil3d_math_value_range_percentile(img, 0.10, val);
  TEST("vil3d_math_value_range_percentile(): 10  %", val==100, true);
  vil3d_math_value_range_percentile(img, 0.90, val);
  TEST("vil3d_math_value_range_percentile(): 90  %", val==900, true);

  // Test an unlikely pair of percentiles
  vil3d_math_value_range_percentile(img, 0.311, val);
  TEST("vil3d_math_value_range_percentile(): 31.1 %", val==311, true);
  vil3d_math_value_range_percentile(img, 0.737, val);
  TEST("vil3d_math_value_range_percentile(): 73.7 %", val==737, true);

  // Test several percentiles at once
  unsigned int nfrac = 9;
  std::vector<double> fraction(nfrac);
  std::vector<double> true_value(nfrac);
  fraction[0] = 0.000;  true_value[0] =    1;
  fraction[1] = 0.050;  true_value[1] =   50;
  fraction[2] = 0.100;  true_value[2] =  100;
  fraction[3] = 0.311;  true_value[3] =  311;
  fraction[4] = 0.500;  true_value[4] =  500;
  fraction[5] = 0.737;  true_value[5] =  737;
  fraction[6] = 0.900;  true_value[6] =  900;
  fraction[7] = 0.950;  true_value[7] =  950;
  fraction[8] = 1.000;  true_value[8] = 1000;
  std::vector<int> value;
  vil3d_math_value_range_percentiles(img, fraction, value);
  bool all_correct = true;
  for (unsigned f=0; f<nfrac; ++f)
  {
    if (value[f] != true_value[f])
    {
      all_correct = false;
    }
  }
  TEST("vil3d_math_value_range_percentiles(): all correct", all_correct, true);
}

static void test_math_integral_image()
{
  std::cout << "***********************************\n"
           << " Testing vil3d_math_integral_image\n"
           << "***********************************\n";
  // create a 3x3x3 image
  unsigned n = 3;
  vil3d_image_view<vxl_byte> im(n,n,n,1);
  im(0,0,0)=3 ; im(1,0,0)=5 ; im(2,0,0)=2 ;
  im(0,1,0)=1 ; im(1,1,0)=10; im(2,1,0)=7 ;
  im(0,2,0)=15; im(1,2,0)=3 ; im(2,2,0)=8 ;
  im(0,0,1)=2 ; im(1,0,1)=7 ; im(2,0,1)=8 ;
  im(0,1,1)=9 ; im(1,1,1)=3 ; im(2,1,1)=5 ;
  im(0,2,1)=4 ; im(1,2,1)=1 ; im(2,2,1)=7 ;
  im(0,0,2)=5 ; im(1,0,2)=8 ; im(2,0,2)=2 ;
  im(0,1,2)=2 ; im(1,1,2)=7 ; im(2,1,2)=3 ;
  im(0,2,2)=4 ; im(1,2,2)=5 ; im(2,2,2)=8 ;

  // explicitly define the integral image
  unsigned n1 = n+1;
  vil3d_image_view<vxl_int_16> im_sum1(n1,n1,n1,1);
  im_sum1.fill(0);
  im_sum1(1,1,1)=3  ; im_sum1(2,1,1)=8  ; im_sum1(3,1,1)=10 ;
  im_sum1(1,2,1)=4  ; im_sum1(2,2,1)=19 ; im_sum1(3,2,1)=28 ;
  im_sum1(1,3,1)=19 ; im_sum1(2,3,1)=37 ; im_sum1(3,3,1)=54 ;
  im_sum1(1,1,2)=5  ; im_sum1(2,1,2)=17 ; im_sum1(3,1,2)=27 ;
  im_sum1(1,2,2)=15 ; im_sum1(2,2,2)=40 ; im_sum1(3,2,2)=62 ;
  im_sum1(1,3,2)=34 ; im_sum1(2,3,2)=63 ; im_sum1(3,3,2)=100;
  im_sum1(1,1,3)=10 ; im_sum1(2,1,3)=30 ; im_sum1(3,1,3)=42 ;
  im_sum1(1,2,3)=22 ; im_sum1(2,2,3)=62 ; im_sum1(3,2,3)=89 ;
  im_sum1(1,3,3)=45 ; im_sum1(2,3,3)=94 ; im_sum1(3,3,3)=144;

  // do voxel-wise comparison with value returned from integral_image fn
  vil3d_image_view<vxl_int_16> im_sum2;
  vil3d_math_integral_image(im,im_sum2);
  bool all_correct = true;
  for (unsigned k=0; k<n1; k++)
    for (unsigned j=0; j<n1; j++)
      for (unsigned i=0; i<n1; i++)
        if (im_sum1(i,j,k) != im_sum2(i,j,k))
          all_correct = false;
  TEST("vil3d_math_integral_image(): all correct", all_correct, true);

  // print out values if test failed
  if (!all_correct)
  {
    std::cout << "Expected and obtained values for integral image\n";
    for (unsigned k=0; k<n1; k++)
    {
      for (unsigned j=0; j<n1; j++)
      {
        for (unsigned i=0; i<n1; i++)
          std::cout << im_sum1(i,j,k) << ' ';
        std::cout << "\t\t";
        for (unsigned i=0; i<n1; i++)
          std::cout << im_sum2(i,j,k) << ' ';
        std::cout << std::endl;
      }
    }
  }
}

static void test_math_integral_image_sqr()
{
  std::cout << "***************************************\n"
           << " Testing vil3d_math_integral_image_sqr\n"
           << "***************************************\n";
  // create a 3x3x3 image
  unsigned n = 3;
  vil3d_image_view<vxl_byte> im(n,n,n,1);
  im(0,0,0)=3 ; im(1,0,0)=5 ; im(2,0,0)=2 ;
  im(0,1,0)=1 ; im(1,1,0)=10; im(2,1,0)=7 ;
  im(0,2,0)=15; im(1,2,0)=3 ; im(2,2,0)=8 ;
  im(0,0,1)=2 ; im(1,0,1)=7 ; im(2,0,1)=8 ;
  im(0,1,1)=9 ; im(1,1,1)=3 ; im(2,1,1)=5 ;
  im(0,2,1)=4 ; im(1,2,1)=1 ; im(2,2,1)=7 ;
  im(0,0,2)=5 ; im(1,0,2)=8 ; im(2,0,2)=2 ;
  im(0,1,2)=2 ; im(1,1,2)=7 ; im(2,1,2)=3 ;
  im(0,2,2)=4 ; im(1,2,2)=5 ; im(2,2,2)=8 ;

  // explicitly define the integral image
  unsigned n1 = n+1;
  vil3d_image_view<vxl_int_16> im_sum1(n1,n1,n1,1);
  im_sum1.fill(0);
  im_sum1(1,1,1)=3  ; im_sum1(2,1,1)=8  ; im_sum1(3,1,1)=10 ;
  im_sum1(1,2,1)=4  ; im_sum1(2,2,1)=19 ; im_sum1(3,2,1)=28 ;
  im_sum1(1,3,1)=19 ; im_sum1(2,3,1)=37 ; im_sum1(3,3,1)=54 ;
  im_sum1(1,1,2)=5  ; im_sum1(2,1,2)=17 ; im_sum1(3,1,2)=27 ;
  im_sum1(1,2,2)=15 ; im_sum1(2,2,2)=40 ; im_sum1(3,2,2)=62 ;
  im_sum1(1,3,2)=34 ; im_sum1(2,3,2)=63 ; im_sum1(3,3,2)=100;
  im_sum1(1,1,3)=10 ; im_sum1(2,1,3)=30 ; im_sum1(3,1,3)=42 ;
  im_sum1(1,2,3)=22 ; im_sum1(2,2,3)=62 ; im_sum1(3,2,3)=89 ;
  im_sum1(1,3,3)=45 ; im_sum1(2,3,3)=94 ; im_sum1(3,3,3)=144;

  // explicitly define the integral square image
  vil3d_image_view<vxl_int_16> im_sum1_sq(n1,n1,n1,1);
  im_sum1_sq.fill(0);
  im_sum1_sq(1,1,1)=9  ; im_sum1_sq(2,1,1)=34 ; im_sum1_sq(3,1,1)=38 ;
  im_sum1_sq(1,2,1)=10 ; im_sum1_sq(2,2,1)=135; im_sum1_sq(3,2,1)=188;
  im_sum1_sq(1,3,1)=235; im_sum1_sq(2,3,1)=369; im_sum1_sq(3,3,1)=486;
  im_sum1_sq(1,1,2)=13 ; im_sum1_sq(2,1,2)=87 ; im_sum1_sq(3,1,2)=155;
  im_sum1_sq(1,2,2)=95 ; im_sum1_sq(2,2,2)=278; im_sum1_sq(3,2,2)=420;
  im_sum1_sq(1,3,2)=336; im_sum1_sq(2,3,2)=529; im_sum1_sq(3,3,2)=784;
  im_sum1_sq(1,1,3)=38 ; im_sum1_sq(2,1,3)=176; im_sum1_sq(3,1,3)=248;
  im_sum1_sq(1,2,3)=124; im_sum1_sq(2,2,3)=420; im_sum1_sq(3,2,3)=575;
  im_sum1_sq(1,3,3)=381; im_sum1_sq(2,3,3)=712; im_sum1_sq(3,3,3)=1044;

  // Compute sum and sum_sq integral images
  vil3d_image_view<vxl_int_16> im_sum2, im_sum2_sq;
  vil3d_math_integral_sqr_image(im,im_sum2,im_sum2_sq);

  // do voxel-wise comparison with value returned from integral_image fn
  bool all_correct = true;
  for (unsigned k=0; k<n1; k++)
    for (unsigned j=0; j<n1; j++)
      for (unsigned i=0; i<n1; i++)
        if (im_sum1(i,j,k) != im_sum2(i,j,k))
          all_correct = false;
  TEST("vil3d_math_integral_image_sqr(): all correct for sum",
       all_correct, true);

  // print out values if test failed
  if (!all_correct)
  {
    std::cout << "Expected and obtained values for sum integral image\n";
    for (unsigned k=0; k<n1; k++)
    {
      for (unsigned j=0; j<n1; j++)
      {
        for (unsigned i=0; i<n1; i++)
          std::cout << im_sum1(i,j,k) << ' ';
        std::cout << "\t\t";
        for (unsigned i=0; i<n1; i++)
          std::cout << im_sum2(i,j,k) << ' ';
        std::cout << std::endl;
      }
    }
  }

  // ditto for sum_sq
  all_correct = true;
  for (unsigned k=0; k<n1; k++)
    for (unsigned j=0; j<n1; j++)
      for (unsigned i=0; i<n1; i++)
        if (im_sum1_sq(i,j,k) != im_sum2_sq(i,j,k))
          all_correct = false;
  TEST("vil3d_math_integral_image_sqr(): all correct for sqr",
       all_correct, true);

  // print out values if test failed
  if (!all_correct)
  {
    std::cout << "Expected and obtained values for sum_sq integral image\n";
    for (unsigned k=0; k<n1; k++)
    {
      for (unsigned j=0; j<n1; j++)
      {
        for (unsigned i=0; i<n1; i++)
          std::cout << im_sum1_sq(i,j,k) << ' ';
        std::cout << "\t\t";
        for (unsigned i=0; i<n1; i++)
          std::cout << im_sum2_sq(i,j,k) << ' ';
        std::cout << std::endl;
      }
    }
  }
}

static void test_math()
{
  test_image_view_maths_float();
  test_math_value_range();
  test_math_integral_image();
  test_math_integral_image_sqr();
}

TESTMAIN(test_math);
