// This is mul/vil3d/tests/test_sample_profile_trilin.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_sample_profile_trilin.h>
#include <vcl_vector.h>

static void test_sample_profile_trilin_float()
{
  vcl_cout << "*************************************\n"
           << " Testing vil3d_sample_profile_trilin\n"
           << "*************************************\n";

  vcl_cout<< "One plane image\n";
  vil3d_image_view<float> image1;
  image1.set_size(10,10,10);

  for (unsigned y=0;y<image1.nj();++y)
   for (unsigned x=0;x<image1.ni();++x)
     for (unsigned z=0;z<image1.nk();++z)
       image1(x,y,z) = x*0.1f+y+z*10;

  double x0=5,y0=5,z0=5;
  double dx0=1,dy0=0,dz0=0;
  double dx1=0.3,dy1=0.3,dz1=0.3;

  vcl_vector<double> vec(100);
  vil3d_sample_profile_trilin(&vec[0],image1,x0,y0,z0,dx0,dy0,dz0,3);
  TEST_NEAR("First value",vec[0],55.5f,1e-6);
  TEST_NEAR("Last value",vec[2],55.7f,1e-5);

  vil3d_sample_profile_trilin(&vec[0],image1,x0,y0,z0,dx0,dy0,dz0,8);
  TEST_NEAR("First value",vec[0],55.5f,1e-6);
  TEST_NEAR("Last value (outside image)",vec[7],0,1e-6);

  vil3d_sample_profile_trilin(&vec[0],image1,x0,y0,z0,dx1,dy1,dz1,2);
  TEST_NEAR("(Diagonal) First value",vec[0],55.5f,1e-6);
  TEST_NEAR("(Diagonal) Last value",vec[1],58.83f,1e-5);

  vcl_cout<< "Three plane image (float)\n";
  vil3d_image_view<float> image3;
  image3.set_size(10,10,10,3);

  for (unsigned z=0;z<image3.nk();++z)
    for (unsigned y=0;y<image3.nj();++y)
     for (unsigned x=0;x<image3.ni();++x)
       for (unsigned k=0;k<3;++k)
         image3(x,y,z,k) = 0.1f*x+y+10*z+100*k;

  vil3d_sample_profile_trilin(&vec[0],image3,x0,y0,z0,dx0,dy0,dz0,3);
  TEST_NEAR("First value",vec[0],55.5f,1e-6);
  TEST_NEAR("Second value",vec[1],155.5f,1e-6);
  TEST_NEAR("Last value",vec[8],255.7f,1e-6);

  vil3d_sample_profile_trilin(&vec[0],image3,x0,y0,z0,dx0,dy0,dz0,8);
  TEST_NEAR("First value",vec[0],55.5f,1e-6);
  TEST_NEAR("Last value (outside image)",vec[23],0,1e-6);
}

static void test_sample_profile_trilin()
{
  test_sample_profile_trilin_float();
}

TESTMAIN(test_sample_profile_trilin);
