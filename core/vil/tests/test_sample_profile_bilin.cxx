// This is mul/vil2/tests/test_sample_profile_bilin.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_sample_profile_bilin.h>

void test_sample_profile_bilin_byte()
{
  vcl_cout << "***********************************\n";
  vcl_cout << " Testing vil2_sample_profile_bilin\n";
  vcl_cout << "***********************************\n";

  vcl_cout<< "One plane image"<<vcl_endl;
  vil2_image_view<vxl_byte> image1;
  image1.resize(10,10);

  for (unsigned int y=0;y<image1.nj();++y)
     for (unsigned int x=0;x<image1.ni();++x)
       image1(x,y) = x+y*10;

  double x0 = 5.0, y0 = 5.0;
  double dx = 1.0, dy = 0.0;
  vcl_vector<double> vec(3);

  vil2_sample_profile_bilin(&vec[0],image1,x0,y0,dx,dy,3);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Last value",vec[2],57,1e-6);

  vcl_vector<double> vec2(8);
  vil2_sample_profile_bilin(&vec2[0],image1,x0,y0,dx,dy,8);
  TEST_NEAR("First value",vec2[0],55,1e-6);
  TEST_NEAR("Last value (outside image)",vec2[7],0,1e-6);

  vcl_cout<< "Three plane image (float)"<<vcl_endl;
  vil2_image_view<float> image3;
  image3.resize(10,10,3);

  for (unsigned int y=0;y<image3.nj();++y)
     for (unsigned int x=0;x<image3.ni();++x)
       for (unsigned int k=0;k<3;++k)
         image3(x,y,k) = 0.1f*x+y+10*k;

  vcl_vector<double> vec3(9);

  vil2_sample_profile_bilin(&vec3[0],image3,x0,y0,dx,dy,3);
  TEST_NEAR("First value",vec3[0],5.5f,1e-6);
  TEST_NEAR("Second value",vec3[1],15.5f,1e-6);
  TEST_NEAR("Last value",vec3[8],25.7f,1e-6);

  vcl_vector<double> vec4(24);
  vil2_sample_profile_bilin(&vec4[0],image3,x0,y0,dx,dy,8);
  TEST_NEAR("First value",vec4[0],5.5f,1e-6);
  TEST_NEAR("Last value (outside image)",vec4[23],0.f,1e-6);
}

MAIN( test_sample_profile_bilin )
{
  START( "Bilinear Profile Sampling" );

  test_sample_profile_bilin_byte();

  SUMMARY();
}
