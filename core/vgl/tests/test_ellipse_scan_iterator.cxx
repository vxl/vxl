#include <testlib/testlib_test.h>

#include <vgl/vgl_ellipse_scan_iterator.h>

static void test_circle()
{
  // Scan convert the unit circle
  vgl_ellipse_scan_iterator<double> circle1( 0, 0, 1, 1, 0 );
  TEST("unit circle", circle1.count(), 5 );

  vgl_ellipse_scan_iterator<double> circle2( 10, 10, 1, 1, 0 );
  TEST("offset unit circle", circle2.count(), 5 );

  vgl_ellipse_scan_iterator<double> circle3( 0, 0, 1, 1, 0.2 );
  TEST("rotated unit circle", circle3.count(), 5 );

  vgl_ellipse_scan_iterator<double> circle4( 0, 0, 3, 3, 0 );
  TEST("radius 3 circle", circle4.count(), 29 );

  vgl_ellipse_scan_iterator<double> circle5( -5, -5, 3, 3, 0 );
  TEST("offset radius 3 circle", circle5.count(), 29 );

  vgl_ellipse_scan_iterator<double> circle6( 20, 5, 3, 3, 3.6 );
  TEST("offset, rotated radius 3 circle", circle6.count(), 29 );
}

static void test_ellipse()
{
  // Scan convert simple ellipses
  vgl_ellipse_scan_iterator<float> ellipse1( 0, 0, 1, 2, 0 );
  TEST("centred ellipse radius 1,2", ellipse1.count(), 7 );

  vgl_ellipse_scan_iterator<float> ellipse2( 10, -10, 2, 1, 0 );
  TEST("offset ellipse radius 2,1", ellipse2.count(), 7 );

  vgl_ellipse_scan_iterator<float> ellipse3( 0, 0, 2, 1, 0.1 );
  TEST("centred, rotated ellipse radius 2,1", ellipse3.count(), 5 );

  vgl_ellipse_scan_iterator<float> ellipse4( 0, 0, 2, 3, -3.1415/4.0 );
  TEST("centred, rotated ellipse radius 2,3", ellipse4.count(), 19 );
}

static void test_sliver()
{
  vgl_ellipse_scan_iterator<double> ellipse1( 0, 0, 0.1, 3, 0 );
  TEST("vertical sliver: 0.1,3", ellipse1.count(), 7 );

  vgl_ellipse_scan_iterator<double> ellipse2( 0, 0, 2, 0.1, 0 );
  TEST("horizontal sliver: 2,0.1", ellipse2.count(), 5 );

  vgl_ellipse_scan_iterator<double> ellipse3( 0, 0, 2, 0.1, 0 );
  TEST("horizontal sliver: 2,0.1", ellipse3.count(), 5 );

  vgl_ellipse_scan_iterator<double> ellipse4( 0, 0, 0.1, 3, 1.4 );
  TEST("rotated sliver", ellipse4.count(), 1 );
}


static void test_degenerate()
{
vgl_ellipse_scan_iterator<float> ellipse1( 0.5, 0.5, 0.3, 0.1, 0 );
  TEST("empty ellipse", ellipse1.count(), 0 );

  vgl_ellipse_scan_iterator<float> ellipse2( 0, 0, 2, 0, 0 );
  TEST("horizontal line", ellipse2.count(), 5 );

  vgl_ellipse_scan_iterator<float> ellipse3( 0, 0, 0, 2, 0 );
  TEST("vertical line", ellipse3.count(), 5 );

  vgl_ellipse_scan_iterator<float> ellipse4( 0, 0, 0, 0, 0 );
  TEST("point at integer coordinates", ellipse4.count(), 1 );

  vgl_ellipse_scan_iterator<float> ellipse5( 0.1, 0.1, 0, 0, 0 );
  TEST("point at non-integer coordinates", ellipse5.count(), 0 );
}

static void test_ellipse_scan_iterator()
{
  test_circle();
  test_ellipse();
  test_sliver();
  test_degenerate();
}

TESTMAIN(test_ellipse_scan_iterator);
