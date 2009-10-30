// This is mul/vil3d/tests/test_algo_erode.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil3d/algo/vil3d_erode.h>

static inline void print_image(const vil3d_image_view<float>& im)
{
  for (unsigned j=0;j<im.nj();++j)
  {
    for (unsigned k=0;k<im.nk();++k)
    {
      // Concatenate rows
      for (unsigned i=0;i<im.ni();++i)
        if (im(i,j,k)) vcl_cout<<'X';
        else           vcl_cout<<'.';
      vcl_cout<<"     ";
    }
    vcl_cout<<vcl_endl;
  }
}

static void test_algo_erode()
{
  vcl_cout << "*********************\n"
           << " Testing vil3d_erode\n"
           << "*********************\n";

  vil3d_image_view<float> image0;
  image0.set_size(10,10,3);
  image0.fill(0.0);

  image0(5,4,1)=3.0f;  // Central pixel
  image0(5,5,1)=3.0f;  // Central pixel
  image0(5,6,1)=3.0f;  // Central pixel
  image0(3,0,1)=3.0f;  // Edge pixel
  image0(4,0,1)=3.0f;  // Edge pixel
  image0(5,0,1)=3.0f;  // Edge pixel
  vcl_cout<<"Original image\n";
  print_image(image0);

  vil3d_structuring_element element_i,element_j,element_k;
  element_i.set_to_line_i(-1,1);
  element_j.set_to_line_j(-1,1);
  element_k.set_to_line_k(-1,1);
  vcl_cout<<"Structuring element: "<<element_i<<vcl_endl;

  vil3d_image_view<float> image1;
  vil3d_erode(image0,image1,element_i);
  vcl_cout<<"Result of one erosion in i\n";
  print_image(image1);

  TEST("image1(5,4,1)",image1(5,4,1), 0.0);
  TEST("image1(5,5,1)",image1(5,5,1), 0.0);
  TEST("image1(5,6,1)",image1(5,6,1), 0.0);
  TEST("image1(3,0,1)",image1(3,0,1), 0.0);
  TEST("image1(4,0,1)",image1(4,0,1), 3.0);
  TEST("image1(5,0,1)",image1(5,0,1), 0.0);

  image1.fill(3.0);
  vil3d_erode(image0,image1,element_j);
  vcl_cout<<"Result of one erosion in j\n";
  print_image(image1);

  TEST("image1(5,4,1)",image1(5,4,1), 0.0);
  TEST("image1(5,5,1)",image1(5,5,1), 3.0);
  TEST("image1(5,6,1)",image1(5,6,1), 0.0);
  TEST("image1(3,0,1)",image1(3,0,1), 0.0);
  TEST("image1(4,0,1)",image1(4,0,1), 0.0);
  TEST("image1(5,0,1)",image1(5,0,1), 0.0);

  image1.fill(3.0);
  vil3d_erode(image0,image1,element_k);
  vcl_cout<<"Result of one erosion in k\n";
  print_image(image1);


  TEST("image1(5,4,1)",image1(5,4,1), 0.0);
  TEST("image1(5,5,1)",image1(5,5,1), 0.0);
  TEST("image1(5,6,1)",image1(5,6,1), 0.0);
  TEST("image1(3,0,1)",image1(3,0,1), 0.0);
  TEST("image1(4,0,1)",image1(4,0,1), 0.0);
  TEST("image1(5,0,1)",image1(5,0,1), 0.0);
}

TESTMAIN(test_algo_erode);
