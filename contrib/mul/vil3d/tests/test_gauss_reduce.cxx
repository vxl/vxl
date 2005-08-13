// This is mul/vil3d/tests/test_gauss_reduce.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/algo/vil3d_gauss_reduce.h>
#include <vil3d/vil3d_print.h>


static void test_gauss_reduce_float()
{
  vcl_cout << "***********************************\n"
           << " Testing vil3d_gauss_reduce<float>\n"
           << "***********************************\n";

  unsigned ni = 10, nj = 20, nk = 30;
  vcl_cout<<"Image Size: "<<ni<<" x "<<nj<<" k "<<nk<<vcl_endl;

  vil3d_image_view<float> image0;
  image0.set_size(ni,nj,nk);

  for (unsigned k=0;k<image0.nk();++k)
    for (unsigned j=0;j<image0.nj();++j)
      for (unsigned i=0;i<image0.ni();++i)
        image0(i,j,k) = i*0.1f-j+k*10;


  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = (nk+1)/2;
  vil3d_image_view<float> image1,work_im1,work_im2;
  vil3d_gauss_reduce(image0,image1,work_im1,work_im2);
  TEST("size i",image1.ni(), ni2);
  TEST("size j",image1.nj(), nj2);
  TEST("size k",image1.nk(), nk2);
  TEST_NEAR("Pixel (0,0,0)",image0(0,0,0),image1(0,0,0),1e-5);
  TEST_NEAR("Pixel (1,1,1)",image0(2,2,2),image1(1,1,1),1e-5);
  TEST_NEAR("Pixel (2,3,3)",image0(4,6,6),image1(2,3,3),1e-5);
  TEST_NEAR("RFT corner pixel",image0(ni2*2-2,nj2*2-2,nk2*2-2),image1(ni2-1,nj2-1,nk2-1),1e-5);
  TEST_NEAR("RFB corner pixel",image0(ni2*2-2,nj2*2-2,0),      image1(ni2-1,nj2-1,0)    ,1e-5);
  TEST_NEAR("RNT corner pixel",image0(ni2*2-2,0,nk2*2-2),      image1(ni2-1,0,nk2-1)    ,1e-5);
  TEST_NEAR("RNB corner pixel",image0(ni2*2-2,0,0),            image1(ni2-1,0,0)        ,1e-5);
  TEST_NEAR("LFT corner pixel",image0(0,nj2*2-2,nk2*2-2),      image1(0,nj2-1,nk2-1)    ,1e-5);
  TEST_NEAR("LFB corner pixel",image0(0,nj2*2-2,0),            image1(0,nj2-1,0)        ,1e-5);
  TEST_NEAR("LNT corner pixel",image0(0,0,nk2*2-2),            image1(0,0,nk2-1)        ,1e-5);
}

// Check in-homogeneous smoothing option (ie onlj smooth in i,j but not k on some levels)
static void test_gauss_reduce_ij()
{
  vcl_cout << "**************************************\n"
           << " Testing vil3d_gauss_reduce_ij<float>\n"
           << "**************************************\n";

  unsigned ni = 10, nj = 20, nk = 30;
  vcl_cout<<"Image Size: "<<ni<<" x "<<nj<<" k "<<nk<<vcl_endl;

  vil3d_image_view<float> image0;
  image0.set_size(ni,nj,nk);

  for (unsigned k=0;k<image0.nk();++k)
    for (unsigned j=0;j<image0.nj();++j)
      for (unsigned i=0;i<image0.ni();++i)
        image0(i,j,k) = i*0.1f-j+k*10;

  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = nk;  // Shouldn't change first level

  vil3d_image_view<float> image1, work_im;
  vil3d_gauss_reduce_ij(image0,image1,work_im);

  TEST("Level 1 size i",image1.ni(),ni2);
  TEST("Level 1 size j",image1.nj(),nj2);
  TEST("Level 1 size k",image1.nk(),nk2);
  TEST_NEAR("Pixel (0,0,0)",image0(0,0,0),image1(0,0,0),1e-5);
  TEST_NEAR("Pixel (1,1,1)",image0(2,2,2),image1(1,1,2),1e-5);
  TEST_NEAR("Pixel (2,3,3)",image0(4,6,3),image1(2,3,3),1e-5);
  TEST_NEAR("RFT corner pixel",image0(ni2*2-2,nj2*2-2,nk2-1),image1(ni2-1,nj2-1,nk2-1),1e-5);
  TEST_NEAR("RFB corner pixel",image0(ni2*2-2,nj2*2-2,0),    image1(ni2-1,nj2-1,0)    ,1e-5);
  TEST_NEAR("RNT corner pixel",image0(ni2*2-2,0,nk2-1),      image1(ni2-1,0,nk2-1)    ,1e-5);
  TEST_NEAR("RNB corner pixel",image0(ni2*2-2,0,0),          image1(ni2-1,0,0)        ,1e-5);
  TEST_NEAR("LFT corner pixel",image0(0,nj2*2-2,nk2-1),      image1(0,nj2-1,nk2-1)    ,1e-5);
  TEST_NEAR("LFB corner pixel",image0(0,nj2*2-2,0),          image1(0,nj2-1,0)        ,1e-5);
  TEST_NEAR("LNT corner pixel",image0(0,0,nk2-1),            image1(0,0,nk2-1)        ,1e-5);
}

