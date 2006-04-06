// This is mul/vimt/tests/test_image_bounds_and_centre_2d.cxx

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vimt/vimt_image_2d.h>
#include <vimt/vimt_image_2d_of.h>


static void test_world_bounding_box()
{
  vimt_image_2d_of<int> image(10, 10, 1);
  vgl_box_2d<double> bbox = world_bounding_box(image);
  vgl_point_2d<double> min_pt = bbox.min_point();
  vgl_point_2d<double> max_pt = bbox.max_point();
  bool ok = (min_pt==vgl_point_2d<double>(0,0) &&
             max_pt==vgl_point_2d<double>(9,9));
  TEST("world_bounding_box()", ok, true);
}


static test_centre_image_at_origin()
{
  vimt_image_2d_of<int> image(10, 10, 1);
  vgl_point_2d<double> orig1 = image.world2im().origin();
  vimt_centre_image_at_origin(image);
  vgl_point_2d<double> orig2 = image.world2im().origin();
  bool ok = (orig1 != orig2 &&
             orig2 == vgl_point_2d<double>(4.5, 4.5));
  TEST("centre_image_at_origin", ok, true);
}


static void test_image_bounds_and_centre_2d()
{
  test_world_bounding_box();
  test_centre_image_at_origin();
}


TESTMAIN(test_image_bounds_and_centre_2d);
