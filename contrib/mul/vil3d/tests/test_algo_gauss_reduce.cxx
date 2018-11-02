// This is mul/vil3d/tests/test_algo_gauss_reduce.cxx
#include <iostream>
#include <iomanip>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/vil3d_image_view.h>
#include <vil3d/algo/vil3d_gauss_reduce.h>
#include <vil3d/vil3d_print.h>
#include <vil3d/vil3d_crop.h>

static void test_algo_gauss_reduce_float(vil3d_image_view<float>& image,
                                         vil3d_image_view<float>& dest)
{
  unsigned ni = image.ni(), nj = image.nj(), nk = image.nk(), np=image.nplanes();
  std::cout<<"Image Size: "<<ni<<" x "<<nj<<" x "<<nk<< " x " << np << "planes" << std::endl;


  for (unsigned p=0;p<image.nplanes();++p)
    for (unsigned k=0;k<nk;++k)
      for (unsigned j=0;j<nj;++j)
        for (unsigned i=0;i<ni;++i)
          image(i,j,k,p) = i*0.1f-j+k*10 + p*100;

  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = (nk+1)/2;
  vil3d_image_view<float> work_im1,work_im2;
  float old_image222 = image(2,2,2);
  vil3d_gauss_reduce(image,dest,work_im1,work_im2);

  TEST("size i", dest.ni(),(ni+1)/2);
  TEST("size j", dest.nj(),(nj+1)/2);
  TEST("size k", dest.nk(),(nk+1)/2);
  TEST("nplanes", dest.nplanes(), np);
  for (unsigned p=0; p<np; ++p)
  {
    std::cout << "\n plane " << p << std::endl;
    TEST_NEAR("Pixel (0,0,0)", dest(0,0,0,p), 4.5409f + p*100.0f, 1e-4f);
    TEST_NEAR("Pixel (1,1,1)", dest(1,1,1,p), old_image222 + p*100.0f, 1e-4f);
    TEST_NEAR("Pixel (2,3,3)", dest(2,3,3,p), 54.4f + p*100.0f, 1e-4f);
    TEST_NEAR("Corner pixel", dest(ni2-1,nj2-1,nk2-1,p),  258.2591f + p*100.0f, 1e-4f);
  }
}

// Check in-homogeneous smoothing option (ie onlj smooth in i,j but not k on some levels)
static void test_algo_gauss_reduce_ij(unsigned np)
{
  std::cout << "**************************************\n"
           << " Testing vil3d_gauss_reduce_ij<float>\n"
           << "**************************************\n";

  unsigned ni = 10, nj = 20, nk = 30;
  std::cout<<"Image Size: "<<ni<<" x "<<nj<<" x "<<nk<< " x " << np << "planes" << std::endl;

  vil3d_image_view<float> image0;
  image0.set_size(ni, nj, nk, np);

  for (unsigned p=0;p<image0.nplanes();++p)
    for (unsigned k=0;k<image0.nk();++k)
      for (unsigned j=0;j<image0.nj();++j)
        for (unsigned i=0;i<image0.ni();++i)
          image0(i,j,k,p) = i*0.1f-j+k*10 + p*100;

  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = nk;  // Shouldn't change first level

  vil3d_image_view<float> image1, work_im;
  vil3d_gauss_reduce_ij(image0,image1,work_im);

  TEST("Level 1 size i", image1.ni(), ni2);
  TEST("Level 1 size j", image1.nj(), nj2);
  TEST("Level 1 size k", image1.nk(), nk2);
  TEST("Level 1 size p", image1.nplanes(), np);

  for (unsigned p=0; p<np; ++p)
  {
    std::cout << "\n plane " << p << std::endl;
    TEST_NEAR("Pixel (0,0,0)", image1(0,0,0,p), -0.4491f + p*100.0, 1e-4f);
    TEST_NEAR("Pixel (1,1,1)", image1(1,1,2,p), image0(2,2,2) + p*100.0, 1e-4f);
    TEST_NEAR("Pixel (2,3,3)", image1(2,3,3,p), 24.4f + p*100.0, 1e-4f);
    TEST_NEAR("Corner pixel", image1(ni2-1, nj2-1, nk2-1,p), 273.2491f + p*100.0, 1e-4f);
  }
}

