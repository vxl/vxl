// This is mul/vimt3d/tests/test_save.cxx
#include <vcl_iostream.h>
#include <mbl/mbl_test.h>
#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_new.h>
#include <vimt3d/vimt3d_add_all_loaders.h>
#include <vimt3d/vimt3d_load.h>
#include <vimt3d/vimt3d_save.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <testlib/testlib_test.h>


static void test_save()
{
  vcl_cout << "*********************\n"
           << " Testing vimt3d_save\n"
           << "*********************\n";

  vimt3d_add_all_loaders();


  vimt3d_transform_3d tr_gold1;
  tr_gold1.set_zoom_only( 0.1, 0.2, 0.3, 4, 5, 6);

  {
    vil3d_image_resource_sptr ir_out1 = vil3d_new_image_resource(
      "test_save.v3i", 5, 5, 5, 1, VIL_PIXEL_FORMAT_FLOAT, "v3i");
    vimt3d_save_transform(ir_out1, tr_gold1, true);
  }

  vil3d_image_resource_sptr im_in1 = vil3d_load_image_resource("test_save.v3i");
  vimt3d_transform_3d tr_in1 = vimt3d_load_transform(im_in1, true);


  vcl_cout << "Golden transform: " << tr_gold1
           << "\nLoaded transform: " << tr_in1 << vcl_endl;

  TEST("v3i image round-trip has same zoom transform",
       mbl_test_summaries_are_equal(tr_in1, tr_gold1 ), true);


  {
    vil3d_image_resource_sptr ir_out1 = vil3d_new_image_resource(
      "test_save.v3m", 5, 5, 5, 1, VIL_PIXEL_FORMAT_FLOAT, "v3m");
    vimt3d_save_transform(ir_out1, tr_gold1, true);
  }

  vil3d_image_resource_sptr im_in2 = vil3d_load_image_resource("test_save.v3m");
  vimt3d_transform_3d tr_in2 = vimt3d_load_transform(im_in2, true);


  vcl_cout << "Golden transform: " << tr_gold1
           << "\nLoaded transform: " << tr_in2 << vcl_endl;

  TEST("v3m image round-trip has same zoom transform",
       mbl_test_summaries_are_equal(tr_in2, tr_gold1 ), true);

}

TESTMAIN(test_save);
