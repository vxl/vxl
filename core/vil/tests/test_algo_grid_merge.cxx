// This is mul/vil2/tests/test_algo_threshold.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/algo/vil2_grid_merge.h>

void test_grid_merge_byte()
{
  vcl_cout << "************************\n"
           << " Testing vil2_grid_merge\n"
           << "************************\n";

  vil2_image_view<int> image1,image2, dest_image;

  image1.set_size(20,30);  image1.fill(17);
  image2.set_size(20,30);  image2.fill(31);

  vil2_grid_merge(image1,image2,dest_image,5,3);

  TEST("Width",dest_image.ni(),image1.ni());
  TEST("Height",dest_image.nj(),image1.nj());
  TEST("Origin",dest_image(0,0),image1(0,0));
  TEST("(3,4)",dest_image(3,4),image2(0,0));
  TEST("(6,1)",dest_image(6,1),image2(0,0));
  TEST("(6,4)",dest_image(6,4),image1(0,0));

}



MAIN( test_algo_grid_merge )
{
  START( "vil2_grid_merge" );

  test_grid_merge_byte();

  SUMMARY();
}
