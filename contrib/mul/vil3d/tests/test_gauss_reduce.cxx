// This is mul/vil3d/tests/test_gauss_reduce.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/algo/vil3d_gauss_reduce.h>

static void test_gauss_reduce_float()
{
  vcl_cout << "****************************\n"
           << " Testing vil3d_gauss_reduce\n"
           << "****************************\n";

  unsigned ni = 20, nj = 20, nk = 20;
  vcl_cout<<"Image Size: "<<ni<<" x "<<nj<<" k "<<nk<<vcl_endl;

  vil3d_image_view<float> image0;
  image0.set_size(ni,nj,nk);

  for (unsigned k=0;k<image0.nk();++k)
    for (unsigned j=0;j<image0.nj();++j)
      for (unsigned i=0;i<image0.ni();++i)
        image0(i,j,k) = i*0.1f+j+k*10;


  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = (nk+1)/2;
  vil3d_image_view<float> image1,work_im1,work_im2;
  vil3d_gauss_reduce(image0,image1,work_im1,work_im2);
  TEST("size i",image1.ni(),(ni+1)/2);
  TEST("size j",image1.nj(),(nj+1)/2);
  TEST("size k",image1.nk(),(nk+1)/2);
  TEST_NEAR("Pixel (0,0,0)",image0(0,0,0),image1(0,0,0),1e-5);
  TEST_NEAR("Pixel (1,1,1)",image0(2,2,2),image1(1,1,1),1e-5);
  TEST_NEAR("Pixel (2,3,3)",image1(2,3,3),66.4f,1e-4);
  TEST_NEAR("Corner pixel",image0(ni2*2-2,nj2*2-2,nk2*2-2),image1(ni2-1,nj2-1,nk2-1),1e-5);
}

// Check in-homogeneous smoothing option (ie onlj smooth in i,j but not k on some levels)
static void test_gauss_reduce_ij()
{
  vcl_cout<<"test_gauss_reduce_ij()\n";
  unsigned ni = 20, nj = 20, nk = 10;

  vil3d_image_view<float> image0;
  image0.set_size(ni,nj,nk);

  for (unsigned k=0;k<image0.nk();++k)
    for (unsigned j=0;j<image0.nj();++j)
      for (unsigned i=0;i<image0.ni();++i)
        image0(i,j,k) = i*0.1f+j+k*10;

  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = nk;  // Shouldn't change first level

  vil3d_image_view<float> image1, work_im;
  vil3d_gauss_reduce_ij(image0,image1,work_im);

  TEST("Level 1 size i",image1.ni(),ni2);
  TEST("Level 1 size j",image1.nj(),nj2);
  TEST("Level 1 size k",image1.nk(),ni2);
  TEST_NEAR("Pixel (0,0,0)",image0(0,0,0),image1(0,0,0),1e-5);
  TEST_NEAR("Pixel (1,1,1)",image0(2,2,2),image1(1,1,2),1e-5);
  TEST_NEAR("Pixel (2,3,3)",image1(2,3,3),36.4f,1e-4);
  TEST_NEAR("Corner pixel",image0(ni2*2-2,nj2*2-2,nk2-1),image1(ni2-1,nj2-1,nk2-1),1e-5);
}

// Check in-homogeneous smoothing option (ie onlj smooth in i,k but not j on some levels)
static void test_gauss_reduce_ik()
{
  vcl_cout<<"test_gauss_reduce_ik()\n";
  int ni = 20, nj = 10, nk = 20;

  vil3d_image_view<float> image0;
  image0.set_size(ni,nj,nk);

  for (unsigned k=0;k<image0.nk();++k)
    for (unsigned j=0;j<image0.nj();++j)
      for (unsigned i=0;i<image0.ni();++i)
        image0(i,j,k) = i*0.1f+j+k*10;

  vil3d_image_view<float> image1, work_im;
  vil3d_gauss_reduce_ik(image0,image1,work_im);

  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = nj;   // Shouldn't change first level
  unsigned nk2 = (nk+1)/2;
  TEST("Level 1 size i",image1.ni(),ni2);
  TEST("Level 1 size j",image1.nj(),nj2);
  TEST("Level 1 size k",image1.nk(),ni2);
  TEST_NEAR("Pixel (0,0,0)",image0(0,0,0),image1(0,0,0),1e-5);
  TEST_NEAR("Pixel (1,1,1)",image0(2,2,2),image1(1,2,1),1e-5);
  TEST_NEAR("Pixel (2,3,3)",image1(2,3,3),63.4f,1e-4);
  TEST_NEAR("Corner pixel",image0(ni2*2-2,nj2-1,nk2*2-2),image1(ni2-1,nj2-1,nk2-1),1e-5);
}

// Check in-homogeneous smoothing option (ie onlj smooth in j,k but not i on some levels)
static void test_gauss_reduce_jk()
{
  vcl_cout<<"test_gauss_reduce_jk()\n";
  unsigned ni = 10, nj = 20, nk = 20;

  vil3d_image_view<float> image0;
  image0.set_size(ni,nj,nk);

  for (unsigned k=0;k<image0.nk();++k)
    for (unsigned j=0;j<image0.nj();++j)
      for (unsigned i=0;i<image0.ni();++i)
        image0(i,j,k) = i*0.1f+j+k*10;

  vil3d_image_view<float> image1, work_im;
  vil3d_gauss_reduce_jk(image0,image1,work_im);

  unsigned ni2 = ni;    // Shouldn't change first level
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = (nk+1)/2;
  TEST("Level 1 size i",image1.ni(),ni2);
  TEST("Level 1 size j",image1.nj(),nj2);
  TEST("Level 1 size k",image1.nk(),ni2);
  TEST_NEAR("Pixel (0,0,0)",image0(0,0,0),image1(0,0,0),1e-5);
  TEST_NEAR("Pixel (1,1,1)",image0(2,2,2),image1(2,1,1),1e-5);
  TEST_NEAR("Pixel (2,3,3)",image1(2,3,3),66.2f,1e-4);
  TEST_NEAR("Corner pixel",image0(ni2-1,nj2*2-2,nk2*2-2),image1(ni2-1,nj2-1,nk2-1),1e-5);
}

static void test_gauss_reduce()
{
  test_gauss_reduce_float();
  test_gauss_reduce_ij();
  test_gauss_reduce_ik();
  test_gauss_reduce_jk();
}

TESTMAIN(test_gauss_reduce);
