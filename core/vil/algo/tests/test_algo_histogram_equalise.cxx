// This is core/vil/tests/test_algo_histogram_equalise.cxx
#include <testlib/testlib_test.h>
#include <vil/algo/vil_histogram_equalise.h>

static void test_histogram_equalise_byte()
{
  vil_image_view<vxl_byte> image(10,10);
  for (unsigned j=0;j<image.nj();++j)
    for (unsigned i=0;i<image.ni();++i)
      image(i,j) = i+j;

  vil_histogram_equalise(image);
  TEST("Equalised pixel (0,0)",image(0,0),0);
  TEST("Equalised pixel (0,1)",image(0,1),5);
  TEST("Equalised pixel (4,5)",image(4,5),139);
  TEST("Equalised pixel (9,9)",image(9,9),255);

  image.set_size(9,9);
  for (unsigned j=0;j<image.nj();++j)
    for (unsigned i=0;i<image.ni();++i)
      image(i,j) = i;

  vil_histogram_equalise(image);
  TEST("Equalised pixel (0,0)",image(0,0),0);
  TEST("Equalised pixel (1,1)",image(1,1),31);
  TEST("Equalised pixel (4,1)",image(4,1),127);
  TEST("Equalised pixel (8,1)",image(8,1),255);
}

static void test_algo_histogram_equalise()
{
  test_histogram_equalise_byte();
}

TESTMAIN(test_algo_histogram_equalise);
