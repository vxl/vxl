// This is mul/vil3d/tests/test_algo_binary_dilate.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil3d/algo/vil3d_binary_dilate.h>

inline void print_binary_image(const vil3d_image_view<bool>& im)
{
  for (unsigned j=0;j<im.nj();++j)
  {
    for (unsigned k=0;k<im.nk();++k)
    {
      // Concatenate rows
      for (unsigned i=0;i<im.ni();++i)
        vcl_cout << (im(i,j,k) ? 'X' : '.');
      vcl_cout << "     ";
    }
    vcl_cout<<vcl_endl;
  }
}

static void test_algo_binary_dilate()
{
  vcl_cout << "*****************************\n"
           << " Testing vil3d_binary_dilate\n"
           << "*****************************\n";

  vil3d_image_view<bool> image0;
  image0.set_size(10,10,3);
  image0.fill(false);

  image0(5,5,1)=true;  // Central pixel
  image0(3,0,1)=true;  // Edge pixel
  vcl_cout<<"Original image\n";
  print_binary_image(image0);

  vil3d_structuring_element element_i,element_j,element_k;
  element_i.set_to_line_i(-1,1);
  element_j.set_to_line_j(-1,1);
  element_k.set_to_line_k(-1,1);
  vcl_cout<<"Structuring element: "<<element_i<<vcl_endl;

  vil3d_image_view<bool> image1;
  vil3d_binary_dilate(image0,image1,element_i);
  vcl_cout<<"Result of one dilation in i\n";
  print_binary_image(image1);

  TEST("image1(5,5,1)",image1(5,5,1),true);
  TEST("image1(4,5,1)",image1(4,5,1),true);
  TEST("image1(6,5,1)",image1(6,5,1),true);
  TEST("image1(5,6,1)",image1(5,6,1),false);
  TEST("image1(2,0,1)",image1(2,0,1),true);

  image1.fill(true);
  vil3d_binary_dilate(image0,image1,element_j);
  vcl_cout<<"Result of one dilation in j\n";
  print_binary_image(image1);

  TEST("image1(5,5,1)",image1(5,5,1),true);
  TEST("image1(4,5,1)",image1(4,5,1),false);
  TEST("image1(6,5,1)",image1(6,5,1),false);
  TEST("image1(5,6,1)",image1(5,6,1),true);
  TEST("image1(5,4,1)",image1(5,6,1),true);
  TEST("image1(3,0,1)",image1(3,0,1),true);
  TEST("image1(3,1,1)",image1(3,1,1),true);

  image1.fill(true);
  vil3d_binary_dilate(image0,image1,element_k);
  vcl_cout<<"Result of one dilation in k\n";
  print_binary_image(image1);

  TEST("image1(5,5,1)",image1(5,5,1),true);
  TEST("image1(4,5,1)",image1(4,5,1),false);
  TEST("image1(6,5,1)",image1(6,5,1),false);
  TEST("image1(5,5,0)",image1(5,5,0),true);
  TEST("image1(5,5,2)",image1(5,5,2),true);
  TEST("image1(3,0,1)",image1(3,0,1),true);
  TEST("image1(3,0,0)",image1(3,0,0),true);
}

TESTMAIN(test_algo_binary_dilate);
