// This is core/vil/tests/test_sample_grid_bicub.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vil/vil_sample_grid_bicub.h>

static void test_sample_grid_bicub_byte()
{
  vcl_cout << "*******************************\n"
           << " Testing vil_sample_grid_bicub\n"
           << "*******************************\n";

  vil_image_view<vxl_byte> image0;
  image0.set_size(10,10);

  vcl_cout<<"Testing one plane image\n";

  for (unsigned int j=0;j<image0.nj();++j)
     for (unsigned int i=0;i<image0.ni();++i)
       image0(i,j) = i+j*10;

  double x0 = 5.0, y0= 5.0;
  double dx1 = 1.0, dy1 = 0.0;
  double dx2 = 0.0, dy2 = 1.0;
  vcl_vector<double> vec(12);

  vcl_cout<<"Fully in image\n";
  vil_sample_grid_bicub(&vec[0],image0,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Third value",vec[2],75,1e-6);
  TEST_NEAR("Last value",vec[11],78,1e-6);

  vil_sample_grid_bicub(&vec[0],image0,x0,y0,dx2,dy2,dx1,dy1,4,3);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Third value",vec[2],57,1e-6);
  TEST_NEAR("Last value",vec[11],87,1e-6);

  vcl_cout<<"Beyond edge of image\n";
  x0 = 8;
  vil_sample_grid_bicub(&vec[0],image0,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST_NEAR("First value",vec[0],58,1e-6);
  TEST_NEAR("Last value",vec[11],0,1e-6);

  vcl_cout<<"Testing three plane image\n";

  image0.set_size(10,10,2);
  for (unsigned int j=0;j<image0.nj();++j)
    for (unsigned int i=0;i<image0.ni();++i)
      for (unsigned int p=0;p<2;++p)
       image0(i,j,p) = i+j*10+p*100;

  vcl_vector<double> vec2(24);

  vcl_cout<<"Fully in image\n";
  x0 = 5.0;
  vil_sample_grid_bicub(&vec2[0],image0,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST_NEAR("First value",vec2[0],55,1e-6);
  TEST_NEAR("Second value",vec2[1],155,1e-6);
  TEST_NEAR("Third value",vec2[2],65,1e-6);
  TEST_NEAR("Last value",vec2[23],178,1e-6);

  vcl_cout<<"Beyond edge of image\n";
  x0 = 8;
  vil_sample_grid_bicub(&vec2[0],image0,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST_NEAR("First value",vec2[0],58,1e-6);
  TEST_NEAR("Last value",vec2[23],0,1e-6);
}

static void test_sample_grid_bicub()
{
  test_sample_grid_bicub_byte();
}

TESTMAIN(test_sample_grid_bicub);