// Check in-homogeneous smoothing option (ie onlj smooth in i,k but not j on some levels)
static void test_gauss_reduce_ik()
{
  vcl_cout << "**************************************\n"
           << " Testing vil3d_gauss_reduce_ik<float>\n"
           << "**************************************\n";

  unsigned ni = 10, nj = 20, nk = 30;
  vcl_cout<<"Image Size: "<<ni<<" x "<<nj<<" k "<<nk<<vcl_endl;

  vil3d_image_view<float> image0;
  image0.set_size(ni,nj,nk);

  for (unsigned k=0;k<image0.nk();++k)
    for (unsigned j=0;j<image0.nj();++j)
      for (unsigned i=0;i<image0.ni();++i)
        image0(i,j,k) = i*0.1f-j+k*10;

  vil3d_image_view<float> image1, work_im;
  vil3d_gauss_reduce_ik(image0,image1,work_im);

  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = nj;   // Shouldn't change first level
  unsigned nk2 = (nk+1)/2;
  TEST("Level 1 size i",image1.ni(),ni2);
  TEST("Level 1 size j",image1.nj(),nj2);
  TEST("Level 1 size k",image1.nk(),nk2);
  TEST_NEAR("Pixel (0,0,0)",image0(0,0,0),image1(0,0,0),1e-5);
  TEST_NEAR("Pixel (1,1,1)",image0(2,2,2),image1(1,2,1),1e-5);
  TEST_NEAR("Pixel (2,3,3)",image0(4,3,6),image1(2,3,3),1e-5);
  TEST_NEAR("RFT corner pixel",image0(ni2*2-2,nj2-1,nk2*2-2),image1(ni2-1,nj2-1,nk2-1),1e-5);
  TEST_NEAR("RFB corner pixel",image0(ni2*2-2,nj2-1,0),      image1(ni2-1,nj2-1,0)    ,1e-5);
  TEST_NEAR("RNT corner pixel",image0(ni2*2-2,0,nk2*2-2),    image1(ni2-1,0,nk2-1)    ,1e-5);
  TEST_NEAR("RNB corner pixel",image0(ni2*2-2,0,0),          image1(ni2-1,0,0)        ,1e-5);
  TEST_NEAR("LFT corner pixel",image0(0,nj2-1,nk2*2-2),      image1(0,nj2-1,nk2-1)    ,1e-5);
  TEST_NEAR("LFB corner pixel",image0(0,nj2-1,0),            image1(0,nj2-1,0)        ,1e-5);
  TEST_NEAR("LNT corner pixel",image0(0,0,nk2*2-2),          image1(0,0,nk2-1)        ,1e-5);
}

