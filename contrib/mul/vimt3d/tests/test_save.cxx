// This is mul/vimt3d/tests/test_save.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  std::cout << "******************************\n"
           << " Testing vimt3d_save_transform\n"
           << "******************************\n";

  vimt3d_add_all_loaders();


  vimt3d_transform_3d tr_gold1;
  tr_gold1.set_zoom_only( 0.1, 0.2, 0.3, 4, 5, 6);
  vimt3d_transform_3d tr_gold2;
  tr_gold2.set_zoom_only( 0.1, 0.2, 0.3, 0, 0, 0);  // To test formats which can't cope with translation

  {
    vil3d_image_resource_sptr ir_out1 = vil3d_new_image_resource(
      "test_save.v3i", 5, 5, 5, 1, VIL_PIXEL_FORMAT_FLOAT, "v3i");
    vimt3d_save_transform(ir_out1, tr_gold1, true);
  }

  vil3d_image_resource_sptr im_in1 = vil3d_load_image_resource("test_save.v3i");
  vimt3d_transform_3d tr_in1 = vimt3d_load_transform(im_in1, true);


  std::cout << "Golden transform: " << tr_gold1
           << "\nLoaded transform: " << tr_in1 << std::endl;

  TEST("v3i image round-trip has same zoom transform",
       mbl_test_summaries_are_equal(tr_in1, tr_gold1 ), true);


  {
    vil3d_image_resource_sptr ir_out1 = vil3d_new_image_resource(
      "test_save.v3m", 5, 5, 5, 1, VIL_PIXEL_FORMAT_FLOAT, "v3m");
    vimt3d_save_transform(ir_out1, tr_gold1, true);
  }

  vil3d_image_resource_sptr im_in2 = vil3d_load_image_resource("test_save.v3m");
  vimt3d_transform_3d tr_in2 = vimt3d_load_transform(im_in2, true);


  std::cout << "Golden transform: " << tr_gold1
           << "\nLoaded transform: " << tr_in2 << std::endl;

  TEST("v3m image round-trip has same zoom transform",
       mbl_test_summaries_are_equal(tr_in2, tr_gold1 ), true);

  std::cout << "*********************\n"
           << " Testing vimt3d_save\n"
           << "*********************\n";

  vimt3d_image_3d_of<vxl_byte> image;
  image.image().set_size(2,3,4);
  image.set_world2im(tr_gold1);
  bool use_mm=true;

  vimt3d_save("test_save2.v3i",image,use_mm);
  {
    vimt3d_image_3d_of<vxl_byte> loaded_image;
    vimt3d_load("test_save2.v3i",loaded_image,use_mm);
    TEST("v3i image round-trip has same zoom transform",
         mbl_test_summaries_are_equal(loaded_image.world2im(), tr_gold1 ), true);
  }

  vimt3d_save("test_save2.v3m",image,use_mm);
  {
    vimt3d_image_3d_of<vxl_byte> loaded_image;
    vimt3d_load("test_save2.v3m",loaded_image,use_mm);
    TEST("v3m image round-trip has same zoom transform",
         mbl_test_summaries_are_equal(loaded_image.world2im(), tr_gold1 ), true);
  }

  image.set_world2im(tr_gold2);  // hdr can't currently cope with translation
  vimt3d_save("test_save2.hdr",image,use_mm);
  {
    vimt3d_image_3d_of<vxl_byte> loaded_image;
    vimt3d_load("test_save2.hdr",loaded_image,use_mm);
    TEST("hdr image round-trip has same zoom transform",
         mbl_test_summaries_are_equal(loaded_image.world2im(), image.world2im() ), true);
  }

// There is an inconsistency in the way that gipl deals with the origin
  image.set_world2im(tr_gold2);
  vimt3d_save("test_save2.gipl",image,use_mm);
  {
    vimt3d_image_3d_of<vxl_byte> loaded_image;
    vimt3d_load("test_save2.gipl",loaded_image,use_mm);
    std::cout<<"gipl loaded: "<<loaded_image<<std::endl;
    TEST("gipl image round-trip has same zoom transform",
         mbl_test_summaries_are_equal(loaded_image.world2im(), image.world2im() ), true);
  }
}

TESTMAIN(test_save);
