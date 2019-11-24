// This is core/vil/tests/test_warp.cxx
#include "testlib/testlib_test.h"
#include "vil/vil_image_view.h"
#include "vil/vil_nearest_interp.h"
#include "vil/vil_warp.h"
#include "vil/vil_print.h"

static vxl_byte interpolator(vil_image_view<vxl_byte> const& view,
                             double x, double y, unsigned p)
{
  return vil_nearest_interp_safe(view, x, y, p);
}

void mapper(double ox, double oy, double &ix, double &iy)
{
  ix = oy;
  iy = -ox+1;
}

static void test_warp()
{
  vil_image_view<vxl_byte>  in(2,2);
  in(0,0) = 1;
  in(0,1) = 2;
  in(1,0) = 3;
  in(1,1) = 4;

  vil_image_view<vxl_byte>  out(2,3);
  vil_warp(in, out, mapper, interpolator);

  vil_print_all(std::cout, in);
  vil_print_all(std::cout, out);

  TEST("pixel 0,0", out(0,0), 2);
  TEST("pixel 0,1", out(0,1), 4);
  TEST("pixel 1,0", out(1,0), 1);
  TEST("pixel 1,1", out(1,1), 3);
  TEST("pixel 1,1", out(0,2), 0);
  TEST("pixel 1,1", out(1,2), 0);
}

TESTMAIN(test_warp);
