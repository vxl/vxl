
#include <testlib/testlib_test.h>
#include <vil3d/algo/vil3d_histogram.h>

static void test_histogram_byte()
{
  vil3d_image_view<vxl_byte> image(5,5,5);
  for (unsigned k=0;k<image.nk();++k)
    for (unsigned j=0;j<image.nj();++j)
      for (unsigned i=0;i<image.ni();++i)
        image(i,j,k) = i+j+k;

  vcl_vector<double> histo;
  vil3d_histogram_byte(image,histo);
  TEST("Histo size", histo.size(), 256);
  TEST_NEAR("Histo element 0", histo[0], 1, 1e-6);
  TEST_NEAR("Histo element 1", histo[1], 3, 1e-6);
  TEST_NEAR("Histo element 2", histo[2], 6, 1e-6);
  TEST_NEAR("Histo element 3", histo[3], 10, 1e-6);
  TEST_NEAR("Histo element 4", histo[4], 15, 1e-6);
  TEST_NEAR("Histo element 5", histo[5], 18, 1e-6);
  TEST_NEAR("Histo element 13",histo[22],0, 1e-6);

  vcl_vector<double> histo2;
  vil3d_histogram(image,histo2,0,20,20);
  TEST("Histo size", histo2.size(), 20);
  TEST_NEAR("Histo element 0", histo2[0], 1, 1e-6);
  TEST_NEAR("Histo element 1", histo2[1], 3, 1e-6);
  TEST_NEAR("Histo element 5", histo2[5], 18, 1e-6);
  TEST_NEAR("Histo element 19",histo2[19],0, 1e-6);

  vcl_vector<double> histo3;
  vil3d_histogram(image,histo3,0,20,10);
  TEST("Histo size", histo3.size(), 10);
  TEST_NEAR("Histo element 0", histo3[0], 4, 1e-6);
  TEST_NEAR("Histo element 1", histo3[1], 16, 1e-6);
  TEST_NEAR("Histo element 2", histo3[2], 33, 1e-6);
  TEST_NEAR("Histo element 9", histo3[9], 0, 1e-6);
}

static void test_algo_histogram()
{
  test_histogram_byte();
}

TESTMAIN(test_algo_histogram);
