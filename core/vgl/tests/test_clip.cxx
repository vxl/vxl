#include <vgl/vgl_polygon.h>
#include <vgl/vgl_clip.h>
#include <vcl_cmath.h> // for abs(float/double)

#include <testlib/testlib_test.h>

template <class T> inline static bool is_vertex(vgl_polygon<T> const& poly, T x, T y)
{
  for ( unsigned int i=0; i < poly.num_sheets(); ++i )
    for ( unsigned int p=0; p < poly[i].size(); ++p )
      if ( vcl_abs(x - poly[i][p].x()) < 1e-6 && vcl_abs(y - poly[i][p].y()) < 1e-6 )
        return true;

  return false;
}


static void test_union()
{
  double cont1[] = { 0,0,  5,0,  5,5,  0,5 };
  vgl_polygon<double> poly1(cont1, 4);

  {
    vgl_polygon<double> poly2;

    testlib_test_begin("union with null polygon 1");
    vgl_polygon<double> result = vgl_clip( poly1, poly2, vgl_clip_type_union );
    testlib_test_perform( result.num_sheets() == 1 &&
                          result.num_vertices() == 4 );

    testlib_test_begin("union with null polygon 2");
    vgl_polygon<double> result2 = vgl_clip( poly2, poly1, vgl_clip_type_union );
    testlib_test_perform( result2.num_sheets() == 1 &&
                          result2.num_vertices() == 4 );
  }

  {
    double cont2[] = { 6,0,  8,1,  6,2 };
    vgl_polygon<double> poly2(cont2, 3);

    testlib_test_begin("disjoint union");
    vgl_polygon<double> result = vgl_clip( poly1, poly2, vgl_clip_type_union );
    testlib_test_perform( result.num_sheets() == 2 &&
                          result.num_vertices() == 7 );
  }

  {
    double cont2[] = { 4,1,  8,1,  8,6,  4,6 };
    vgl_polygon<double> poly2(cont2, 4);

    testlib_test_begin("overlapping union");
    vgl_polygon<double> result = vgl_clip( poly1, poly2, vgl_clip_type_union );
    testlib_test_perform( result.num_sheets() == 1 &&
                          result.num_vertices() == 8 &&
                          is_vertex(result, 5.0,1.0) &&
                          is_vertex(result, 4.0,6.0) );
  }
}


static void test_intersection()
{
  float cont1[] = { 0,0,  5,0,  5,5,  0,5 };
  vgl_polygon<float> poly1(cont1, 4);

  {
    vgl_polygon<float> poly2;

    testlib_test_begin("intersection with null polygon 1");
    vgl_polygon<float> result = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    testlib_test_perform( result.num_sheets() == 0 &&
                          result.num_vertices() == 0);
#if 0  //This test is identical to the previous.  Why?
    testlib_test_begin("intersection with null polygon 2");
    vgl_polygon<float> result2 = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    testlib_test_perform( result2.num_sheets() == 0 &&
                          result2.num_vertices() == 0 );
#endif 
  }
  {
    float cont2[] = { 6,0,  8,1,  6,2 };
    vgl_polygon<float> poly2(cont2, 3);

    testlib_test_begin("disjoint simple intersection");
    vgl_polygon<float> result = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    testlib_test_perform( result.num_sheets() == 0 &&
                          result.num_vertices() == 0 );
  }

  {
    float cont2[] = { 4,1,  8,1,  8,6,  4,6 };
    vgl_polygon<float> poly2(cont2, 4);

    testlib_test_begin("overlapping simple intersection");
    vgl_polygon<float> result = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    testlib_test_perform( result.num_sheets() == 1 &&
                          result.num_vertices() == 4 &&
                          is_vertex(result, 4.f,1.f) );
  }

  {
    float cont2[] = { -3,-3,  8,-3,  8,8,  -3,8 };
    float cont3[] = { -1,-1,  6,-1,  6,6,  -1,6 };
    vgl_polygon<float> poly2(cont2, 4);
    poly2.add_contour(cont3, 4);

    testlib_test_begin("disjoint holey intersection");
    vgl_polygon<float> result = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    testlib_test_perform( result.num_sheets() == 0 &&
                          result.num_vertices() == 0 );
  }
}


static void test_clip()
{
  test_union();
  test_intersection();
}

TESTMAIN(test_clip);
