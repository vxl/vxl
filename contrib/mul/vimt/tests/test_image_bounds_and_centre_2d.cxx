// This is mul/vimt/tests/test_image_bounds_and_centre_2d.cxx

#include <iostream>
#include <testlib/testlib_test.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vimt/vimt_transform_2d.h>
#include <vimt/vimt_image_2d_of.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>

static void test_world_bounding_box()
{
  std::cout << "---------------------------\n"
           << "test_world_bounding_box():\n"
           << "---------------------------\n";

  // Image with identity transform
  {
    constexpr unsigned ni = 8;  // image width (pixels)
    constexpr unsigned nj = 9;  // image height (pixels)
    constexpr unsigned np = 1;
    const vgl_point_2d<double> pix(1.0, 1.0); // pixel dimensions (mm)
    vimt_transform_2d w2i;
    w2i.set_zoom_only(1.0/pix.x(), 1.0/pix.y(), 0, 0);
    const vimt_image_2d_of<int> image(ni, nj, np, w2i);

    const vgl_box_2d<double> bbox = world_bounding_box(image);
    const vgl_point_2d<double> min_pt(0.0, 0.0);
    const vgl_point_2d<double> max_pt((ni-1)*pix.x(), (nj-1)*pix.y());
    TEST("With identity transform",
         (bbox.min_point()-min_pt).length()<1e-6 &&
         (bbox.max_point()-max_pt).length()<1e-6,
         true);
  }

  // Image with zoom transform (i.e. non-unity pixel size)
  {
    constexpr unsigned ni = 8;  // image width (pixels)
    constexpr unsigned nj = 9;  // image height (pixels)
    constexpr unsigned np = 1;
    const vgl_point_2d<double> pix(0.7, 1.2); // pixel dimensions (mm)
    vimt_transform_2d w2i;
    w2i.set_zoom_only(1.0/pix.x(), 1.0/pix.y(), 0, 0);
    const vimt_image_2d_of<int> image(ni, nj, np, w2i);

    const vgl_box_2d<double> bbox = world_bounding_box(image);
    const vgl_point_2d<double> min_pt(0.0, 0.0);
    const vgl_point_2d<double> max_pt((ni-1)*pix.x(), (nj-1)*pix.y());
    TEST("With zoom transform",
         (bbox.min_point()-min_pt).length()<1e-6 &&
         (bbox.max_point()-max_pt).length()<1e-6,
          true);
  }
}


static void test_centre_image_at_origin()
{
  std::cout << "-------------------------------\n"
           << "test_centre_image_at_origin():\n"
           << "-------------------------------\n";

  // Image with identity transform
  {
    constexpr unsigned ni = 8;  // image width (pixels)
    constexpr unsigned nj = 9;  // image height (pixels)
    constexpr unsigned np = 1;
    const vgl_point_2d<double> pix(1.0, 1.0); // pixel dimensions (mm)
    vimt_transform_2d w2i;
    w2i.set_zoom_only(1.0/pix.x(), 1.0/pix.y(), 0, 0);
    vimt_image_2d_of<int> image(ni, nj, np, w2i);

    vgl_point_2d<double> orig1 = image.world2im().origin();
    vgl_box_2d<double> bbox1 = world_bounding_box(image);
    vimt_centre_image_at_origin(image);
    vgl_point_2d<double> orig2 = image.world2im().origin();
    vgl_box_2d<double> bbox2 = world_bounding_box(image);

    const vgl_point_2d<double> orig_true((ni-1)/2.0, (nj-1)/2.0);
    bool orig_ok = ((orig1-orig2).length()>1e-6 &&    // check that origin has changed
                    (orig2-orig_true).length()<1e-6); // check that new origin is correct
    TEST("With identity transform, orig OK?", orig_ok, true);

    const double hwx = (ni-1)*pix.x()/2.0; // half-width of centred image in world coords
    const double hwy = (nj-1)*pix.y()/2.0; // half-width of centred image in world coords
    const vgl_point_2d<double> lo(-hwx, -hwy);
    const vgl_point_2d<double> hi(+hwx, +hwy);
    const vgl_box_2d<double> bbox(lo, hi); // bbox of centred image in world coords
    bool bbox_ok = !(bbox1==bbox2) &&   // check that bbox has changed
                   (bbox.min_point()-bbox2.min_point()).length()<1e-6 && // check that bbox is correct
                   (bbox.max_point()-bbox2.max_point()).length()<1e-6;
    TEST("With identity transform, bbox correct?", bbox_ok, true);
  }

  // Image with zoom transform (i.e. non-unity pixel size)
  {
    constexpr unsigned ni = 8;  // image width (pixels)
    constexpr unsigned nj = 9;  // image height (pixels)
    constexpr unsigned np = 1;
    const vgl_point_2d<double> pix(0.7, 1.2); // pixel dimensions (mm)
    vimt_transform_2d w2i;
    w2i.set_zoom_only(1.0/pix.x(), 1.0/pix.y(), 0, 0);
    vimt_image_2d_of<int> image(ni, nj, np, w2i);

    vgl_point_2d<double> orig1 = image.world2im().origin();
    vgl_box_2d<double> bbox1 = world_bounding_box(image);
    vimt_centre_image_at_origin(image);
    vgl_point_2d<double> orig2 = image.world2im().origin();
    vgl_box_2d<double> bbox2 = world_bounding_box(image);

    const vgl_point_2d<double> orig_true((ni-1)/2.0, (nj-1)/2.0);
    bool orig_ok = ((orig1-orig2).length()>1e-6 &&    // check that origin has changed
                    (orig2-orig_true).length()<1e-6); // check that new origin is correct
    TEST("With zoom transform, origin correct?", orig_ok, true);

    const double hwx = (ni-1)*pix.x()/2.0; // half-width of centred image in world coords
    const double hwy = (nj-1)*pix.y()/2.0; // half-width of centred image in world coords
    const vgl_point_2d<double> lo(-hwx, -hwy);
    const vgl_point_2d<double> hi(+hwx, +hwy);
    const vgl_box_2d<double> bbox(lo, hi); // bbox of centred image in world coords
    bool bbox_ok = !(bbox1==bbox2) &&   // check that bbox has changed
                   (bbox.min_point()-bbox2.min_point()).length()<1e-6 && // check that bbox is correct
                   (bbox.max_point()-bbox2.max_point()).length()<1e-6;
    TEST("With zoom transform, bbox correct?", bbox_ok, true);
  }
}


static void test_image_bounds_and_centre_2d()
{
  test_world_bounding_box();
  test_centre_image_at_origin();
}


TESTMAIN(test_image_bounds_and_centre_2d);
