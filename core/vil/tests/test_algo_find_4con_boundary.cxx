#include <vxl_config.h>
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <vil2/algo/vil2_find_4con_boundary.h>
#include <vil2/vil2_crop.h>

void show_boundary(const vcl_vector<int>& bi,const vcl_vector<int>& bj)
{
  for (unsigned int i=0;i<bi.size();++i)
    vcl_cout<<"("<<bi[i]<<","<<bj[i]<<")";
  vcl_cout<<vcl_endl;
}

void test_algo_find_4con_boundary_below_byte()
{
  vcl_cout<<"=== Testing vil2_find_4con_boundary_below_threshold ==="<<vcl_endl;
  vil2_image_view<vxl_byte> image(10,11);

  // Create 3 x 3 square
  image.fill(10);
  vil2_crop(image, 4,3, 5,3).fill(1);

  vcl_vector<int> bi,bj;
  vil2_find_4con_boundary_below_threshold(bi,bj,image,vxl_byte(7),5,5);

  show_boundary(bi,bj);

  TEST("Length of boundary (3x3)",bi.size(),8);

  // Create 4 x 4 square
  image.fill(10);
  vil2_crop(image, 4,4, 5,4).fill(1);

  vil2_find_4con_boundary_below_threshold(bi,bj,image,vxl_byte(7),5,5);

  show_boundary(bi,bj);

  TEST("Length of boundary (4x4)",bi.size(),12);

  // Create 4 x 1 line
  image.fill(10);
  vil2_crop(image, 4,4, 5,1).fill(1);

  vil2_find_4con_boundary_below_threshold(bi,bj,image,vxl_byte(7),5,5);

  show_boundary(bi,bj);

  TEST("Length of boundary (4x1)",bi.size(),6);


  // Create 1 x 5 line
  image.fill(10);
  vil2_crop(image, 5,1, 3,5).fill(1);

  vil2_find_4con_boundary_below_threshold(bi,bj,image,vxl_byte(7),5,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (1x5)",bi.size(),8);

  // Make an L shape
  image(6,3)=1;
  vil2_find_4con_boundary_below_threshold(bi,bj,image,vxl_byte(7),5,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (L shape)",bi.size(),10);

  // Make a T shape
  image.fill(10);
  vil2_crop(image, 5,1, 3,5).fill(1);
  image(6,5)=1;
  vil2_find_4con_boundary_below_threshold(bi,bj,image,vxl_byte(7),5,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (T shape)",bi.size(),10);

  // Line up to edge
  image.fill(10);
  vil2_crop(image, 5,1, 0,10).fill(1);
  vil2_find_4con_boundary_below_threshold(bi,bj,image,vxl_byte(7),5,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (Vertical line)",bi.size(),18);

  // Line up to edge
  image.fill(10);
  vil2_crop(image, 0,10, 5,1).fill(1);
  vil2_find_4con_boundary_below_threshold(bi,bj,image,vxl_byte(7),5,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (Horizontal line)",bi.size(),18);
}


void test_algo_find_4con_boundary_above_byte()
{
  vcl_cout<<"=== Testing vil2_find_4con_boundary_above_threshold ==="<<vcl_endl;
  vil2_image_view<vxl_byte> image(10,11);

  // Create 3 x 3 square
  image.fill(10);
  vil2_crop(image, 4,3, 5,3).fill(17);

  vcl_vector<int> bi,bj;
  vil2_find_4con_boundary_above_threshold(bi,bj,image,vxl_byte(12),5,5);

  show_boundary(bi,bj);

  TEST("Length of boundary (3x3)",bi.size(),8);

  // Create 4 x 4 square
  image.fill(10);
  vil2_crop(image, 4,4, 5,4).fill(17);

  vil2_find_4con_boundary_above_threshold(bi,bj,image,vxl_byte(12),5,5);

  show_boundary(bi,bj);

  TEST("Length of boundary (4x4)",bi.size(),12);

  // Create 4 x 1 line
  image.fill(10);
  vil2_crop(image, 4,4, 5,1).fill(17);

  vil2_find_4con_boundary_above_threshold(bi,bj,image,vxl_byte(12),5,5);

  show_boundary(bi,bj);

  TEST("Length of boundary (4x1)",bi.size(),6);


  // Create 1 x 5 line
  image.fill(10);
  vil2_crop(image, 5,1, 3,5).fill(17);

  vil2_find_4con_boundary_above_threshold(bi,bj,image,vxl_byte(12),5,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (1x5)",bi.size(),8);

  // Make an L shape
  image(6,3)=17;
  vil2_find_4con_boundary_above_threshold(bi,bj,image,vxl_byte(12),5,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (L shape)",bi.size(),10);

  // Make a T shape
  image.fill(10);
  vil2_crop(image, 5,1, 3,5).fill(17);
  image(6,5)=17;
  vil2_find_4con_boundary_above_threshold(bi,bj,image,vxl_byte(12),5,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (T shape)",bi.size(),10);

  // Line up to edge
  image.fill(10);
  vil2_crop(image, 5,1, 0,10).fill(17);
  vil2_find_4con_boundary_above_threshold(bi,bj,image,vxl_byte(12),5,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (Vertical line)",bi.size(),18);

  // Line up to edge
  image.fill(10);
  vil2_crop(image, 0,10, 5,1).fill(17);
  vil2_find_4con_boundary_above_threshold(bi,bj,image,vxl_byte(12),5,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (Horizontal line)",bi.size(),18);
}

MAIN( test_algo_find_4con_boundary )
{
  START( "vil2_find_4con_boundary" );
  test_algo_find_4con_boundary_below_byte();
  test_algo_find_4con_boundary_above_byte();
  SUMMARY();
}
