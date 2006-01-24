// This is mul/vil3d/tests/test_gen_synthetic.cxx
#include <vcl_functional.h>
#include <vcl_algorithm.h>

#include <vxl_config.h> // for vxl_uint_16 etc.

#include <testlib/testlib_test.h>

#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/file_formats/vil3d_gen_synthetic.h>

// \author Ian Scott
// \date Jan 2006


template <class T>
inline bool all_pixels_are(const vil3d_image_view<T> & im, T v)
{
  return vcl_find_if(im.begin(), im.end(), vcl_bind2nd(vcl_not_equal_to<T>(), v))==im.end();
}
void test_gen_synthetic( )
{
  
  vil3d_file_format::add_format(new vil3d_gen_synthetic_format);

  {
    vil3d_image_view<vxl_byte> im = vil3d_load("gen:5x6x7:vxl_byte:128");
    TEST("gen:5x6x7:vxl_byte:128 correct size",
      im.ni()==5 && im.nj()==6 && im.nk()==7 && im.nplanes()==1, true);
    TEST("image is compact", im.is_contiguous(), true);
    TEST("gen:5x6x7:vxl_byte:128 correct value",
      all_pixels_are<vxl_byte>(im, 128), true);
  }
  {
    vil3d_image_view<int> im = vil3d_load("gen:10x12x14:vxl_int_32:128");
    TEST("gen:10x12x14:vxl_int_32:128 correct size",
      im.ni()==10 && im.nj()==12 && im.nk()==14 && im.nplanes()==1, true);
    TEST("image is compact", im.is_contiguous(), true);
    TEST("gen:10x12x14:vxl_int_32:128 correct value",
      all_pixels_are(im, 128), true);
  }
  {
    vil3d_image_view<int> im = vil3d_load("gen:10x12x14:vxl_int_32:-128");
    TEST("gen:10x12x14:vxl_int_32:-128 correct size",
      im.ni()==10 && im.nj()==12 && im.nk()==14 && im.nplanes()==1, true);
    TEST("image is compact", im.is_contiguous(), true);
    TEST("gen:10x12x14:vxl_int_32:-128 correct value",
      all_pixels_are(im, -128), true);
  }
  {
    vil3d_image_view<float> im = vil3d_load("gen:10x12x14:float:-1.5");
    TEST("gen:10x12x14:float:-1.5 correct size",
      im.ni()==10 && im.nj()==12 && im.nk()==14 && im.nplanes()==1, true);
    TEST("image is compact", im.is_contiguous(), true);
    TEST("gen:10x12x14:float:-1.5 correct value",
      all_pixels_are(im, -1.5f), true);
  }
}

TESTMAIN(test_gen_synthetic);
