// This is mul/vimt3d/tests/test_gauss_reduce.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <vimt3d/vimt3d_gauss_reduce.h>


static void test_gauss_reduce_float()
{
  vcl_cout << "****************************\n"
           << " Testing vimt3d_gauss_reduce\n"
           << "****************************\n";

  unsigned ni = 20, nj = 20, nk = 20;
  vcl_cout<<"Image Size: "<<ni<<" x "<<nj<<" k "<<nk<<vcl_endl;

  vimt3d_image_3d_of<float> image0;
  image0.image().set_size(ni,nj,nk);

  for (unsigned k=0; k<image0.image().nk(); ++k)
    for (unsigned j=0; j<image0.image().nj(); ++j)
      for (unsigned i=0; i<image0.image().ni(); ++i)
        image0.image()(i,j,k) = i*0.1f+j+k*10;


  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = (nk+1)/2;
  vimt3d_image_3d_of<float> image1,work_im1,work_im2;
  vimt3d_gauss_reduce(image0,image1,work_im1,work_im2);
  TEST("size i",image1.image().ni(),(ni+1)/2);
  TEST("size j",image1.image().nj(),(nj+1)/2);
  TEST("size k",image1.image().nk(),(nk+1)/2);
  TEST_NEAR("Pixel (0,0,0)",image0.image()(0,0,0),image1.image()(0,0,0),1e-5);
  TEST_NEAR("Pixel (1,1,1)",image0.image()(2,2,2),image1.image()(1,1,1),1e-5);
  TEST_NEAR("Pixel (2,3,3)",image1.image()(2,3,3),66.4f,1e-4);
  TEST_NEAR("Corner pixel",image0.image()(ni2*2-2,nj2*2-2,nk2*2-2),image1.image()(ni2-1,nj2-1,nk2-1),1e-5);
}


static void test_gauss_reduce()
{
  test_gauss_reduce_float();
}

TESTMAIN(test_gauss_reduce);
