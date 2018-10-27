// This is brl/bseg/sdet/tests/test_selective_search.cxx
#include <string>
#include <map>
#include <fstream>
#include <sdet/sdet_graph_img_seg.h>
#include <sdet/sdet_selective_search.h>
#include <sdet/sdet_region.h>
#include <sdet/sdet_region_sptr.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vgl/vgl_area.h>
#include <testlib/testlib_test.h>

static void test_selective_search(int argc, char * argv[])
{

  vil_image_view<vxl_byte> img(4,4);
  img.fill(0); img(1,2) = 255;  img(2,2) = 255;
  vil_image_view<vil_rgb<vxl_byte> > out_img;
  sdet_segment_img<vxl_byte>(img, 1, 8, (vxl_byte)100, 0, 0, out_img);
  TEST("out img pix 0 is not the same as pix 1", out_img(1,1) != out_img(1,2), true);
  TEST("out img pix 0 is not the same as pix 1", out_img(1,1) == out_img(2,1), true);
  TEST("out img pix 0 is not the same as pix 1", out_img(2,2) != out_img(2,1), true);
  TEST("out img pix 0 is not the same as pix 1", out_img(2,2) == out_img(1,2), true);

}

TESTMAIN_ARGS(test_selective_search);
