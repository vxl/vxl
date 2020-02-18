#include <testlib/testlib_test.h>

#include <bsgm/bsgm_error_checking.h>
#include <vil/vil_image_view.h>


static void test_compute_invalid_map()
{
  /*
   * Test the bsgm_error_checking::bsgm_compute_invalid_map function.
   * We give as input the following one-pixel tall images
   * (where x represents a filled in value, and space represents the border value):
   *
   *  0 1 2 3 4 5 6
   * | | | |x|x| | |    <- Target image
   *
   *  0 1 2 3 4 5 6
   * | | |x|x|x| | |    <- Reference image
   *
   * And we expect to get the following invalid map as output:
   *
   *  0 1 2 3 4 5 6
   * |x|x|x| |x|x|x|    <- Invalid target image
   */

  // the value in the borders of our rectified images
  // that represents no data
  vxl_byte border_val = 0;

  // target image
  size_t width = 7;
  size_t height = 1;
  vil_image_view<vxl_byte> img_target(width, height);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      img_target(x, y) = border_val;
    }
  }
  img_target(3, 0) = 1;
  img_target(4, 0) = 1;

  // reference image
  vil_image_view<vxl_byte> img_reference(width, height);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      img_reference(x, y) = border_val;
    }
  }
  img_reference(2, 0) = 1;
  img_reference(3, 0) = 1;
  img_reference(4, 0) = 1;

  // disparity range
  int min_disparity = -1;
  int max_disparity = 1;

  // compute the invalid map
  vil_image_view<bool> invalid_target;
  bsgm_compute_invalid_map(img_target, img_reference, invalid_target,
                           min_disparity, (max_disparity - min_disparity),
                           border_val);

  // the invalid map we expect to get
  vil_image_view<bool> expected_invalid_target(width, height);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      expected_invalid_target(x, y) = true;
    }
  }
  expected_invalid_target(3, 0) = false;

  // check if the invalid map matches what we expected
  bool good = true;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      good = good && (invalid_target(x, y) == expected_invalid_target(x, y));
    }
  }
  TEST("Testing BSGM Compute Invalid Map", good, true);
}

TESTMAIN(test_compute_invalid_map);
