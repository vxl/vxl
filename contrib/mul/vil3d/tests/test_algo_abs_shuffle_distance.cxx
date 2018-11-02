// This is mul/vil3d/tests/test_algo_abs_shuffle_distance.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/algo/vil3d_structuring_element.h>
#include <vil3d/algo/vil3d_abs_shuffle_distance.h>
#include <vil3d/vil3d_crop.h>
#include <vil3d/vil3d_math.h>

static void asd_fill_image(vil3d_image_view<int>& image)
{
  for (unsigned j=0;j<image.nj();++j)
    for (unsigned i=0;i<image.ni();++i)
      for (unsigned k=0;k<image.nk();++k)
        image(i,j,k)= int(i);
}

static void test_algo_abs_shuffle_distance()
{
  std::cout << "************************************\n"
           << " Testing vil3d_abs_shuffle_distance\n"
           << "************************************\n";

  unsigned Nx=50;
  unsigned Ny=45;
  unsigned Nz=55;
  vil3d_image_view<int> image0(Nx,Ny,Nz);
  vil3d_image_view<int> image1(Nx,Ny,Nz);
  int mean;

  vil3d_structuring_element se;
  se.set_to_sphere(1.5);
  std::cout<<"Structuring element: "<<se<<std::endl;

  asd_fill_image(image0);

  TEST_NEAR("Shuffle dist to self = 0",
            vil3d_abs_shuffle_distance(image0,image0,se),0.0,1e-6);

  vil3d_abs_shuffle_distance(image0,image0,se,image1);
  vil3d_math_mean( mean, image1, 0 );

  TEST_NEAR("Shuffle dist to self = 0 - image",
            mean,0.0,1e-6);


  TEST_NEAR("Shuffle dist to self = 0 without borders",
            vil3d_abs_shuffle_distance(image0,image0,se,false),0.0,1e-6);


  unsigned ni1=Nx-10, nj1=Ny-10, nk1=Nz-10;
  vil3d_image_view<int> sub_im1 = vil3d_crop(image0,2,ni1,2,nj1,2,nk1);
  vil3d_image_view<int> sub_im2 = vil3d_crop(image0,3,ni1,2,nj1,2,nk1);
  vil3d_image_view<int> sub_im3 = vil3d_crop(image0,4,ni1,2,nj1,2,nk1);
  image1.set_size(ni1,nj1,nk1);

  TEST_NEAR("Shuffle dist to self, 1 pixel displacement",
            vil3d_abs_shuffle_distance(sub_im1,sub_im2,se,false),0.0,1e-6);

  vil3d_abs_shuffle_distance(sub_im1,sub_im2,se,image1);
  vil3d_math_mean( mean, image1, 0 );
  TEST_NEAR("Shuffle dist to self, 1 pixel displacement - image",
            mean,0.0,1e-6);


  TEST_NEAR("Shuffle dist to self, 2 pixel displacement",
            vil3d_abs_shuffle_distance(sub_im1,sub_im3,se,false),1.0,1e-6);

  vil3d_image_view<int> image2(ni1,nj1,nk1);
  vil3d_abs_shuffle_distance(sub_im1,sub_im3,se,image2);
  vil3d_math_mean( mean, image2, 0 );
  TEST_NEAR("Shuffle dist to self, 2 pixel displacement - image",
            mean,1.0,1e-6);

  std::cout<<"Using larger radius: "<<std::endl;
  se.set_to_sphere(2.5);
  std::cout<<"Structuring element: "<<se<<std::endl;
  TEST_NEAR("Shuffle dist to self, 2 pixel displacement",
            vil3d_abs_shuffle_distance(sub_im1,sub_im3,se,false),0.0,1e-6);
}

TESTMAIN(test_algo_abs_shuffle_distance);
