// This is mul/vil3d/tests/test_gauss_reduce.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_print.h>
#include <vil3d/algo/vil3d_gauss_reduce.h>
#include <vil3d/vil3d_print.h>
#include <vil3d/vil3d_crop.h>


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
  
  TEST("size i",image1.ni(),(ni+1)/2);
  TEST("size j",image1.nj(),(nj+1)/2);
  TEST("size k",image1.nk(),(nk+1)/2);
  TEST_NEAR("Pixel (0,0,0)", image1(0,0,0), 4.5409f, 1e-4f);
  TEST_NEAR("Pixel (1,1,1)", image1(1,1,1), image0(2,2,2), 1e-5f);
  TEST_NEAR("Pixel (2,3,3)", image1(2,3,3), 54.4f, 1e-4f);
  TEST_NEAR("Corner pixel", image1(ni2-1,nj2-1,nk2-1),  258.2591f, 1e-4f);
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



  TEST("Level 1 size i", image1.ni(), ni2);
  TEST("Level 1 size j", image1.nj(), nj2);
  TEST("Level 1 size k", image1.nk(), nk2);
  TEST_NEAR("Pixel (0,0,0)", image1(0,0,0), -0.4491f, 1e-4f);
  TEST_NEAR("Pixel (1,1,1)", image1(1,1,2), image0(2,2,2), 1e-5f);
  TEST_NEAR("Pixel (2,3,3)", image1(2,3,3), 24.4f, 1e-4f);
  TEST_NEAR("Corner pixel", image1(ni2-1, nj2-1, nk2-1), 273.2491f, 1e-4f);
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
  TEST_NEAR("Pixel (0,0,0)", image1(0,0,0), 5.039900f, 1e-4f);
  TEST_NEAR("Pixel (1,1,1)", image1(1,2,1), image0(2,2,2), 1e-5f);
  TEST_NEAR("Pixel (2,3,3)", image1(2,3,3), 57.4f, 1e-4f);
  TEST_NEAR("Corner pixel", image1(ni2-1,nj2-1,nk2-1), 256.7601f, 1e-4f);
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
  TEST_NEAR("Pixel (0,0,0)", image1(0,0,0), 4.491f, 1e-4f);
  TEST_NEAR("Pixel (1,1,1)", image1(2,1,1), image0(2,2,2), 1e-5f);
  TEST_NEAR("Pixel (2,3,3)", image1(2,3,3), 54.2f, 1e-4f);
  TEST_NEAR("Corner pixel", image1(ni2-1,nj2-1,nk2-1), 258.409f , 1e-4f);
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

  vil3d_print_all(vcl_cout, vil3d_crop(image0, 0, 6, 0, 6, 0, 6));
  vil3d_print_all(vcl_cout, vil3d_crop(image1, 0, 3, 0, 3, 0, 3));



  TEST("size i",image1.ni(),ni2);
  TEST("size j",image1.nj(),nj2);
  TEST("size k",image1.nk(),nk2);

  TEST_NEAR("Pixel (0,0,0)", image1(0,0,0), -45, 0);
  TEST_NEAR("Pixel (1,1,1)", image1(1,1,1), image0(2,2,2), 0);
  TEST_NEAR("Pixel (2,2,2)", image1(2,2,2), image0(4,4,4), 0);
  TEST_NEAR("Pixel (3,3,3)", image1(3,3,3), image0(6,6,6), 0);
  TEST_NEAR("RFT corner pixel", image1(ni2-1,nj2-1,nk2-1), -457, 0);
  TEST_NEAR("RFB corner pixel", image1(ni2-1,nj2-1,0),     -727, 0);
  TEST_NEAR("RNT corner pixel", image1(ni2-1,0,nk2-1),     -475, 0);
  TEST_NEAR("RNB corner pixel", image1(ni2-1,0,0),         -745, 0);
  TEST_NEAR("LFT corner pixel", image1(0,nj2-1,nk2-1),     243, 0);
  TEST_NEAR("LFB corner pixel", image1(0,nj2-1,0),         -27, 0);
  TEST_NEAR("LNT corner pixel", image1(0,0,nk2-1),         225, 0);
}


static void test_gauss_reduce()
{
  vcl_cout << vcl_setprecision(10);
  test_gauss_reduce_float();
  test_gauss_reduce_ij();
  test_gauss_reduce_ik();
  test_gauss_reduce_jk();
  test_gauss_reduce_int();
}

TESTMAIN(test_gauss_reduce);
