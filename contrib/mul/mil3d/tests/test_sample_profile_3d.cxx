#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <testlib/testlib_test.h>
#include <vnl/vnl_vector.h>
#include <mil3d/mil3d_image_3d_of.h>
#include <mil3d/mil3d_sample_profile_3d.h>
#include <vcl_cmath.h> // for fabs()

void test_sample_profile_3d_float()
{
  vcl_cout << "*******************************\n";
  vcl_cout << " Testing mil3d_sample_profile_3d\n";
  vcl_cout << "*******************************\n";

  vcl_cout<< "One plane image"<<vcl_endl;
  mil3d_image_3d_of<float> image1;
  image1.resize(10,10,10);

  for (int y=0;y<image1.ny();++y)
   for (int x=0;x<image1.nx();++x)
     for (int z=0;z<image1.nz();++z)
     {
       image1(x,y,z) = x+y*10+z*100;
     }

  vgl_point_3d<double> p0(5,5,5);
  vgl_vector_3d<double> u0(1,0,0);
  vgl_vector_3d<double> u1(0.3,0.3,0.3);
  vnl_vector<double> vec;

  mil3d_sample_profile_3d(vec,image1,p0,u0,3);
  TEST("Profile correct length",vec.size(),3);
  TEST("First value",vcl_fabs(vec[0]-555)<1e-6,true);
  TEST("Last value",vcl_fabs(vec[2]-557)<1e-6,true);

  mil3d_sample_profile_3d(vec,image1,p0,u0,8);
  TEST("Profile correct length",vec.size(),8);
  TEST("First value",vcl_fabs(vec[0]-555)<1e-6,true);
  TEST("Last value (outside image)",vcl_fabs(vec[7]-0)<1e-6,true);

  mil3d_sample_profile_3d(vec,image1,p0,u1,2);
  TEST("(Diagonal) First value",vcl_fabs(vec[0]-555)<1e-6,true);
  TEST("(Diagonal) Last value",vcl_fabs(vec[1]-588.3)<1e-6,true);

  vcl_cout<< "Three plane image (float)"<<vcl_endl;
  mil3d_image_3d_of<float> image3;
  image3.resize(10,10,10,3);

  for (int z=0;z<image3.nz();++z)
    for (int y=0;y<image3.ny();++y)
     for (int x=0;x<image3.nx();++x)
       for (int k=0;k<3;++k)
       {
         image3(x,y,z,k) = x+y*10+100*z+1000*k;
       }

  mil3d_sample_profile_3d(vec,image3,p0,u0,3);
  TEST("Profile correct length",vec.size(),9);
  TEST("First value",vcl_fabs(vec[0]-555)<1e-6,true);
  TEST("Second value",vcl_fabs(vec[1]-1555)<1e-6,true);
  TEST("Last value",vcl_fabs(vec[8]-2557)<1e-6,true);

  mil3d_sample_profile_3d(vec,image3,p0,u0,8);
  TEST("Profile correct length",vec.size(),24);
  TEST("First value",vcl_fabs(vec[0]-555)<1e-6,true);
  TEST("Last value (outside image)",vcl_fabs(vec[23]-0)<1e-6,true);
}

void test_sample_profile_3d()
{
  test_sample_profile_3d_float();
}


TESTLIB_DEFINE_MAIN(test_sample_profile_3d);