// Check in-homogeneous smoothing option (ie onlj smooth in j,k but not i on some levels)
static void test_gauss_reduce_jk()
{
  vcl_cout << "**************************************\n"
           << " Testing vil3d_gauss_reduce_jk<float>\n"
           << "**************************************\n";

  unsigned ni = 10, nj = 20, nk = 30;
  vcl_cout<<"Image Size: "<<ni<<" x "<<nj<<" k "<<nk<<vcl_endl;

  vil3d_image_view<float> image0;
  image0.set_size(ni,nj,nk);

  for (unsigned k=0;k<image0.nk();++k)
    for (unsigned j=0;j<image0.nj();++j)
      for (unsigned i=0;i<image0.ni();++i)
        image0(i,j,k) = i*0.1f-j+k*10;

  vil3d_image_view<float> image1, work_im;
  vil3d_gauss_reduce_jk(image0,image1,work_im);

  unsigned ni2 = ni;    // Shouldn't change first level
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = (nk+1)/2;
  TEST("Level 1 size i",image1.ni(),ni2);
  TEST("Level 1 size j",image1.nj(),nj2);
  TEST("Level 1 size k",image1.nk(),nk2);
  TEST_NEAR("Pixel (0,0,0)",image0(0,0,0),image1(0,0,0),1e-5);
  TEST_NEAR("Pixel (1,1,1)",image0(2,2,2),image1(2,1,1),1e-5);
  TEST_NEAR("Pixel (2,3,3)",image0(2,6,6),image1(2,3,3),1e-5);
  TEST_NEAR("RFT corner pixel",image0(ni2-1,nj2*2-2,nk2*2-2),image1(ni2-1,nj2-1,nk2-1),1e-5);
  TEST_NEAR("RFB corner pixel",image0(ni2-1,nj2*2-2,0),      image1(ni2-1,nj2-1,0)    ,1e-5);
  TEST_NEAR("RNT corner pixel",image0(ni2-1,0,nk2*2-2),      image1(ni2-1,0,nk2-1)    ,1e-5);
  TEST_NEAR("RNB corner pixel",image0(ni2-1,0,0),            image1(ni2-1,0,0)        ,1e-5);
  TEST_NEAR("LFT corner pixel",image0(0,nj2*2-2,nk2*2-2),    image1(0,nj2-1,nk2-1)    ,1e-5);
  TEST_NEAR("LFB corner pixel",image0(0,nj2*2-2,0),          image1(0,nj2-1,0)        ,1e-5);
  TEST_NEAR("LNT corner pixel",image0(0,0,nk2*2-2),          image1(0,0,nk2-1)        ,1e-5);
}


static void test_gauss_reduce_int()
{
  vcl_cout << "*********************************\n"
           << " Testing vil3d_gauss_reduce<int>\n"
           << "*********************************\n";

  unsigned ni = 10, nj = 20, nk = 30;
  vcl_cout<<"Image Size: "<<ni<<" x "<<nj<<" k "<<nk<<vcl_endl;

  vil3d_image_view<int> image0;
  image0.set_size(ni,nj,nk);

  for (unsigned k=0;k<image0.nk();++k)
    for (unsigned j=0;j<image0.nj();++j)
      for (unsigned i=0;i<image0.ni();++i)
        image0(i,j,k) = -100*static_cast<int>(i) + j + 10*k;

  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = (nk+1)/2;
  vil3d_image_view<int> image1,work_im1,work_im2;
  vil3d_gauss_reduce(image0,image1,work_im1,work_im2);

  TEST("size i",image1.ni(),ni2);
  TEST("size j",image1.nj(),nj2);
  TEST("size k",image1.nk(),nk2);

  TEST("Pixel (0,0,0)", image0(0,0,0), image1(0,0,0));
  TEST("Pixel (1,1,1)", image0(2,2,2), image1(1,1,1));
  TEST("Pixel (2,2,2)", image0(4,4,4), image1(2,2,2));
  TEST("Pixel (3,3,3)", image0(6,6,6), image1(3,3,3));
  TEST("RFT corner pixel",image0(ni2*2-2,nj2*2-2,nk2*2-2),image1(ni2-1,nj2-1,nk2-1));
  TEST("RFB corner pixel",image0(ni2*2-2,nj2*2-2,0),      image1(ni2-1,nj2-1,0));
  TEST("RNT corner pixel",image0(ni2*2-2,0,nk2*2-2),      image1(ni2-1,0,nk2-1));
  TEST("RNB corner pixel",image0(ni2*2-2,0,0),            image1(ni2-1,0,0));
  TEST("LFT corner pixel",image0(0,nj2*2-2,nk2*2-2),      image1(0,nj2-1,nk2-1));
  TEST("LFB corner pixel",image0(0,nj2*2-2,0),            image1(0,nj2-1,0));
  TEST("LNT corner pixel",image0(0,0,nk2*2-2),            image1(0,0,nk2-1));
}


static void test_gauss_reduce()
{
  test_gauss_reduce_float();
  test_gauss_reduce_ij();
  test_gauss_reduce_ik();
  test_gauss_reduce_jk();
  test_gauss_reduce_int();
}

TESTMAIN(test_gauss_reduce);
