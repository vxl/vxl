// This is mul/vimt3d/tests/test_v3m.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h>
#include <vul/vul_temp_filename.h>
#include <mbl/mbl_stl.h>
#include <vimt3d/vimt3d_load.h>
#include <testlib/testlib_test.h>
#include <vil3d/vil3d_save.h>
#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_property.h>
#include <vimt3d/vimt3d_vil3d_v3m.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <vimt3d/vimt3d_add_all_loaders.h>

static void test_v3m()
{
  std::cout << "*********************************\n"
           << " Testing vimt3d_vil3d_v3m_format\n"
           << "*********************************\n";

  vimt3d_add_all_loaders();

  // create a simple single plane image
  vil3d_image_view<vxl_int_32> im1(3,4,5);
  mbl_stl_increments(im1.begin(), im1.end(), -2);

  // Try saving and loading vil3d image.
  std::string fname1 = vul_temp_filename() + ".v3m";
  TEST( "Successfully saved simple v3m image",vil3d_save(im1, fname1.c_str()), true);
  vil3d_image_resource_sptr ir1 =   vil3d_load_image_resource(fname1.c_str());
  TEST( "Successfully loaded simple v3m image",!ir1, false);

  TEST("Loaded simple image has identity for a transform",
    dynamic_cast<vimt3d_vil3d_v3m_image&>(*ir1).world2im().is_identity(),true);
  vpl_unlink(fname1.c_str());

  vil3d_image_view<float> im2(3,4,5,6);
  mbl_stl_increments(im2.begin(), im2.end(), -200.0f);
  vimt3d_transform_3d tr2;
  tr2.set_zoom_only(2.0, -5.0, -5.0, -5.0);
  std::string fname2 = vul_temp_filename() + ".v3m";
  {
    vil3d_image_resource_sptr ir2 = vil3d_new_image_resource(
      fname2.c_str(), 3, 4, 5, 6, VIL_PIXEL_FORMAT_FLOAT, "v3m");
    TEST("Successfully opened image on disk", !ir2, false);

    ir2->put_view(im2);
    dynamic_cast<vimt3d_vil3d_v3m_image&>(*ir2).set_world2im(tr2);
    // Save image as ir2 is destroyed.

  }

  vil3d_image_resource_sptr ir3 = vil3d_load_image_resource(fname2.c_str());
  TEST("Successfully loaded complicated v3m image", !ir3, false);
  vimt3d_image_3d_of<float> im3(ir3->get_view(), vimt3d_load_transform(ir3));
  TEST("Loaded complicated image has correct pixel values",
    vil3d_image_view_deep_equality(im3.image(), im2), true);
  TEST("Loaded complicated image has correct transform", im3.world2im(), tr2);
  float size[3];
  TEST("get_property()", ir3->get_property(vil3d_property_voxel_size, size), true);
  TEST("vil3d_property_voxel_size is correct", size[0] == 0.5 &&
    size[1] == 0.5 && size[2] == 0.5, true);
  vpl_unlink(fname2.c_str());


  vil3d_image_view<float> im4(3,4,5,6);
  mbl_stl_increments(im4.begin(), im4.end(), -200.0f);

  std::string fname3 = vul_temp_filename() + ".v3m";
  {
    vil3d_image_resource_sptr ir4 = vil3d_new_image_resource(
      fname3.c_str(), 3, 4, 5, 6, VIL_PIXEL_FORMAT_FLOAT, "v3m");
    TEST("Successfully opened image on disk", !ir4, false);

    ir4->put_view(im4);
    // Start with one pixel size
    vimt3d_transform_3d tr4;
    tr4.set_zoom_only(2.0, -5.0, -5.0, -5.0);
    TEST("set_voxel_size()", ir4->set_voxel_size_mm(1.0f, 2.0f, 3.0f), true);
    // Save image as ir4 is destroyed.

  }

  vil3d_image_resource_sptr ir5 = vil3d_load_image_resource(fname3.c_str());
  TEST( "Successfully loaded complicated v3m image",!ir5, false);
  TEST( "Got expected image size with just headers", ir5->ni()==3 && ir5->nj()==4 && ir5->nk()==5 && ir5->nplanes()==6, true);
  vil3d_image_view<float> im5(ir5->get_view());
  TEST("Loaded complicated image has correct pixel values",
    vil3d_image_view_deep_equality(im3.image(), im2), true);
  TEST("get_property()", ir5->get_property(vil3d_property_voxel_size, size), true);
  TEST("vil3d_property_voxel_size is correct", size[0] == 0.001f &&
    size[1] == 0.002f && size[2] == 0.003f, true);

  vpl_unlink(fname3.c_str());

  // Try saving and loading empty vil3d image.
  vil3d_image_view<vxl_int_32> im6;
  std::string fname6 = vul_temp_filename() + ".v3m";
  TEST( "Successfully saved empty v3m image",vil3d_save(im6, fname6.c_str()), true);
  vil3d_image_resource_sptr ir6 = vil3d_load_image_resource(fname6.c_str());
  TEST( "Successfully loaded empty v3m image",!ir6, false);

  TEST("Loaded simple image has identity for a transform",
    dynamic_cast<vimt3d_vil3d_v3m_image&>(*ir6).world2im().is_identity(),true);
  TEST("Loaded image is empty", ir6->ni() + ir6->nj() + ir6->nk(), 0);
  vil3d_image_view<vxl_int_32> im7 = ir6->get_view();
  TEST("Loaded image is empty2", im7.size(), 0);

  vpl_unlink(fname6.c_str());

}

TESTMAIN(test_v3m);
