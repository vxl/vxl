// This is core/vil/algo/tests/test_binary_dilate.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/algo/vil_binary_dilate.h>

static inline void print_binary_image(const vil_image_view<bool>& im)
{
  for (unsigned j=0;j<im.nj();++j)
  {
    for (unsigned i=0;i<im.ni();++i)
      if (im(i,j)) vcl_cout<<'X';
      else         vcl_cout<<'.';
    vcl_cout<<vcl_endl;
  }
}

static void test_binary_dilate_byte()
{
  vcl_cout << "***************************\n"
           << " Testing vil_binary_dilate\n"
           << "***************************\n";

  vil_image_view<bool> image0;
  image0.set_size(10,10);
  image0.fill(false);

  image0(5,5)=true;  // Central pixel
  image0(3,0)=true;  // Edge pixel
  vcl_cout<<"Original image\n";
  print_binary_image(image0);

  vil_structuring_element element_i,element_j;
  element_i.set_to_line_i(-1,1);
  element_j.set_to_line_j(-1,1);
  vcl_cout<<"Structuring element: "<<element_i<<vcl_endl;

  vil_image_view<bool> image1;
  vil_binary_dilate(image0,image1,element_i);
  vcl_cout<<"Result of one dilation\n";
  print_binary_image(image1);
  TEST("image1(5,5)", image1(5,5), true);
  TEST("image1(4,5)", image1(4,5), true);
  TEST("image1(6,5)", image1(6,5), true);
  TEST("image1(5,6)", image1(5,6), false);
  TEST("image1(2,0)", image1(2,0), true);

  vil_image_view<bool> image2;
  vil_binary_dilate(image1,image2,element_j);
  vcl_cout<<"Result of two dilations\n";
  print_binary_image(image2);
  TEST("image2(5,5)", image2(5,5), true);
  TEST("image2(4,5)", image2(4,5), true);
  TEST("image2(6,5)", image2(6,5), true);
  TEST("image2(5,6)", image2(5,6), true);
  TEST("image2(5,4)", image2(5,4), true);
  TEST("image2(5,0)", image2(5,0), false);
  TEST("image2(5,9)", image2(5,9), false);
  TEST("image2(2,0)", image2(2,0), true);
  TEST("image2(2,1)", image2(2,1), true);

  vil_structuring_element disk;
  disk.set_to_disk(4);
  vcl_cout<<"Structuring element: "<<disk<<vcl_endl;
  vil_image_view<bool> image3;
  image0.fill(false); image0(5,5)=true;
  vil_binary_dilate(image0,image3,disk);
  vcl_cout<<"Result of dilation with disk\n";
  print_binary_image(image3);
}

static void test_binary_dilate()
{
  test_binary_dilate_byte();
}

TESTMAIN(test_binary_dilate);
