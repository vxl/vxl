// This is mul/mil/tests/test_sample_profile_2d.cxx
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_sample_profile_2d.h>
#include <vxl_config.h>
#include <testlib/testlib_test.h>

void test_sample_profile_2d_byte()
{
  vcl_cout << "*******************************\n"
           << " Testing mil_sample_profile_2d\n"
           << "*******************************\n";

  vcl_cout<< "One plane image\n";
  mil_image_2d_of<vxl_byte> image1;
  image1.resize(10,10);

  for (int y=0;y<image1.ny();++y)
     for (int x=0;x<image1.nx();++x)
     {
       image1(x,y) = x+y*10;
     }

  vgl_point_2d<double> p0(5,5);
  vgl_vector_2d<double> u0(1,0);
  vnl_vector<double> vec;

  mil_sample_profile_2d(vec,image1,p0,u0,3);
  TEST("Profile correct length",vec.size(),3);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Last value",vec[2],57,1e-6);

  mil_sample_profile_2d(vec,image1,p0,u0,8);
  TEST("Profile correct length",vec.size(),8);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Last value (outside image)",vec[7],0,1e-6);

  vcl_cout<< "Three plane image (float)\n";
  mil_image_2d_of<float> image3;
  image3.resize(10,10,3);

  for (int y=0;y<image3.ny();++y)
     for (int x=0;x<image3.nx();++x)
       for (int k=0;k<3;++k)
         image3(x,y,k) = 0.1f*x+y+10*k;

  mil_sample_profile_2d(vec,image3,p0,u0,3);
  TEST("Profile correct length",vec.size(),9);
  TEST_NEAR("First value",vec[0],5.5f,1e-6);
  TEST_NEAR("Second value",vec[1],15.5f,1e-6);
  TEST_NEAR("Last value",vec[8],25.7f,1e-5);

  mil_sample_profile_2d(vec,image3,p0,u0,8);
  TEST("Profile correct length",vec.size(),24);
  TEST_NEAR("First value",vec[0],5.5f,1e-6);
  TEST_NEAR("Last value (outside image)",vec[23],0.f,1e-6);
}

void test_sample_profile_2d()
{
  test_sample_profile_2d_byte();
}


TESTLIB_DEFINE_MAIN(test_sample_profile_2d);
