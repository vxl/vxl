#include <vgl/vgl_test.h>

#include <vgl/vgl_ellipse_scan_iterator.h>

int count( vgl_ellipse_scan_iterator& iter )
{
  int cnt = 0;
  iter.reset();
  while( iter.next() ) {
    if( iter.startx() > iter.endx() )
      return -1;
    cnt += iter.endx() - iter.startx() + 1;
  }
  return cnt;
}

void
test_circle()
{
  // Scan convert the unit circle
  vgl_test_begin("unit circle");
  vgl_ellipse_scan_iterator circle1( 0, 0, 1, 1, 0 );
  vgl_test_perform( count( circle1 ) == 5 );

  vgl_test_begin("offset unit circle");
  vgl_ellipse_scan_iterator circle2( 10, 10, 1, 1, 0 );
  vgl_test_perform( count( circle2 ) == 5 );

  vgl_test_begin("rotated unit circle");
  vgl_ellipse_scan_iterator circle3( 0, 0, 1, 1, 0.2 );
  vgl_test_perform( count( circle3 ) == 5 );

  vgl_test_begin("radius 3 circle");
  vgl_ellipse_scan_iterator circle4( 0, 0, 3, 3, 0 );
  vgl_test_perform( count( circle4 ) == 29 );

  vgl_test_begin("offset radius 3 circle");
  vgl_ellipse_scan_iterator circle5( -5, -5, 3, 3, 0 );
  vgl_test_perform( count( circle5 ) == 29 );

  vgl_test_begin("offset, rotated radius 3 circle");
  vgl_ellipse_scan_iterator circle6( 20, 5, 3, 3, 3.6 );
  vgl_test_perform( count( circle6 ) == 29 );
}

void
test_ellipse()
{
  // Scan convert simple ellipses
  vgl_test_begin("centred ellipse radius 1,2");
  {
    vgl_ellipse_scan_iterator ellipse( 0, 0, 1, 2, 0 );
    vgl_test_perform( count( ellipse ) == 7 );
  }

  vgl_test_begin("offset ellipse radius 2,1");
  {
    vgl_ellipse_scan_iterator ellipse( 10, -10, 2, 1, 0 );
    vgl_test_perform( count( ellipse ) == 7 );
  }

  vgl_test_begin("centred, rotated ellipse radius 2,1");
  {
    vgl_ellipse_scan_iterator ellipse( 0, 0, 2, 1, 0.1 );
    vgl_test_perform( count( ellipse ) == 5 );
  }

  vgl_test_begin("centred, rotated ellipse radius 2,3");
  {
    vgl_ellipse_scan_iterator ellipse( 0, 0, 2, 3, -3.1415/4.0 );
    vgl_test_perform( count( ellipse ) == 19 );
  }
}

void
test_sliver()
{
  vgl_test_begin("vertical sliver: 0.1,3");
  {
    vgl_ellipse_scan_iterator ellipse( 0, 0, 0.1, 3, 0 );
    vgl_test_perform( count( ellipse ) == 7 );
  }

  vgl_test_begin("horizontal sliver: 2,0.1");
  {
    vgl_ellipse_scan_iterator ellipse( 0, 0, 2, 0.1, 0 );
    vgl_test_perform( count( ellipse ) == 5 );
  }

  vgl_test_begin("horizontal sliver: 2,0.1");
  {
    vgl_ellipse_scan_iterator ellipse( 0, 0, 2, 0.1, 0 );
    vgl_test_perform( count( ellipse ) == 5 );
  }

  vgl_test_begin("rotated sliver");
  {
    vgl_ellipse_scan_iterator ellipse( 0, 0, 0.1, 3, 1.4 );
    vgl_test_perform( count( ellipse ) == 1 );
  }
}


void
test_degenerate()
{
  vgl_test_begin("empty ellipse");
  {
    vgl_ellipse_scan_iterator ellipse( 0.5, 0.5, 0.3, 0.1, 0 );
    vgl_test_perform( count( ellipse ) == 0 );
  }

  vgl_test_begin("horizontal line");
  {
    vgl_ellipse_scan_iterator ellipse( 0, 0, 2, 0, 0 );
    vgl_test_perform( count( ellipse ) == 5 );
  }

  vgl_test_begin("vertical line");
  {
    vgl_ellipse_scan_iterator ellipse( 0, 0, 0, 2, 0 );
    vgl_test_perform( count( ellipse ) == 5 );
  }

  vgl_test_begin("point at integer coordinates");
  {
    vgl_ellipse_scan_iterator ellipse( 0, 0, 0, 0, 0 );
    vgl_test_perform( count( ellipse ) == 1 );
  }

  vgl_test_begin("point at non-integer coordinates");
  {
    vgl_ellipse_scan_iterator ellipse( 0.1, 0.1, 0, 0, 0 );
    vgl_test_perform( count( ellipse ) == 0 );
  }
}

int main()
{
  vgl_test_start("Ellipse scan iterator");

  test_circle();
  test_ellipse();
  test_sliver();
  test_degenerate();

  return vgl_test_summary();
}
