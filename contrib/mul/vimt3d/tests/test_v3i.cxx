// This is mul/vimt3d/tests/test_v3i.cxx
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vul/vul_temp_filename.h>
#include <vil3d/vil3d_save.h>
#include <mbl/mbl_stl.h>
#include <vimt3d/vimt3d_load.h>
#include <testlib/testlib_test.h>
#include <vil3d/vil3d_save.h>
#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_image_resource.h>
#include <vimt3d/vimt3d_vil3d_v3i.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <vimt3d/vimt3d_add_all_loaders.h>


static void test_v3i()
{
  vcl_cout << "*********************************\n"
           << " Testing vimt3d_vil3d_v3i_format\n"
           << "*********************************\n";

  vimt3d_add_all_loaders();
  
  // create a simple single plane image
  vil3d_image_view<vxl_int_32> im1(3,4,5);
  mbl_stl_increments(im1.begin(), im1.end(), -2);
  
  // Try saving and laoding vil3d image.
  vcl_string fname1 = vul_temp_filename() + ".v3i";  
  TEST( "Succesfully saved simple v3i image",vil3d_save(im1,fname1.c_str()), true);
  vil3d_image_resource_sptr ir1 =   vil3d_load_image_resource(fname1.c_str());
  TEST( "Succesfully loaded simple v3i image",!ir1, false);
  
  TEST("Loaded simple image has identity for a transform",
    dynamic_cast<vimt3d_vil3d_v3i_image&>(*ir1).world2im().is_identity(),true);
  vpl_unlink(fname1.c_str());
  
  vil3d_image_view<float> im2(3,4,5,6);
  mbl_stl_increments(im2.begin(), im2.end(), -200.0f);
  vimt3d_transform_3d tr2;
  tr2.set_zoom_only(2.0, -5.0, -5.0, -5.0);
  vcl_string fname2 = vul_temp_filename() + ".v3i";    
  {
    vil3d_image_resource_sptr ir2 = vil3d_new_image_resource(
      fname2.c_str(), 3, 4, 5, 6, VIL_PIXEL_FORMAT_FLOAT, "v3i");
    TEST("Successfully openned image on disk", !ir2, false);

    ir2->put_view(im2);
    dynamic_cast<vimt3d_vil3d_v3i_image&>(*ir1).set_world2im(tr2);
    // Save image as ir2 closes.
  }
  
  vil3d_image_resource_sptr ir3 = vil3d_load_image_resource(fname2.c_str());
  TEST( "Succesfully loaded complicated v3i image",!ir3, false);
  vimt3d_image_3d_of<float> im3(ir3->get_view(), vimt3d_load_transform(ir1));
  TEST("Loaded complicated image has correct pixel values",
    vil3d_image_view_deep_equality(im3.image(), im2), true);
  TEST("Loaded complicated image has correct transform", im3.world2im(), tr2);
  vpl_unlink(fname2.c_str());
  
}

TESTMAIN(test_v3i);
