#include <vcl_fstream.h>

#include <vpl/vpl.h>
#include <vul/vul_temp_filename.h>

#include <vil/vil_load.h>
#include <vil/vil_image_impl.h>

#include <vil/vil_test.h>

static void test(char const* magic, int comps, int bits)
{
  vcl_string tmp_nam = vul_temp_filename() + ".pgm";
  char const *file = tmp_nam!="" ? tmp_nam.c_str() : "t.pgm";
  {
    vcl_ofstream f(file);
#ifdef LEAVE_IMAGES_BEHIND
      vpl_chmod(file, 0666); // -rw-rw-rw-
#endif
    f << magic << "\n2\n3\n255\nABCDEF";
  }

  vil_image i = vil_load(file);

  vcl_cout <<
    "vil_image_impl: size " << i.width() << "x" << i.height() <<
    ", " << i.components() << " component" <<
    ", " << i.bits_per_component() << " bit" <<
    vcl_endl;

  TEST("components", i.components(), comps);
  TEST("bits per component", i.bits_per_component(), bits);

#ifndef LEAVE_IMAGES_BEHIND
  vpl_unlink(file);
#endif
}

void test_load_pnm()
{
  test("P1", 1, 1);
  test("P2", 1, 8);
  test("P3", 3, 8);
  test("P4", 1, 1);
  test("P5", 1, 8);
  test("P6", 3, 8);
}

TESTMAIN(test_load_pnm);
