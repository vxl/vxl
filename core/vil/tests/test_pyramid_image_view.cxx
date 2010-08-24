// This is core/vil/tests/test_pyramid_image_view.cxx
#include <testlib/testlib_test.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pyramid_image_view.h>


static void test_pyramid_image_view()
{
  // test default constructor, should create an empty list of views
  vil_pyramid_image_view<float> v;
  TEST_EQUAL("Empty View", v.nlevels(), 0);

  // test adding a view to an epmty pyramid
  vil_image_view_base_sptr img = new vil_image_view<float>(10,10,1);
  v.add_view(img, 1.0);
  TEST_EQUAL("Add View", v.nlevels(), 1);

  vil_image_view<float> black_img(120,120,1);
  black_img.fill(1.0);
  vil_image_view_base_sptr black_img_sptr = new vil_image_view<float>(black_img);
  vil_pyramid_image_view<float> v2(black_img_sptr,3);

  // get the level 1 image. level 0 is the original one
  double scale;
  vil_image_view_base_sptr small_img = v2.get_view(1, scale);
  TEST_NEAR("New Scale at the pyramid", scale, 0.5, 0.001);
  TEST_EQUAL("New image ni at the pyramid", small_img->ni(), 60);
  TEST_EQUAL("New image nj at the pyramid", small_img->nj(), 60);

  // get the level 2 image. level 0 is the original one
  small_img = v2.get_view(2, scale);
  TEST_NEAR("New Scale at the pyramid", scale, 0.25, 0.001);
  TEST_EQUAL("New image ni at the pyramid", small_img->ni(), 30);
  TEST_EQUAL("New image nj at the pyramid", small_img->nj(), 30);

  // test adding a preset scaled image, inserted in the right place
  v2.add_view(small_img,0.7);
  small_img = v2.get_view(1, scale);
  TEST_NEAR("New Scale at the pyramid", scale, 0.7, 0.001);
}

TESTMAIN(test_pyramid_image_view);
