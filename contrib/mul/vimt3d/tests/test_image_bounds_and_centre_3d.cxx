// This is mul/vimt3d/tests/test_image_bounds_and_centre_3d.cxx

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vimt3d/vimt3d_image_3d.h>
#include <vimt3d/vimt3d_image_3d_of.h>


static void test_world_bounding_box()
{
  vcl_cout << "--------------------------------------------------\n";
  vcl_cout << "test_world_bounding_box(): \n";
  vcl_cout << "--------------------------------------------------\n";

  // Image with identity transform
  {
    const unsigned ni = 8;  // image width (pixels)
    const unsigned nj = 9;  // image height (pixels)
    const unsigned nk = 10; // image depth (pixels)
    const unsigned np = 1;
    const vgl_point_3d<double> pix(1.0, 1.0, 1.0); // pixel dimensions (mm)
    vimt3d_transform_3d w2i;
    w2i.set_zoom_only(1.0/pix.x(), 1.0/pix.y(), 1.0/pix.z(), 0, 0, 0);
    const vimt3d_image_3d_of<int> image(ni, nj, nk, np, w2i);

    const vgl_box_3d<double> bbox = world_bounding_box(image);
    const vgl_point_3d<double> min_pt(0.0, 0.0, 0.0);
    const vgl_point_3d<double> max_pt((ni-1)*pix.x(), (nj-1)*pix.y(), (nk-1)*pix.z());
    TEST("With identity transform", 
         (bbox.min_point()-min_pt).length()<1e-6 && 
         (bbox.max_point()-max_pt).length()<1e-6,  
         true);
  }
  
  // Image with zoom transform (i.e. non-unity pixel size)
  {
    const unsigned ni = 8;  // image width (pixels)
    const unsigned nj = 9;  // image height (pixels)
    const unsigned nk = 10; // image depth (pixels)
    const unsigned np = 1;
    const vgl_point_3d<double> pix(0.7, 1.2, 1.3); // pixel dimensions (mm)
    vimt3d_transform_3d w2i;
    w2i.set_zoom_only(1.0/pix.x(), 1.0/pix.y(), 1.0/pix.z(), 0, 0, 0);
    const vimt3d_image_3d_of<int> image(ni, nj, nk, np, w2i);

    const vgl_box_3d<double> bbox = world_bounding_box(image);
    const vgl_point_3d<double> min_pt(0.0, 0.0, 0.0);
    const vgl_point_3d<double> max_pt((ni-1)*pix.x(), (nj-1)*pix.y(), (nk-1)*pix.z());
    TEST("With zoom transform", 
         (bbox.min_point()-min_pt).length()<1e-6 && 
         (bbox.max_point()-max_pt).length()<1e-6,  
          true);
  }
}


static void test_centre_image_at_origin()
{
  vcl_cout << "--------------------------------------------------\n";
  vcl_cout << "test_centre_image_at_origin(): \n";
  vcl_cout << "--------------------------------------------------\n";

  // Image with identity transform
  {
    const unsigned ni = 8;  // image width (pixels)
    const unsigned nj = 9;  // image height (pixels)
    const unsigned nk = 10; // image depth (pixels)
    const unsigned np = 1;
    const vgl_point_3d<double> pix(1.0, 1.0, 1.0); // pixel dimensions (mm)
    vimt3d_transform_3d w2i;
    w2i.set_zoom_only(1.0/pix.x(), 1.0/pix.y(), 1.0/pix.z(), 0, 0, 0);
    vimt3d_image_3d_of<int> image(ni, nj, nk, np, w2i);

    vgl_point_3d<double> orig1 = image.world2im().origin();
    vgl_box_3d<double> bbox1 = world_bounding_box(image);
    vimt3d_centre_image_at_origin(image);
    vgl_point_3d<double> orig2 = image.world2im().origin();
    vgl_box_3d<double> bbox2 = world_bounding_box(image);
    
    const vgl_point_3d<double> orig_true((ni-1)/2.0, (nj-1)/2.0, (nk-1)/2.0);
    bool orig_ok = ((orig1-orig2).length()>1e-6 &&    // check that origin has changed
                    (orig2-orig_true).length()<1e-6); // check that new origin is correct
    TEST("With identity transform, orig OK?", orig_ok, true);

    const double hwx = (ni-1)*pix.x()/2.0; // half-width of centred image in world coords
    const double hwy = (nj-1)*pix.y()/2.0; // half-width of centred image in world coords
    const double hwz = (nk-1)*pix.z()/2.0; // half-width of centred image in world coords
    const vgl_point_3d<double> lo(-hwx, -hwy, -hwz); 
    const vgl_point_3d<double> hi(+hwx, +hwy, +hwz); 
    const vgl_box_3d<double> bbox(lo, hi); // bbox of centred image in world coords
    bool bbox_ok = !(bbox1==bbox2) &&   // check that bbox has changed
                   (bbox.min_point()-bbox2.min_point()).length()<1e-6 && // check that bbox is correct
                   (bbox.max_point()-bbox2.max_point()).length()<1e-6;
    TEST("With identity transform, bbox correct?", bbox_ok, true);
  }

  // Image with zoom transform (i.e. non-unity pixel size)
  {
    const unsigned ni = 8;  // image width (pixels)
    const unsigned nj = 9;  // image height (pixels)
    const unsigned nk = 10; // image depth (pixels)
    const unsigned np = 1;
    const vgl_point_3d<double> pix(0.7, 1.2, 1.3); // pixel dimensions (mm)
    vimt3d_transform_3d w2i;
    w2i.set_zoom_only(1.0/pix.x(), 1.0/pix.y(), 1.0/pix.z(), 0, 0, 0);
    vimt3d_image_3d_of<int> image(ni, nj, nk, np, w2i);

    vgl_point_3d<double> orig1 = image.world2im().origin();
    vgl_box_3d<double> bbox1 = world_bounding_box(image);
    vimt3d_centre_image_at_origin(image);
    vgl_point_3d<double> orig2 = image.world2im().origin();
    vgl_box_3d<double> bbox2 = world_bounding_box(image);

    const vgl_point_3d<double> orig_true((ni-1)/2.0, (nj-1)/2.0, (nk-1)/2.0);
    bool orig_ok = ((orig1-orig2).length()>1e-6 &&    // check that origin has changed
                    (orig2-orig_true).length()<1e-6); // check that new origin is correct
    TEST("With zoom transform, origin correct?", orig_ok, true);

    const double hwx = (ni-1)*pix.x()/2.0; // half-width of centred image in world coords
    const double hwy = (nj-1)*pix.y()/2.0; // half-width of centred image in world coords
    const double hwz = (nk-1)*pix.z()/2.0; // half-width of centred image in world coords
    const vgl_point_3d<double> lo(-hwx, -hwy, -hwz); 
    const vgl_point_3d<double> hi(+hwx, +hwy, +hwz); 
    const vgl_box_3d<double> bbox(lo, hi); // bbox of centred image in world coords
    bool bbox_ok = !(bbox1==bbox2) &&   // check that bbox has changed
                   (bbox.min_point()-bbox2.min_point()).length()<1e-6 && // check that bbox is correct
                   (bbox.max_point()-bbox2.max_point()).length()<1e-6;
    TEST("With zoom transform, bbox correct?", bbox_ok, true);
  }
}


static void test_image_bounds_and_centre_3d()
{
  test_world_bounding_box();
  test_centre_image_at_origin();
}


TESTMAIN(test_image_bounds_and_centre_3d);