// Check in-homogeneous smoothing option (ie onlj smooth in i,k but not j on some levels)
static void test_algo_gauss_reduce_ik(unsigned np)
{
  std::cout << "**************************************\n"
           << " Testing vil3d_gauss_reduce_ik<float>\n"
           << "**************************************\n";

  unsigned ni = 10, nj = 20, nk = 30;
  std::cout<<"Image Size: "<<ni<<" x "<<nj<<" x "<<nk<< " x " << np << "planes" << std::endl;

  vil3d_image_view<float> image0;
  image0.set_size(ni,nj,nk, np);

  for (unsigned p=0;p<image0.nplanes();++p)
    for (unsigned k=0;k<image0.nk();++k)
      for (unsigned j=0;j<image0.nj();++j)
        for (unsigned i=0;i<image0.ni();++i)
          image0(i,j,k,p) = i*0.1f-j+k*10 + p*100;

  vil3d_image_view<float> image1, work_im;
  vil3d_gauss_reduce_ik(image0,image1,work_im);

  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = nj;   // Shouldn't change first level
  unsigned nk2 = (nk+1)/2;
  TEST("Level 1 size i", image1.ni(),ni2);
  TEST("Level 1 size j", image1.nj(),nj2);
  TEST("Level 1 size k", image1.nk(),nk2);
  TEST("Level 1 size p", image1.nplanes(), np);

  for (unsigned p=0; p<np; ++p)
  {
    std::cout << "\n plane " << p << std::endl;
    TEST_NEAR("Pixel (0,0,0)", image1(0,0,0,p), 5.039900f + p*100.0, 1e-4f);
    TEST_NEAR("Pixel (1,1,1)", image1(1,2,1,p), image0(2,2,2) + p*100.0, 1e-4f);
    TEST_NEAR("Pixel (2,3,3)", image1(2,3,3,p), 57.4f + p*100.0, 1e-4f);
    TEST_NEAR("Corner pixel", image1(ni2-1,nj2-1,nk2-1,p), 256.7601f + p*100.0, 1e-4f);
  }
}

// Check in-homogeneous smoothing option (ie onlj smooth in j,k but not i on some levels)
static void test_algo_gauss_reduce_jk(unsigned np)
{
  std::cout << "**************************************\n"
           << " Testing vil3d_gauss_reduce_jk<float>\n"
           << "**************************************\n";

  unsigned ni = 10, nj = 20, nk = 30;
  std::cout<<"Image Size: "<<ni<<" x "<<nj<<" x "<<nk<< " x " << np << "planes" << std::endl;

  vil3d_image_view<float> image0;
  image0.set_size(ni,nj,nk, np);

  for (unsigned p=0;p<image0.nplanes();++p)
    for (unsigned k=0;k<image0.nk();++k)
      for (unsigned j=0;j<image0.nj();++j)
        for (unsigned i=0;i<image0.ni();++i)
          image0(i,j,k,p) = i*0.1f-j+k*10 + p*100;

  vil3d_image_view<float> image1, work_im;
  vil3d_gauss_reduce_jk(image0,image1,work_im);

  unsigned ni2 = ni;    // Shouldn't change first level
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = (nk+1)/2;
  TEST("Level 1 size i", image1.ni(),ni2);
  TEST("Level 1 size j", image1.nj(),nj2);
  TEST("Level 1 size k", image1.nk(),nk2);
  TEST("Level 1 size p", image1.nplanes(), np);

  for (unsigned p=0; p<np; ++p)
  {
    std::cout << "\n plane " << p << std::endl;
    TEST_NEAR("Pixel (0,0,0)", image1(0,0,0,p), 4.491f + p*100.0, 1e-4f);
    TEST_NEAR("Pixel (1,1,1)", image1(2,1,1,p), image0(2,2,2) + p*100.0, 1e-4f);
    TEST_NEAR("Pixel (2,3,3)", image1(2,3,3,p), 54.2f + p*100.0, 1e-4f);
    TEST_NEAR("Corner pixel", image1(ni2-1,nj2-1,nk2-1,p), 258.409f + p*100.0, 1e-4f);
  }
}

static void test_algo_gauss_reduce_int()
{
  std::cout << "*********************************\n"
           << " Testing vil3d_gauss_reduce<int>\n"
           << "*********************************\n";

  unsigned ni = 10, nj = 20, nk = 30;
  std::cout<<"Image Size: "<<ni<<" x "<<nj<<" k "<<nk<<std::endl;

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

  vil3d_print_all(std::cout, vil3d_crop(image0, 0, 6, 0, 6, 0, 6));
  vil3d_print_all(std::cout, vil3d_crop(image1, 0, 3, 0, 3, 0, 3));

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

static void test_algo_gauss_reduce()
{
  std::cout << "***********************************\n"
           << " Testing vil3d_gauss_reduce<float>\n"
           << "***********************************\n"
           << std::setprecision(10);

  vil3d_image_view<float> image(10, 20, 30);
  vil3d_image_view<float> dest;

  test_algo_gauss_reduce_float(image, dest);
  std::cout<<"Test non-contiguous image\n";
  vil3d_image_view<float> image2(20, 30, 41);
  vil3d_image_view<float> crop_image = vil3d_crop(image2,2,10,3,20,4,30);
  test_algo_gauss_reduce_float(crop_image, dest);
  std::cout<<"Test input image = output_image\n";
  test_algo_gauss_reduce_float(image, image);

  vil3d_image_view<float> image3(10, 20, 30,3);
  std::cout<<"Test multiplane image\n";
  test_algo_gauss_reduce_float(image3, dest);

  test_algo_gauss_reduce_ij(1);
  test_algo_gauss_reduce_ik(1);
  test_algo_gauss_reduce_jk(1);

  test_algo_gauss_reduce_ij(2);
  test_algo_gauss_reduce_ik(2);
  test_algo_gauss_reduce_jk(2);

  test_algo_gauss_reduce_int();
}

TESTMAIN(test_algo_gauss_reduce);
