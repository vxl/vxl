//:
// \file
// \brief Testing I/O of all possible PNM file formats
// \author Peter Vanroose

#include <vcl_fstream.h>

#include <vpl/vpl.h>
#include <vul/vul_temp_filename.h>

#include <vil/vil_load.h>
#include <vil/vil_image.h>

#include <testlib/testlib_test.h>

static void test(char const* magic, int comps, int bits, int maxval)
{
  vcl_string tmp_nam = vul_temp_filename() + ".pgm";
  char const *file = tmp_nam!=".pgm" ? tmp_nam.c_str() : "t.pgm";
  {
    vcl_ofstream f(file);
#ifdef LEAVE_IMAGES_BEHIND
      vpl_chmod(file, 0666); // -rw-rw-rw-
#endif
    f << magic << "\n2\n3\n";
    if (maxval > 1) f << maxval << "\n";
    if (magic[1] > '3')
      f << "ABCDEFGHIJKLMNOPQRSTUVWX"; // 24 bytes: sufficient for 32bit ppm
    else
      for (int i=0; i<24; ++i) f << 5+7*i+9*i*i << '\n';
  }

  vil_image i = vil_load(file);

  if (i) {
    vcl_cout << "test vil_load: size " << i.width() << "x" << i.height() << ", "
             << i.components() << " component(s)" << ", "
             << i.bits_per_component() << " bits\n";

    TEST("width", i.width(), 2);
    TEST("height", i.height(), 3);
    TEST("planes", i.planes(), 1);
    TEST("components", i.components(), comps);
    TEST("bits per component", i.bits_per_component(), bits);
    TEST("get_property(\"memory\")", i.get_property("memory"), false); 
    TEST("get_property(\"top row first\")", i.get_property("top row first"), true); 
    TEST("get_property(\"left first\")", i.get_property("left first"), true); 
    TEST("get_property(\"component order is B,G,R\")", i.get_property("component order is B,G,R"), false); 
    char buf[24];
    TEST("get_plane(0)", i.get_plane(0), i); 
    TEST("get_section()", i.get_section(buf, 0, 0, 2, 3) != 0, true); 
    if (magic[1] > '3')
    {
      int j=0; for (; 8*j < 6*i.bits_per_component(); ++j) if (buf[j] != j+'A') break;
      vcl_cout << j << '\n';
    }
    else
    {
      int j=0; for (; 8*j < 6*i.bits_per_component(); ++j) if (buf[j] != 5+7*j+9*j*j) break;
      vcl_cout << j << '\n';
    }
  } else {
    TEST("loading temp file", false, true);
    vcl_cerr << "Failed to load " << file << vcl_endl;
  }

#ifndef LEAVE_IMAGES_BEHIND
  vpl_unlink(file);
#endif
}

MAIN( test_load )
{
  START( "vil_load" )
  test("P1", 1,  1, 1);
  test("P2", 1,  8, 255);
  test("P2", 1, 16, 65535);
  test("P2", 1, 32, 16777216);
  test("P3", 3,  8, 255);
#if 0 // These give a bus error on some platforms when vil_image i is destructed
  test("P3", 3, 16, 65535);
  test("P3", 3, 32, 16777216);
#endif
  test("P4", 1,  1, 1);
  test("P5", 1,  8, 255);
  test("P5", 1, 16, 65535);
  test("P6", 3,  8, 255);
  test("P6", 3, 16, 65535);
  SUMMARY();
}
