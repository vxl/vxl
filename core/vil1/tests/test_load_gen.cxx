// This is core/vil1/tests/test_load_gen.cxx
#include <vcl_vector.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_byte.h>
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_image_impl.h>
#include <testlib/testlib_test.h>

static void test_rgb(char const *name, int w, int h,
                     unsigned r, unsigned g, unsigned b)
{
  vil1_image i = vil1_load(name);

  vcl_cout << "vil1_image_impl: size " << i.width() << 'x' << i.height() << ", "
           << i.components() << " component, " << i.bits_per_component() << " bit\n";

  TEST("width", i.width(), w);
  TEST("height", i.height(), h);
  TEST("size", i.get_size_bytes(), 3*w*h);
  TEST("# planes", i.planes(), 1);
  TEST("# components", i.components(), 3);
  TEST("# bits per component", i.bits_per_component(), 8);
  TEST("component format", i.component_format(), VIL1_COMPONENT_FORMAT_UNSIGNED_INT);

  vcl_vector<vil1_rgb<vil1_byte> > image_buf(w*h);
  TEST("get_section() on image", i.get_section(&image_buf[0], 0, 0, w, h), true);

  bool result = true;

  for (vcl_vector<vil1_rgb<vil1_byte> >::iterator
       it= image_buf.begin(); it!=image_buf.end(); ++it)
    if (!(*it == vil1_rgb<vil1_byte>(r, g, b)))
    {
      result = false; break;
    }

  TEST("Pixel values", result, true);
}

static void test_gray(char const *name, int w, int h, unsigned v)
{
  vil1_image i = vil1_load(name);

  vcl_cout << "vil1_image_impl: size " << i.width() << 'x' << i.height() << ", "
           << i.components() << " component, " << i.bits_per_component() << " bit\n";

  TEST("width", i.width(), w);
  TEST("height", i.height(), h);
  TEST("size", i.get_size_bytes(), w*h);
  TEST("# planes", i.planes(), 1);
  TEST("# components", i.components(), 1);
  TEST("# bits per component", i.bits_per_component(), 8);
  TEST("component format", i.component_format(), VIL1_COMPONENT_FORMAT_UNSIGNED_INT);

  vcl_vector<vil1_byte> image_buf(w*h);
  TEST ("get_section() on image", i.get_section(&image_buf[0], 0, 0, w, h), true);

  bool result = true;

  for (vcl_vector<vil1_byte>::iterator
       it= image_buf.begin(); it!=image_buf.end(); ++it)
    if (*it != v)
    {
      result = false; break;
    }

  TEST("Pixel values", result, true);
}

MAIN( test_load_gen )
{
  START( "vil1_load_gen" );
  test_rgb("gen:10x20:rgb,30,40,50", 10, 20, 30, 40, 50);
  test_gray("gen:10x20:gray,30", 10, 20, 30);
  SUMMARY();
}
