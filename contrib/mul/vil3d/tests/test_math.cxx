// This is mul/vil3d/tests/test_math.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil3d/vil3d_math.h>

static void test_image_view_maths_float()
{
  vcl_cout << "********************************\n"
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

static void test_math()
{
  test_image_view_maths_float();
}

TESTMAIN(test_math);
