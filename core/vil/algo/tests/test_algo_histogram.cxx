// This is core/vil/algo/tests/test_algo_histogram.cxx
#include <testlib/testlib_test.h>
#include <vil/algo/vil_histogram.h>

static void test_histogram_byte()
{
  vil_image_view<vxl_byte> image(10,10);
  for (unsigned j=0;j<image.nj();++j)
    for (unsigned i=0;i<image.ni();++i)
      image(j,i) = i+j;

  vcl_vector<double> histo;
  vil_histogram_byte(image,histo);
  TEST("Histo size", histo.size(), 256);
  TEST_NEAR("Histo element 0", histo[0], 1, 1e-6);
  TEST_NEAR("Histo element 1", histo[1], 2, 1e-6);
  TEST_NEAR("Histo element 5", histo[5], 6, 1e-6);
  TEST_NEAR("Histo element 22",histo[22],0, 1e-6);

  vcl_vector<double> histo2;
  vil_histogram(image,histo2,0,20,20);
  TEST("Histo size", histo2.size(), 20);
  TEST_NEAR("Histo element 0", histo2[0], 1, 1e-6);
  TEST_NEAR("Histo element 1", histo2[1], 2, 1e-6);
  TEST_NEAR("Histo element 5", histo2[5], 6, 1e-6);
  TEST_NEAR("Histo element 19",histo2[19],0, 1e-6);

  vcl_vector<double> histo3;
  vil_histogram(image,histo3,0,20,10);
  TEST("Histo size", histo3.size(), 10);
  TEST_NEAR("Histo element 0", histo3[0], 3, 1e-6);
  TEST_NEAR("Histo element 1", histo3[1], 7, 1e-6);
  TEST_NEAR("Histo element 2", histo3[2],11, 1e-6);
  TEST_NEAR("Histo element 9", histo3[9], 0, 1e-6);
}

static void test_algo_histogram()
{
  test_histogram_byte();
}

TESTMAIN(test_algo_histogram);
