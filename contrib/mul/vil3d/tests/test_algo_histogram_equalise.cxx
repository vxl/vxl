
#include <testlib/testlib_test.h>
#include <vil3d/algo/vil3d_histogram_equalise.h>
#include <vil3d/vil3d_print.h>

static void test_histogram_equalise_byte()
{
  vil3d_image_view<vxl_byte> image(5,5,5);
  for (unsigned k=0;k<image.nk();++k)
    for (unsigned j=0;j<image.nj();++j)
      for (unsigned i=0;i<image.ni();++i)
        image(i,j,k) = i+j+k;
  

  vil3d_histogram_equalise(image);
  TEST("Equalised pixel (0,0,0)", image(0,0,0), 0);
  TEST("Equalised pixel (0,0,1)", image(0,0,1), 6);
  TEST("Equalised pixel (1,2,3)", image(1,2,3), 146);
  TEST("Equalised pixel (5,5,5)", image(4,4,4), 255);
  
  image.set_size(4,4,4);
  for (unsigned k=0;k<image.nk();++k)
    for (unsigned j=0;j<image.nj();++j)
      for (unsigned i=0;i<image.ni();++i)
        image(i,j,k) = i;

  vil3d_print_all(vcl_cout, image);
  
  
  vil3d_histogram_equalise(image);
  
  vil3d_print_all(vcl_cout, image);
  
  TEST("Equalised pixel (0,0,0)", image(0,0,0), 0);
  TEST("Equalised pixel (1,1,1)", image(1,1,1), 85);
  TEST("Equalised pixel (2,1,3)", image(2,1,3), 170);
  TEST("Equalised pixel (3,3,1)", image(3,3,1), 255);
}

static void test_algo_histogram_equalise()
{
  test_histogram_equalise_byte();
}

TESTMAIN(test_algo_histogram_equalise);
