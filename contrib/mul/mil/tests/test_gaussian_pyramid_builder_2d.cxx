#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vnl/vnl_test.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_distance.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_gaussian_pyramid_builder_2d.h>
#include <mil/mil_image_pyramid.h>
#include <vil/vil_byte.h>


void test_gaussian_pyramid_builder_2d_byte(int nx, int ny)
{
  vcl_cout << "*************************" << vcl_endl;
  vcl_cout << " Testing mil_gaussian_pyramid_builder_2d (byte)(nx="<<nx<<")" << vcl_endl;
  vcl_cout << "*************************" << vcl_endl;

  mil_image_2d_of<vil_byte> image0;
  image0.resize(nx,ny);

  for (int y=0;y<image0.ny();++y)
     for (int x=0;x<image0.nx();++x)
     {
       image0(x,y) = x+y*10;
     }

  mil_gaussian_pyramid_builder_2d<vil_byte> builder;
  mil_image_pyramid image_pyr;

  builder.build(image_pyr,image0);

  vcl_cout<<"Result:"<<vcl_endl;
  image_pyr.print_all(vcl_cout);

  if (image_pyr.nLevels()>0)
  {
    int nx2 = (nx+1)/2;
    int ny2 = (ny+1)/2;
    const mil_image_2d_of<vil_byte>& image1 = (const mil_image_2d_of<vil_byte>&) image_pyr(1);
    TEST("Level 1 size",image1.nx()==(nx+1)/2 && image1.ny()==(ny+1)/2, true);
    TEST("Pixel (0,0)",image0(0,0)==image1(0,0),true);
    TEST("Pixel (1,1)",image0(2,2)==image1(1,1),true);
    TEST("Corner pixel",image0(nx2*2-2,ny2*2-2)==image1(nx2-1,ny2-1),true);
  }
}

void test_gaussian_pyramid_builder_2d()
{
  test_gaussian_pyramid_builder_2d_byte(10,10);
}


TESTMAIN(test_gaussian_pyramid_builder_2d);
