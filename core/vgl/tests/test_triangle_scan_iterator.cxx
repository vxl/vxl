#include <testlib/testlib_test.h>

#include <vcl_iostream.h>

#include <vgl/vgl_triangle_scan_iterator.h>
#include <vgl/vgl_polygon_scan_iterator.h>

static void test_triangle_scan_iterator()
{
  vcl_cout << "Test triangle scan iterator\n";

  float x[3] = { 10.23f, 20.12f, 30.73f };
  float y[3] = { 20.54f, 10.39f, 30.11f };

  vgl_triangle_scan_iterator ti;
  ti.a.x = x[0]; ti.a.y = y[0];
  ti.b.x = x[1]; ti.b.y = y[1];
  ti.c.x = x[2]; ti.c.y = y[2];

  vgl_polygon p(x, y, 3);
  vgl_polygon_scan_iterator pi(p, false);

  bool failed = false;
  ti.reset(); bool ti_more = ti.next();
  pi.reset(); bool pi_more = pi.next();

  while ( ti_more && pi_more && !failed )
  {
    failed = failed || ti.scany() != pi.scany() || ti.startx() != pi.startx() || ti.endx() != pi.endx();
    ti_more = ti.next();
    pi_more = pi.next();
  }
  if ( ti_more != pi.next() ) failed = true;

  TEST("Triangle scan iterator == polygon scan iterator", failed, false);
}

TESTMAIN(test_triangle_scan_iterator);
