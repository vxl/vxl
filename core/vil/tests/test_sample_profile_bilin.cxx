// This is core/vil/tests/test_sample_profile_bilin.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vil/vil_sample_profile_bilin.h>

static void test_sample_profile_bilin_byte()
{
  vcl_cout << "**********************************\n"
           << " Testing vil_sample_profile_bilin\n"
           << "**********************************\n";

  vcl_cout<< "One plane image\n";
  vil_image_view<vxl_byte> image1;
  image1.set_size(10,10);

  for (unsigned int j=0;j<image1.nj();++j)
     for (unsigned int i=0;i<image1.ni();++i)
       image1(i,j) = i+j*10;

  double x0 = 5.0, y0 = 5.0;
  double dx = 1.0, dy = 0.0;
  vcl_vector<double> vec(3);

  vil_sample_profile_bilin(&vec[0],image1,x0,y0,dx,dy,3);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Last value",vec[2],57,1e-6);

  vcl_vector<double> vec2(8);
  vil_sample_profile_bilin(&vec2[0],image1,x0,y0,dx,dy,8);
  TEST_NEAR("First value",vec2[0],55,1e-6);
  TEST_NEAR("Last value (outside image)",vec2[7],0,1e-6);

  vcl_cout<< "Three plane image (float)\n";
  vil_image_view<float> image3;
  image3.set_size(10,10,3);

  for (unsigned int j=0;j<image3.nj();++j)
     for (unsigned int i=0;i<image3.ni();++i)
       for (unsigned int k=0;k<3;++k)
         image3(i,j,k) = 0.1f*i+j+10*k;

  vcl_vector<double> vec3(9);

  vil_sample_profile_bilin(&vec3[0],image3,x0,y0,dx,dy,3);
  TEST_NEAR("First value",vec3[0],5.5f,1e-6);
  TEST_NEAR("Second value",vec3[1],15.5f,1e-6);
  TEST_NEAR("Last value",vec3[8],25.7f,1e-5);

  vcl_vector<double> vec4(24);
  vil_sample_profile_bilin(&vec4[0],image3,x0,y0,dx,dy,8);
  TEST_NEAR("First value",vec4[0],5.5f,1e-6);
  TEST_NEAR("Last value (outside image)",vec4[23],0.f,1e-6);
}

static void test_sample_profile_bilin()
{
  test_sample_profile_bilin_byte();
}

TESTMAIN(test_sample_profile_bilin);
