// This is mul/vil3d/tests/test_analyze_format.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vil3d/file_formats/vil3d_analyze_format.h>
#include <vil3d/vil3d_math.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_print.h>
#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_property.h>
#include <vil3d/vil3d_save.h>

template <class T>
inline void test_analyze_format(const vcl_string& dir,vcl_string type, vil_pixel_format format, T /*dummy*/)
{
  vcl_cout << "******************************************************************\n"
           << " Testing vil3d_analyze_format objects with pixel type = " << type << '\n'
           << "******************************************************************\n";

  vcl_string filepath=dir+"/ff_"+type+".hdr";
  vil3d_image_resource_sptr image_res = vil3d_load_image_resource(filepath.c_str());
  TEST("Load header successful",image_res!=0,true);
  if (image_res==0) return;
  TEST("Resource image format",image_res->pixel_format(),format);
  TEST("ni",image_res->ni(),5);
  TEST("nj",image_res->nj(),4);
  TEST("nk",image_res->nk(),3);

  float width[3] = { 0.0f, 0.0f, 0.0f };
  TEST("Got widths",image_res->get_property(vil3d_property_voxel_size, width),true);
  // Distances in metres
  TEST_NEAR("Width i",width[0]*1000,0.8f,1e-4);
  TEST_NEAR("Width j",width[1]*1000,1.5f,1e-4);
  TEST_NEAR("Width k",width[2]*1000,2.5f,1e-4);

  vil3d_image_view_base_sptr image = image_res->get_view(0, image_res->ni(),
                                                         0, image_res->nj(),
                                                         0, image_res->nk());
  TEST("Loaded an image",image!=0,true);
  TEST("Image type",image->pixel_format(),format);
  vil3d_image_view<T> im=static_cast<vil3d_image_view<T>&>(*image);
  TEST_NEAR("Pixel value (0,0,0)",im(0,0,0),0,1e-6);
  TEST_NEAR("Pixel value (1,0,0)",im(1,0,0),1,1e-6);
  TEST_NEAR("Pixel value (0,1,0)",im(0,1,0),5,1e-6);
  TEST_NEAR("Pixel value (0,0,1)",im(0,0,1),25,1e-6);
  TEST_NEAR("Pixel value (4,3,2)",im(4,3,2),69,1e-6);

  // Test saving the file
  vcl_string out_path="./analyze_format_out.hdr";
  bool save_ok= vil3d_save(im,out_path.c_str());
  TEST("Save successful",save_ok,true);
  if (save_ok)
  {
    vil3d_image_view<T> in_im = vil3d_load(out_path.c_str());
    TEST("Loaded==Saved",vil3d_image_view_deep_equality(in_im,im),true);
  }
}

static void test_analyze_format(int argc, char* argv[])
{
  vcl_string dir(".");
  if ( argc >= 2 ) dir=argv[1];

  test_analyze_format(dir,"float32", VIL_PIXEL_FORMAT_FLOAT, float());
  test_analyze_format(dir,"float32_le", VIL_PIXEL_FORMAT_FLOAT, float());
  test_analyze_format(dir,"byte", VIL_PIXEL_FORMAT_BYTE, vxl_byte());
  test_analyze_format(dir,"byte_le", VIL_PIXEL_FORMAT_BYTE, vxl_byte());
  test_analyze_format(dir,"int16", VIL_PIXEL_FORMAT_INT_16, vxl_int_16());
  test_analyze_format(dir,"int16_le", VIL_PIXEL_FORMAT_INT_16, vxl_int_16());
  test_analyze_format(dir,"int32", VIL_PIXEL_FORMAT_INT_32, vxl_int_32());
  test_analyze_format(dir,"int32_le", VIL_PIXEL_FORMAT_INT_32, vxl_int_32());
}

TESTMAIN_ARGS(test_analyze_format);
