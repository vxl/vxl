// This is mul/vil2/tests/test_sample_grid_bilin.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/algo/vil2_binary_dilate.h>
#include <vil2/algo/vil2_binary_erode.h>

inline void print_binary_image(const vil2_image_view<vxl_byte>& im)
{
  for (int j=0;j<im.nj();++j)
  {
    for (int i=0;i<im.ni();++i)
	  if (im(i,j)==0) vcl_cout<<".";
	  else            vcl_cout<<"X";
    vcl_cout<<vcl_endl;
  }
}

void test_binary_erode_byte()
{
  vcl_cout << "********************************\n"
           << " Testing vil2_binary_dilate\n"
           << "********************************\n";

  vil2_image_view<vxl_byte> image0;
  image0.resize(10,10);
  image0.fill(0);

  image0(5,5)=1;  // Central pixel
  image0(3,0)=1;  // Edge pixel
  vcl_cout<<"Original image"<<vcl_endl;
  print_binary_image(image0);

  vcl_vector<int> px(3),py(3);
  px[0]=-1;py[0]=0;
  px[1]= 0;py[1]=0;
  px[2]= 1;py[2]=0;
  vil2_structuring_element element_i(px,py),element_j(py,px);
  vcl_cout<<"Structuring element: "<<element_i<<vcl_endl;

  vil2_image_view<vxl_byte> image1;
  vil2_binary_dilate(image0,image1,element_i);
  vil2_image_view<vxl_byte> image2;
  vil2_binary_dilate(image1,image2,element_j);
  vcl_cout<<"Result of two dilations"<<vcl_endl;
  print_binary_image(image2);



  vil2_image_view<vxl_byte> image3;
  vil2_binary_erode(image2,image3,element_j);
  vcl_cout<<"Result of one erosion"<<vcl_endl;
  print_binary_image(image3);

  TEST("image3(5,5)",image3(5,5),1);
  TEST("image3(4,5)",image3(4,5),1);
  TEST("image3(6,5)",image3(6,5),1);
  TEST("image3(5,6)",image3(5,6),0);

  vil2_image_view<vxl_byte> image4;
  vil2_binary_erode(image3,image4,element_i);
  vcl_cout<<"Result of two erosions"<<vcl_endl;
  print_binary_image(image4);

  TEST("image4(5,5)",image4(5,5),1);
  TEST("image4(4,5)",image4(4,5),0);
  TEST("image4(6,5)",image4(6,5),0);
  TEST("image4(3,0)",image4(3,0),0);

}

MAIN( test_binary_erode )
{
  START( "Binary Erosion" );

  test_binary_erode_byte();

  SUMMARY();
}
