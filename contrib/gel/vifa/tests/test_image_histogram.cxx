// This is gel/vifa/tests/test_image_histogram.cxx
#include <testlib/testlib_test.h>
#include <vifa/vifa_image_histogram.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_load.h>


static void test_image_histogram(int argc, char* argv[])
{
  // Get the image
  vil_image_view_base_sptr  vivb = vil_load(argc<2 ? "cam0013.tif" : argv[1]);

  if (vivb)
  {
    // Construct the image histogram
    vifa_image_histogram  vih(vivb);

    vih.Print();
  }
}


TESTMAIN_ARGS(test_image_histogram);
