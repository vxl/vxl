#include <vcl_fstream.h>
#include <vcl_vector.h>

#include <vpl/vpl.h>
#include <vul/vul_temp_filename.h>

#include <vil/vil_load.h>
#include <vil/vil_byte.h>
#include <vil/vil_rgb.h>
#include <vil/vil_image_impl.h>

#include <testlib/testlib_test.h>

static void test_rgb(char const *name, unsigned w, unsigned h,
                     unsigned r, unsigned g, unsigned b)
{
  vcl_string tmp_nam = vul_temp_filename() + ".pgm";

  vil_image i = vil_load(name);

  vcl_cout <<
    "vil_image_impl: size " << i.width() << "x" << i.height() <<
    ", " << i.components() << " component" <<
    ", " << i.bits_per_component() << " bit" <<
    vcl_endl;

  TEST("components", i.components(), 3);
  TEST("bits per component", i.bits_per_component(), 8);
  TEST("size", i.width() == w && i.height()==h, true);

  vcl_vector<vil_rgb<vil_byte> > image_buf(w*h);
  TEST ("get_section() on image", ! i.get_section(&image_buf[0], 0, 0, w, h), false);

  bool result = true;

  for (vcl_vector<vil_rgb<vil_byte> >::iterator
    it= image_buf.begin(); it!=image_buf.end(); ++it)
  {
    if (!(*it == vil_rgb<vil_byte>(r, g, b)))
      result = false;
  }

  TEST("Pixel values", result, true);
}

static void test_gray(char const *name, unsigned w, unsigned h,
                     unsigned v)
{
  vcl_string tmp_nam = vul_temp_filename() + ".pgm";

  vil_image i = vil_load(name);

  vcl_cout <<
    "vil_image_impl: size " << i.width() << "x" << i.height() <<
    ", " << i.components() << " component" <<
    ", " << i.bits_per_component() << " bit" <<
    vcl_endl;

  TEST("components", i.components(), 1);
  TEST("bits per component", i.bits_per_component(), 8);
  TEST("size", i.width() == w && i.height()==h, true);

  vcl_vector<vil_byte> image_buf(w*h);
  TEST ("get_section() on image", ! i.get_section(&image_buf[0], 0, 0, w, h), false);

  bool result = true;

  for (vcl_vector<vil_byte>::iterator
    it= image_buf.begin(); it!=image_buf.end(); ++it)
  {
    if (*it != v)
      result = false;
  }

  TEST("Pixel values", result, true);
}

MAIN( test_load_gen )
{
  START( "vil_load_gen" )
  test_rgb("gen:10x20:rgb,30,40,50", 10, 20, 30, 40, 50);
  test_gray("gen:10x20:gray,30", 10, 20, 30);
  SUMMARY();
}
