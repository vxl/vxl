#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vnl/vnl_test.h>
#include <vnl/vnl_vector.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_sample_profile_2d.h>
#include <vil/vil_byte.h>
#include <vcl_cmath.h> // for fabs()

void test_sample_profile_2d_byte()
{
  vcl_cout << "*******************************\n";
  vcl_cout << " Testing mil_sample_profile_2d\n";
  vcl_cout << "*******************************\n";

  mil_image_2d_of<vil_byte> image0;
  image0.resize(10,10);

  for (int y=0;y<image0.ny();++y)
     for (int x=0;x<image0.nx();++x)
     {
       image0(x,y) = x+y*10;
     }

  vgl_point_2d<double> p0(5,5);
  vgl_vector_2d<double> u0(1,0);
  vnl_vector<double> vec;

  mil_sample_profile_2d(vec,image0,p0,u0,3);
  TEST("Profile correct length",vec.size(),3);
  TEST("First value",vcl_fabs(vec[0]-55)<1e-6,true);
  TEST("Last value",vcl_fabs(vec[2]-57)<1e-6,true);

  mil_sample_profile_2d(vec,image0,p0,u0,8);
  TEST("Profile correct length",vec.size(),8);
  TEST("First value",vcl_fabs(vec[0]-55)<1e-6,true);
  TEST("Last value (outside image)",vcl_fabs(vec[7]-0)<1e-6,true);
}

void test_sample_profile_2d()
{
  test_sample_profile_2d_byte();
}


TESTMAIN(test_sample_profile_2d);
