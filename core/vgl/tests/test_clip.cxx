#include <vgl/vgl_polygon.h>
#include <vgl/vgl_clip.h>

#include <testlib/testlib_test.h>

#include "test_driver.h"

static int num_cont( const vgl_polygon& poly ) { return poly.num_sheets(); }

static unsigned int num_vert( const vgl_polygon& poly )
{
  unsigned int count = 0;
  for ( int i=0; i < poly.num_sheets(); ++i )
    count += poly[i].size();
  return count;
}


static bool is_vertex( const vgl_polygon& poly, float x, float y )
{
  for ( int i=0; i < poly.num_sheets(); ++i )
    for ( unsigned int p=0; p < poly[i].size(); ++p )
      if ( close( x, poly[i][p].x() ) && close( y, poly[i][p].y() ) )
        return true;

  return false;
}


static void test_union()
{
  float cont1[] = { 0,0,  5,0,  5,5,  0,5 };
  vgl_polygon poly1 = new_polygon( cont1 );

  {
    vgl_polygon poly2;

    testlib_test_begin("union with null polygon");
    vgl_polygon result = vgl_clip( poly1, poly2, vgl_clip_type_union );
    testlib_test_perform( num_cont(result) == 1 &&
                          num_vert(result) == 4 );
  }

  {
    float cont2[] = { 6,0,  8,1,  6,2 };
    vgl_polygon poly2 = new_polygon( cont2 );

    testlib_test_begin("disjoint union");
    vgl_polygon result = vgl_clip( poly1, poly2, vgl_clip_type_union );
    testlib_test_perform( num_cont(result) == 2 &&
                          num_vert(result) == 7 );
  }

  {
    float cont2[] = { 4,1,  8,1,  8,6, 4,6 };
    vgl_polygon poly2 = new_polygon( cont2 );

    testlib_test_begin("overlapping union");
    vgl_polygon result = vgl_clip( poly1, poly2, vgl_clip_type_union );
    testlib_test_perform( num_cont(result) == 1 &&
                          num_vert(result) == 8 &&
                          is_vertex(result, 5,1) &&
                          is_vertex(result, 4,6) );
  }
}


static void test_intersection()
{
  float cont1[] = { 0,0,  5,0,  5,5,  0,5 };
  vgl_polygon poly1 = new_polygon( cont1 );

  {
    vgl_polygon poly2;

    testlib_test_begin("intersection with null polygon");
    vgl_polygon result = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    testlib_test_perform( num_cont(result) == 1 &&
                          num_vert(result) == 4 );
  }

  {
    float cont2[] = { 6,0,  8,1,  6,2 };
    vgl_polygon poly2 = new_polygon( cont2 );

    testlib_test_begin("disjoint simple intersection");
    vgl_polygon result = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    testlib_test_perform( num_cont(result) == 0 &&
                          num_vert(result) == 0 );
  }

  {
    float cont2[] = { 4,1,  8,1,  8,6, 4,6 };
    vgl_polygon poly2 = new_polygon( cont2 );

    testlib_test_begin("overlapping simple intersection");
    vgl_polygon result = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    testlib_test_perform( num_cont(result) == 1 &&
                          num_vert(result) == 4 &&
                          is_vertex(result, 4,1) );
  }

  {
    float cont2[] = { -3,-3,  8,-3,  8,8, -3,8 };
    float cont3[] = { -1,-1,  6,-1,  6,6, -1,6 };
    vgl_polygon poly2 = new_polygon( cont2 );
    add_contour( poly2, cont3 );

    testlib_test_begin("disjoint holey intersection");
    vgl_polygon result = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    testlib_test_perform( num_cont(result) == 0 &&
                          num_vert(result) == 0 );
  }
}


static void test_clip()
{
  test_union();
  test_intersection();
}

TESTMAIN(test_clip);
