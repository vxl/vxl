// This is mul/vil3d/tests/test_algo_normalised_correlation_3d.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil3d/algo/vil3d_normalised_correlation_3d.h>

static void test_algo_normalised_correlation_3d_byte()
{
  vcl_cout << "*********************************************\n"
           << " Testing vil3d_algo_normalised_correlation_3d\n"
           << "*********************************************\n";

  unsigned ni=10, nj=11, nk=12, m=5;

  vil3d_image_view<vxl_byte> src_im(ni,nj,nk,1);


  for (unsigned int k=0;k<src_im.nk();++k)
   for (unsigned int j=0;j<src_im.nj();++j)
    for (unsigned int i=0;i<src_im.ni();++i)
      src_im(i,j,k) = 1+i+j*ni+k*ni*nj;

      // Create kernel1 (all values equal to unity)
      // Note that really kernel should have mean zero and SD unity
  vil3d_image_view<float> kernel1(m,m,m);
  kernel1.fill(1.0f);

  vil3d_image_view<double> dest_im;

  vil3d_normalised_correlation_3d(src_im,dest_im,kernel1,double());

  TEST("Destination size i",dest_im.ni(),1+ni-m);
  TEST("Destination size j",dest_im.nj(),1+nj-m);
  TEST("Destination size k",dest_im.nk(),1+nk-m);

  // Independent measure
  double sum=0.0,sum2=0.0;
  for (unsigned int k=0;k<m;++k)
   for (unsigned int j=0;j<m;++j)
    for (unsigned int i=0;i<m;++i)
    {  sum+=src_im(i,j,k); sum2+=src_im(i,j,k)*src_im(i,j,k); }

  double mean=sum/(m*m*m);
  double var =sum2/(m*m*m)-mean*mean;
  TEST_NEAR("Test element (0,0,0)",dest_im(0,0,0),sum/vcl_sqrt(var),1e-6);
}

static void test_algo_normalised_correlation_3d()
{
  test_algo_normalised_correlation_3d_byte();
}

TESTMAIN(test_algo_normalised_correlation_3d);
