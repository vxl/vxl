#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vnl/vnl_test.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_distance.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_gaussian_pyramid_builder_2d_general.h>
#include <mil/mil_image_pyramid.h>
#include <vil/vil_byte.h>



void test_gaussian_pyramid_builder_2d_general()
{
  int nx = 20, ny = 20;
  vcl_cout << "*******************************************************" << vcl_endl;
  vcl_cout << " Testing mil_gaussian_pyramid_builder_2d_general (byte)(nx="<<nx<<")" << vcl_endl;
  vcl_cout << "*******************************************************" << vcl_endl;


  mil_image_2d_of<vil_byte> image0;
  image0.resize(nx,ny);

#if(0) // use 2x2 chessboard pattern
  for (int y=0;y<ny/2;++y)
     for (int x=0;x<nx/2;++x)
     {
       image0(x,y) = 100;
       image0(x+nx/2,y+ny/2) = 100;
       image0(x,y+ny/2) = 200;
       image0(x+nx/2,y) = 200;
     }
#else // Use smooth plane pattern
  for (int y=0;y<image0.ny();++y)
     for (int x=0;x<image0.nx();++x)
     {
       image0(x,y) = x+y*10;
     }
#endif

  mil_gaussian_pyramid_builder_2d_general<vil_byte> builder;
  int default_n_levels = builder.maxLevels();
  builder.setMaxLevels(2);
  builder.set_scale_step(1.2);
  mil_image_pyramid image_pyr;

  builder.build(image_pyr,image0);

  vcl_cout<<"Result:"<<vcl_endl;
  image_pyr.print_all(vcl_cout);


  TEST("Found correct number of levels", image_pyr.n_levels(), 2);

  const mil_image_2d_of<vil_byte>& image1 = (const mil_image_2d_of<vil_byte>&) image_pyr(1);
  TEST("Level 1 size",image1.nx()==(int)(nx/1.2+0.5) && image1.ny()==(int)(ny/1.2+0.5), true);
  TEST("Pixel (0,0)",image1(0,0)==1,true);
  TEST("Pyramid(0) (12,12) = Pyramid(1) (11,10)",image0(12,12), image1(11,10));
  TEST("Corner pixel",image1(16,16)==208,true);

  builder.setMaxLevels(default_n_levels);
  builder.extend(image_pyr);
  vcl_cout<<"\n\n\nTesting builder.extend():"<<vcl_endl;
  image_pyr.print_all(vcl_cout);


  TEST("Found correct number of levels", image_pyr.n_levels(), 9);

  
}


TESTMAIN(test_gaussian_pyramid_builder_2d);
