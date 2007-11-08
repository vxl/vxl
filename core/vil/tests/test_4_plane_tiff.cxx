// This is core/vil/tests/test_4-plane_tiff.cxx
#include <testlib/testlib_test.h>
#include <vil/vil_image_view.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_rgba.h>
#include <vil/file_formats/vil_tiff.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_open.h>
#include <vil/vil_stream.h>
#include <vil/vil_new.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vpl/vpl.h> // vpl_unlink()
static void test_4_plane_tiff()
{

  vil_image_view<unsigned short> v(32, 32, 4);
  for(unsigned r = 0; r<32; r++)
    for(unsigned c = 0; c<32; c++)
      for(unsigned p = 0; p<4; p++)
        v(c, r, p) = (r+1)*(c+1)*(p+1);

  {
  vil_stream* os = vil_open("four_plane.tiff", "w");
  vil_blocked_image_resource_sptr out = 
    vil_new_blocked_image_resource(os, 32, 32,4,
                                   VIL_PIXEL_FORMAT_UINT_16,
                                   16, 16,                                   
                                   "tiff");
  out->put_view(v, 0, 0);
  }
  vil_image_resource_sptr in = vil_load_image_resource("four_plane.tiff");
  vil_image_view<unsigned short> iv = in->get_view();
  unsigned short v11 = iv(1,1,1);
  unsigned short v13 = iv(1,1,3);
  TEST("Four plane image read/write", v11+v13, 24);
  vpl_unlink("four_plane.tiff");
}

TESTMAIN(test_4_plane_tiff);
