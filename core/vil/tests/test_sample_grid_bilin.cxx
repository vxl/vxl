// This is mul/vil2/tests/test_sample_grid_bilin.cxx
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_sample_grid_bilin.h>
#include <vil2/vil2_byte.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>

#include <testlib/testlib_test.h>

void test_sample_grid_bilin_byte()
{
  vcl_cout << "*******************************\n"
           << " Testing vil2_sample_grid_bilin\n"
           << "*******************************\n";

  vil2_image_view<vil2_byte> image0;
  image0.resize(10,10);

  vcl_cout<<"Testing one plane image"<<vcl_endl;

  for (int y=0;y<image0.nj();++y)
     for (int x=0;x<image0.ni();++x)
       image0(x,y) = x+y*10;

  double x0 = 5.0, y0= 5.0;
  double dx1 = 1.0, dy1 = 0.0;
  double dx2 = 0.0, dy2 = 1.0;
  vcl_vector<double> vec(12);

  vcl_cout<<"Fully in image"<<vcl_endl;
  vil2_sample_grid_bilin(&vec[0],image0,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Third value",vec[2],75,1e-6);
  TEST_NEAR("Last value",vec[11],78,1e-6);

  vil2_sample_grid_bilin(&vec[0],image0,x0,y0,dx2,dy2,dx1,dy1,4,3);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Third value",vec[2],57,1e-6);
  TEST_NEAR("Last value",vec[11],87,1e-6);

  vcl_cout<<"Beyond edge of image"<<vcl_endl;
  x0 = 8;
  vil2_sample_grid_bilin(&vec[0],image0,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST_NEAR("First value",vec[0],58,1e-6);
  TEST_NEAR("Last value",vec[11],0,1e-6);

  vcl_cout<<"Testing three plane image"<<vcl_endl;

  image0.resize(10,10,2);
  for (int y=0;y<image0.nj();++y)
    for (int x=0;x<image0.ni();++x)
      for (int p=0;p<2;++p)
       image0(x,y,p) = x+y*10+p*100;

  vcl_vector<double> vec2(24);

  vcl_cout<<"Fully in image"<<vcl_endl;
  x0 = 5.0;
  vil2_sample_grid_bilin(&vec2[0],image0,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST_NEAR("First value",vec2[0],55,1e-6);
  TEST_NEAR("Second value",vec2[1],155,1e-6);
  TEST_NEAR("Third value",vec2[2],65,1e-6);
  TEST_NEAR("Last value",vec2[23],178,1e-6);

  vcl_cout<<"Beyond edge of image"<<vcl_endl;
  x0 = 8;
  vil2_sample_grid_bilin(&vec2[0],image0,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST_NEAR("First value",vec2[0],58,1e-6);
  TEST_NEAR("Last value",vec2[23],0,1e-6);
}

MAIN( test_sample_grid_bilin )
{
  START( "Bilinear Grid Sampling" );

  test_sample_grid_bilin_byte();

  SUMMARY();
}
