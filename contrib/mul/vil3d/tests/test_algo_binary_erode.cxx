// This is mul/vil3d/tests/test_algo_binary_erode.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#include <vcl_compiler.h>
#include <vil3d/algo/vil3d_binary_erode.h>

inline void print_binary_image(const vil3d_image_view<bool>& im)
{
  for (unsigned j=0;j<im.nj();++j)
  {
    for (unsigned k=0;k<im.nk();++k)
    {
      // Concatenate rows
      for (unsigned i=0;i<im.ni();++i)
        if (im(i,j,k)) std::cout<<'X';
        else           std::cout<<'.';
      std::cout<<"     ";
    }
    std::cout<<std::endl;
  }
}

static void test_algo_binary_erode()
{
  std::cout << "****************************\n"
           << " Testing vil3d_binary_erode\n"
           << "****************************\n";

  vil3d_image_view<bool> image0;
  image0.set_size(10,10,3);
  image0.fill(false);

  image0(5,4,1)=true;  // Central pixel
  image0(5,5,1)=true;  // Central pixel
  image0(5,6,1)=true;  // Central pixel
  image0(3,0,1)=true;  // Edge pixel
  image0(4,0,1)=true;  // Edge pixel
  image0(5,0,1)=true;  // Edge pixel
  std::cout<<"Original image\n";
  print_binary_image(image0);

  vil3d_structuring_element element_i,element_j,element_k;
  element_i.set_to_line_i(-1,1);
  element_j.set_to_line_j(-1,1);
  element_k.set_to_line_k(-1,1);
  std::cout<<"Structuring element: "<<element_i<<std::endl;

  vil3d_image_view<bool> image1;
  vil3d_binary_erode(image0,image1,element_i);
  std::cout<<"Result of one erosion in i\n";
  print_binary_image(image1);

  TEST("image1(5,4,1)",image1(5,4,1),false);
  TEST("image1(5,5,1)",image1(5,5,1),false);
  TEST("image1(5,6,1)",image1(5,6,1),false);
  TEST("image1(3,0,1)",image1(3,0,1),false);
  TEST("image1(4,0,1)",image1(4,0,1),true);
  TEST("image1(5,0,1)",image1(5,0,1),false);

  image1.fill(true);
  vil3d_binary_erode(image0,image1,element_j);
  std::cout<<"Result of one erosion in j\n";
  print_binary_image(image1);

  TEST("image1(5,4,1)",image1(5,4,1),false);
  TEST("image1(5,5,1)",image1(5,5,1),true);
  TEST("image1(5,6,1)",image1(5,6,1),false);
  TEST("image1(3,0,1)",image1(3,0,1),false);
  TEST("image1(4,0,1)",image1(4,0,1),false);
  TEST("image1(5,0,1)",image1(5,0,1),false);

  image1.fill(true);
  vil3d_binary_erode(image0,image1,element_k);
  std::cout<<"Result of one erosion in k\n";
  print_binary_image(image1);


  TEST("image1(5,4,1)",image1(5,4,1),false);
  TEST("image1(5,5,1)",image1(5,5,1),false);
  TEST("image1(5,6,1)",image1(5,6,1),false);
  TEST("image1(3,0,1)",image1(3,0,1),false);
  TEST("image1(4,0,1)",image1(4,0,1),false);
  TEST("image1(5,0,1)",image1(5,0,1),false);
}

TESTMAIN(test_algo_binary_erode);
