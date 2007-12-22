// This is mul/vimt/tests/test_v2i.cxx
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vnl/vnl_matrix.h>
#include <vul/vul_temp_filename.h>
#include <mbl/mbl_stl.h>
#include <vimt/vimt_load_transform.h>
#include <testlib/testlib_test.h>
#include <vsl/vsl_quick_file.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_property.h>
#include <vimt/vimt_vil_v2i.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_add_all_binary_loaders.h>

static void test_v2i()
{
  vcl_cout << "*****************************\n"
           << " Testing vimt_vil_v2i_format\n"
           << "*****************************\n";

  vimt_add_all_binary_loaders();

  // create a simple single plane image
  vil_image_view<vxl_int_32> im1(3,4);
  mbl_stl_increments(im1.begin(), im1.end(), -2);

  // Try saving and laoding vil image.
  vcl_string fname1 = vul_temp_filename() + ".v2i";
  TEST("Successfully saved simple v2i image",vil_save(im1,fname1.c_str()), true);
  vil_image_resource_sptr ir1 =   vil_load_image_resource(fname1.c_str());
  TEST( "Successfully loaded simple v2i image",!ir1, false);

  TEST("Loaded simple image has identity for a transform",
       dynamic_cast<vimt_vil_v2i_image&>(*ir1).world2im().is_identity(),true);
  vpl_unlink(fname1.c_str());

  vil_image_view<float> im2(3,4,6);
  mbl_stl_increments(im2.begin(), im2.end(), -200.0f);
  vimt_transform_2d tr2;
  tr2.set_zoom_only(2.0, -5.0, -5.0);
  vcl_string fname2 = vul_temp_filename() + ".v2i";
  {
    vil_image_resource_sptr ir2 = vil_new_image_resource(
      fname2.c_str(), 3, 4, 6, VIL_PIXEL_FORMAT_FLOAT, "v2i");
    TEST("Successfully opened image on disk", !ir2, false);

    ir2->put_view(im2);
    dynamic_cast<vimt_vil_v2i_image&>(*ir2).set_world2im(tr2);
    // Save image as ir2 is destroyed.
  }

  vil_image_resource_sptr ir3 = vil_load_image_resource(fname2.c_str());
  TEST( "Successfully loaded complicated v2i image",!ir3, false);
  vimt_image_2d_of<float> im3(ir3->get_view(), vimt_load_transform(ir3));
  TEST("Loaded complicated image has correct pixel values",
    vil_image_view_deep_equality(im3.image(), im2), true);
  TEST("Loaded complicated image has correct transform",
    (im3.world2im().matrix()- tr2.matrix()).frobenius_norm() < 1e-6 , true);

  float size[2];
  TEST("get_property()", ir3->get_property(vil_property_pixel_size, size), true);
  TEST("vil_property_pixel_size is correct", size[0] == 0.5 &&
    size[1] == 0.5, true);
  vpl_unlink(fname2.c_str());

  vil_image_view<float> im4(3,4,6);
  mbl_stl_increments(im4.begin(), im4.end(), -200.0f);

  vcl_string fname3 = vul_temp_filename() + ".v2i";
  {
    vil_image_resource_sptr ir4 = vil_new_image_resource(
      fname3.c_str(), 3, 4, 6, VIL_PIXEL_FORMAT_FLOAT, "v2i");
    TEST("Successfully opened image on disk", !ir4, false);

    ir4->put_view(im4);
    // Start with one pixel size
    dynamic_cast<vimt_vil_v2i_image&>(*ir4).set_pixel_size(0.001f, 0.002f);
    // Save image as ir4 is destroyed.
  }

  vil_image_resource_sptr ir5 = vil_load_image_resource(fname3.c_str());
  TEST("Successfully loaded complicated v2i image",!ir5, false);
  vil_image_view<float> im5(ir5->get_view());
  TEST("Loaded complicated image has correct pixel values",
       vil_image_view_deep_equality(im3.image(), im2), true);
  TEST("get_property()", ir5->get_property(vil_property_pixel_size, size), true);
  TEST("vil_property_pixel_size is correct", size[0] == 0.001f &&
       size[1] == 0.002f, true);

  vpl_unlink(fname3.c_str());

  vimt_transform_2d tr6;
  tr6.set_zoom_only(2.0, -5.0, -5.0);
  vimt_image_2d_of<float> im6(3,4,6, tr6);
  mbl_stl_increments(im6.image().begin(), im6.image().end(), -200.0f);

  vcl_string fname4 = vul_temp_filename() + ".v2i";
  vsl_quick_file_save(im6, fname4);

  vil_image_resource_sptr ir7 = vil_load_image_resource(fname4.c_str());
  TEST( "Successfully loaded complicated v2i image",!ir7, false);
  vil_image_view<float> im7(ir7->get_view());
  TEST("Loaded complicated image has correct pixel values",
       vil_image_view_deep_equality(im6.image(), im7), true);
  TEST("get_property()", ir7->get_property(vil_property_pixel_size, size), true);
  TEST("vil_property_pixel_size is correct", size[0] == 0.0005f &&
       size[1] == 0.0005f, true);
  vimt_transform_2d tr7 = vimt_load_transform(ir7, 1000.0);
  vcl_cout << "saved: " << tr6 << vcl_endl
           << "loaded: " << tr7 << vcl_endl;
  TEST("Loaded complicated image has correct transform",
       (tr6.matrix()- tr7.matrix()).frobenius_norm() < 1e-6 , true);

  vpl_unlink(fname4.c_str());
}

TESTMAIN(test_v2i);
