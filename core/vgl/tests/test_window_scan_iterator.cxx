#include <testlib/testlib_test.h>
#include <vgl/vgl_window_scan_iterator.h>

static void test_window_scan_iterator()
{
  // rectangular window from point (1.5,0.5) to (-1.5,3.5)
  double x1=1.5,y1=0.5, x2=-1.5,y2=3.5; // note that x1 > x2 !
  vgl_window_scan_iterator<double> wsi(x1,y1, x2,y2);
  wsi.reset();
  TEST("reset()", wsi.scany()<y1, true);
  TEST("next()", wsi.next(), true);
  TEST("scany()", wsi.scany(), 1);
  TEST("startx()", wsi.startx(), -1);
  TEST("endx()", wsi.endx(), 1);
  TEST("last scanline", wsi.next() && wsi.next() && !wsi.next(), true);
}

TESTMAIN(test_window_scan_iterator);
