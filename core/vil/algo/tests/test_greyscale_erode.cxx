// This is core/vil/algo/tests/test_greyscale_erode.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/algo/vil_greyscale_erode.h>
#include <vil/vil_print.h>

static void test_greyscale_erode_byte()
{
  vcl_cout << "*****************************\n"
           << " Testing vil_greyscale_erode\n"
           << "*****************************\n";

  vil_image_view<vxl_byte> image0;
  image0.set_size(10,10);
  image0.fill(false);

  image0(4,5)=4;  // Central pixel
  image0(5,5)=5;  // Central pixel
  image0(6,5)=5;  // Central pixel
  image0(3,0)=5;  // Edge pixel
  vcl_cout<<"Original image\n";
  vil_print_all(vcl_cout,image0);

  vil_structuring_element element_i,element_j;
  element_i.set_to_line_i(-1,1);
  element_j.set_to_line_j(-1,1);
  vcl_cout<<"Structuring element: "<<element_i<<'\n';

  vil_image_view<vxl_byte> image1;
  vil_greyscale_erode(image0,image1,element_i);
  vcl_cout<<"Result of one erosion\n";
  vil_print_all(vcl_cout,image1);
  TEST("image1(3,5)", image1(3,5), 0);
  TEST("image1(5,5)", image1(5,5), 4);
  TEST("image1(4,5)", image1(4,5), 0);
  TEST("image1(6,5)", image1(6,5), 0);
  TEST("image1(5,6)", image1(5,6), 0);
  TEST("image1(2,0)", image1(2,0), 0);

  vil_image_view<vxl_byte> image2;
  vil_greyscale_erode(image1,image2,element_j);
  vcl_cout<<"Result of two erosions\n";
  vil_print_all(vcl_cout,image2);
  TEST("image2(5,5)", image2(5,5), 0);
  TEST("image2(4,5)", image2(4,5), 0);
  TEST("image2(6,5)", image2(6,5), 0);
  TEST("image2(5,6)", image2(5,6), 0);
  TEST("image2(5,4)", image2(5,4), 0);
  TEST("image2(5,0)", image2(5,0), 0);
  TEST("image2(5,9)", image2(5,9), 0);
  TEST("image2(2,0)", image2(2,0), 0);
  TEST("image2(2,1)", image2(2,1), 0);
}

static void test_greyscale_erode()
{
  test_greyscale_erode_byte();
}

TESTMAIN(test_greyscale_erode);
