// This is mul/vil2/tests/test_sample_grid_bilin.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vxl_config.h> // for bool
#include <vil2/algo/vil2_binary_dilate.h>

void print_binary_image(const vil2_image_view<bool>& im)
{
  for (int j=0;j<im.nj();++j)
  {
    for (int i=0;i<im.ni();++i)
	  if (im(i,j)) vcl_cout<<".";
	  else         vcl_cout<<"X";
    vcl_cout<<vcl_endl;
  }
}

void test_binary_dilate_byte()
{
  vcl_cout << "********************************\n"
           << " Testing vil2_binary_dilate\n"
           << "********************************\n";

  vil2_image_view<bool> image0;
  image0.resize(10,10);
  image0.fill(false);

  image0(5,5)=true;  // Central pixel
  image0(3,0)=true;  // Edge pixel
  vcl_cout<<"Original image"<<vcl_endl;
  print_binary_image(image0);

  vcl_vector<int> px(3),py(3);
  px[0]=-1;py[0]=0;
  px[1]= 0;py[1]=0;
  px[2]= 1;py[2]=0;
  vil2_structuring_element element_i(px,py),element_j(py,px);
  vcl_cout<<"Structuring element: "<<element_i<<vcl_endl;

  vil2_image_view<bool> image1;
  vil2_binary_dilate(image0,image1,element_i);
  vcl_cout<<"Result of one dilation"<<vcl_endl;
  print_binary_image(image1);
  TEST("image1(5,5)",image1(5,5),true);
  TEST("image1(4,5)",image1(4,5),true);
  TEST("image1(6,5)",image1(6,5),true);
  TEST("image1(5,6)",image1(5,6),false);
  TEST("image1(2,0)",image1(2,0),true);

  vil2_image_view<bool> image2;
  vil2_binary_dilate(image1,image2,element_j);
  vcl_cout<<"Result of two dilations"<<vcl_endl;
  print_binary_image(image2);
  TEST("image2(5,5)",image2(5,5),true);
  TEST("image2(4,5)",image2(4,5),true);
  TEST("image2(6,5)",image2(6,5),true);
  TEST("image2(5,6)",image2(5,6),true);
  TEST("image2(5,4)",image2(5,4),true);
  TEST("image2(5,0)",image2(5,0),false);
  TEST("image2(5,9)",image2(5,9),false);
  TEST("image2(2,0)",image2(2,0),true);
  TEST("image2(2,1)",image2(2,1),true);

  vil2_structuring_element disk;
  disk.set_to_disk(4);
  vcl_cout<<"Structuring element: "<<disk<<vcl_endl;
  vil2_image_view<bool> image3;
  image0.fill(false); image0(5,5)=true;
  vil2_binary_dilate(image0,image3,disk);
  vcl_cout<<"Result of dilation with disk"<<vcl_endl;
  print_binary_image(image3);



}

MAIN( test_binary_dilate )
{
  START( "Binary Dilate" );

  test_binary_dilate_byte();

  SUMMARY();
}
