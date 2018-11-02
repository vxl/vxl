#include <iostream>
#include <testlib/testlib_test.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_triangle_scan_iterator.h>
#include <vgl/vgl_polygon_scan_iterator.h>

static void test_triangle_scan_iterator()
{
  std::cout << "Test triangle scan iterator\n";

  {
    float x[3] = { 10.23f, 20.12f, 30.73f };
    float y[3] = { 20.54f, 10.39f, 30.11f };

    vgl_triangle_scan_iterator<float> ti;
    ti.a.x = x[0]; ti.a.y = y[0];
    ti.b.x = x[1]; ti.b.y = y[1];
    ti.c.x = x[2]; ti.c.y = y[2];

    vgl_polygon<float> p(x, y, 3);
    vgl_polygon_scan_iterator<float> pi(p, false);

    bool failed = false;
    ti.reset(); bool ti_more = ti.next();
    pi.reset(); bool pi_more = pi.next();

    while ( ti_more && pi_more && !failed )
    {
      failed = failed || ti.scany() != pi.scany() || ti.startx() != pi.startx() || ti.endx() != pi.endx();
      ti_more = ti.next();
      pi_more = pi.next();
    }

    // additional empty scan lines are allowed
    while(ti_more){
      failed = failed || ti.startx() <= ti.endx();
      ti_more = ti.next();
    }

    // additional empty scan lines are allowed
    while(pi_more){
      failed = failed || pi.startx() <= pi.endx();
      pi_more = pi.next();
    }

    TEST("Triangle scan iterator == polygon scan iterator", failed, false);
  }

  {
    float x[3] = { 0.5f, 2.5f, 3.1f };
    float y[3] = { 0.5f, 0.5f, 5.2f };
    // .XX.   This triangle rasterizes correctly
    // ..X.   with two connected components!
    // ..X.
    // ....  <= this scan line has no pixels
    // ...X
    int points[5][2] = { {1,1},{2,1},{2,2},{2,3},{3,5} };

    vgl_triangle_scan_iterator<float> ti;
    ti.a.x = x[0]; ti.a.y = y[0];
    ti.b.x = x[1]; ti.b.y = y[1];
    ti.c.x = x[2]; ti.c.y = y[2];

    bool pass = true;
    int i=0;
    for(ti.reset(); ti.next();){
      int y = ti.scany();
      for (int x = ti.startx(); x <= ti.endx(); ++x,++i){
        pass = pass && (points[i][0] == x) && (points[i][1] == y);
      }
    }
    TEST("triangle with gap", pass, true);
  }
}

TESTMAIN(test_triangle_scan_iterator);
