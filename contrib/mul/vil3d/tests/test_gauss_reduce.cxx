// This is mul/vil2/tests/test_image_view.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_functional.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/algo/vil3d_gauss_reduce.h>

void test_gauss_reduce_float()
{
  vcl_cout << "*****************************\n";
  vcl_cout << " Testing vil3d_gauss_reduce\n";
  vcl_cout << "*****************************\n";

  int ni = 20, nj = 20, nk = 20;
  vcl_cout<<"Image Size: "<<ni<<" x "<<nj<<" z "<<nk<<vcl_endl;

  vil3d_image_view<float> image0;
  image0.resize(ni,nj,nk);

  for (int z=0;z<image0.nk();++z)
    for (int y=0;y<image0.nj();++y)
      for (int x=0;x<image0.ni();++x)
        image0(x,y,z) = x*0.1f+y+z*10;


  int ni2 = (ni+1)/2;
  int nj2 = (nj+1)/2;
  int nk2 = (nk+1)/2;
  vil3d_image_view<float> image1,work_im1,work_im2;
  vil3d_gauss_reduce(image0,image1,work_im1,work_im2);
  TEST("size x",image1.ni(),(ni+1)/2);
  TEST("size y",image1.nj(),(nj+1)/2);
  TEST("size z",image1.nk(),(nk+1)/2);
  TEST_NEAR("Pixel (0,0,0)",image0(0,0,0),image1(0,0,0),1e-6);
  TEST_NEAR("Pixel (1,1,1)",image0(2,2,2),image1(1,1,1),1e-6);
  TEST_NEAR("Pixel (2,3,3)",image1(2,3,3),66.4f,1e-6);
  TEST_NEAR("Corner pixel",image0(ni2*2-2,nj2*2-2,nk2*2-2),image1(ni2-1,nj2-1,nk2-1),1e-6);
}

MAIN( test_gauss_reduce )
{
  START( "vil3d_gauss_reduce" );
  test_gauss_reduce_float();

  SUMMARY();
}
