// This is mul/mil/tests/test_sample_grid_2d.cxx
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_sample_grid_2d.h>
#include <vxl_config.h>
#include <testlib/testlib_test.h>

void test_sample_grid_2d_byte()
{
  vcl_cout << "****************************\n"
           << " Testing mil_sample_grid_2d\n"
           << "****************************\n";

  mil_image_2d_of<vxl_byte> image0;
  image0.resize(10,10);

  for (int y=0;y<image0.ny();++y)
     for (int x=0;x<image0.nx();++x)
     {
       image0(x,y) = x+y*10;
     }

  vgl_point_2d<double> p0(5,5);
  vgl_vector_2d<double> u0(1,0);
  vgl_vector_2d<double> v0(0,1);
  vnl_vector<double> vec;

  mil_sample_grid_2d(vec,image0,p0,u0,v0,3,3);
  TEST("Profile correct length",vec.size(),9);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Third value",vec[2],75,1e-6);
  TEST_NEAR("Last value",vec[8],77,1e-6);

  mil_sample_grid_2d(vec,image0,p0,v0,u0,3,3);
  TEST("Profile correct length",vec.size(),9);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Third value",vec[2],57,1e-6);
  TEST_NEAR("Last value",vec[8],77,1e-6);
}

void test_sample_grid_2d()
{
  test_sample_grid_2d_byte();
}


TESTLIB_DEFINE_MAIN(test_sample_grid_2d);
