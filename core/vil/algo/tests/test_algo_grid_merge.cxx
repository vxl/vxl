// This is core/vil/algo/tests/test_algo_grid_merge.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/algo/vil_grid_merge.h>

static void test_grid_merge_byte()
{
  vcl_cout << "************************\n"
           << " Testing vil_grid_merge\n"
           << "************************\n";

  vil_image_view<int> image1,image2, dest_image;

  image1.set_size(20,30);  image1.fill(17);
  image2.set_size(20,30);  image2.fill(31);

  vil_grid_merge(image1,image2,dest_image,5,3);

  TEST("Width", dest_image.ni(), image1.ni());
  TEST("Height",dest_image.nj(), image1.nj());
  TEST("Origin",dest_image(0,0), image1(0,0));
  TEST("(3,4)", dest_image(3,4), image2(0,0));
  TEST("(6,1)", dest_image(6,1), image2(0,0));
  TEST("(6,4)", dest_image(6,4), image1(0,0));
}

static void test_algo_grid_merge()
{
  test_grid_merge_byte();
}

TESTMAIN(test_algo_grid_merge);
