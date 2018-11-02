// This is core/vil/algo/tests/test_binary_erode.cxx
#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_binary_opening.h>
#include <vil/algo/vil_binary_closing.h>

static inline void print_binary_image(const vil_image_view<bool>& im)
{
  for (unsigned j=0;j<im.nj();++j)
  {
    for (unsigned i=0;i<im.ni();++i)
      if (im(i,j)) std::cout<<"X";
      else         std::cout<<".";
    std::cout<<std::endl;
  }
}

static void test_binary_erode_byte()
{
  std::cout << "**************************\n"
           << " Testing vil_binary_erode\n"
           << "**************************\n";

  vil_image_view<bool> image0;
  image0.set_size(10,10);
  image0.fill(false);

  image0(5,5)=true;  // Central pixel
  image0(3,0)=true;  // Edge pixel
  std::cout<<"Original image\n";
  print_binary_image(image0);

  std::vector<int> px(3),py(3);
  px[0]=-1;py[0]=0;
  px[1]= 0;py[1]=0;
  px[2]= 1;py[2]=0;
  vil_structuring_element element_i(px,py),element_j(py,px);
  std::cout<<"Structuring element: "<<element_i<<std::endl;

  vil_image_view<bool> image1;
  vil_binary_dilate(image0,image1,element_i);
  vil_image_view<bool> image2;
  vil_binary_dilate(image1,image2,element_j);
  std::cout<<"Result of two dilations\n";
  print_binary_image(image2);


  vil_image_view<bool> image3;
  vil_binary_erode(image2,image3,element_j);
  std::cout<<"Result of one erosion\n";
  print_binary_image(image3);

  TEST("image3(5,5)", image3(5,5), true);
  TEST("image3(4,5)", image3(4,5), true);
  TEST("image3(6,5)", image3(6,5), true);
  TEST("image3(5,6)", image3(5,6), false);

  vil_image_view<bool> image4;
  vil_binary_erode(image3,image4,element_i);
  std::cout<<"Result of two erosions\n";
  print_binary_image(image4);

  TEST("image4(5,5)", image4(5,5), true);
  TEST("image4(4,5)", image4(4,5), false);
  TEST("image4(6,5)", image4(6,5), false);
  TEST("image4(3,0)", image4(3,0), true);


  std::cout<<"Testing vil_binary_closing\n";
  image0.fill(false);
  image0(4,5)=true;
  image0(6,5)=true;
  vil_binary_closing(image0,image1,element_i);
  TEST("image1(4,5)", image1(4,5), true);
  TEST("Gap filled",  image1(5,5), true);
  TEST("image1(6,5)", image1(6,5), true);
  TEST("image1(7,5)", image1(7,5), false);

  std::cout<<"Testing vil_binary_opening\n";
  vil_binary_opening(image0,image1,element_i);
  TEST("image1(4,5)", image1(4,5), false);
  TEST("Gap",         image1(5,5), false);
  TEST("image1(6,5)", image1(6,5), false);
  TEST("image1(7,5)", image1(7,5), false);
}

static void test_binary_erode()
{
  test_binary_erode_byte();
}

TESTMAIN(test_binary_erode);
