#include <vcl_fstream.h>

#include <vpl/vpl.h>
#include <vul/vul_temp_filename.h>

#include <vil/vil_load.h>
#include <vil/vil_image_impl.h>

#include <vil/vil_test.h>

static void test(char const* magic, int comps, int bits)
{
  char const* TMPNAM = vul_temp_filename().c_str();
  char const* FNAME = TMPNAM ? TMPNAM : "/tmp/t.pgm";
  {
    vcl_ofstream f(FNAME);
    // reference to rvalue not allowed.
    f << magic << "\n2\n3\n255\nABCDEF";
  }

  vil_image i = vil_load(FNAME);

  vcl_cout <<
    "vil_image_impl: size " << i.width() << "x" << i.height() <<
    ", " << i.components() << " component" <<
    ", " << i.bits_per_component() << " bit" <<
    vcl_endl;

  TEST("components", i.components(), comps);
  TEST("bits per component", i.bits_per_component(), bits);

  vpl_unlink(FNAME);
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
