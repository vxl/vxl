#include <cmath>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_clip.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <testlib/testlib_test.h>

template <class T> inline static bool is_vertex(vgl_polygon<T> const& poly, T x, T y)
{
  for ( unsigned int i=0; i < poly.num_sheets(); ++i )
    for ( unsigned int p=0; p < poly[i].size(); ++p )
      if ( std::abs(x - poly[i][p].x()) < 1e-6 && std::abs(y - poly[i][p].y()) < 1e-6 )
        return true;

  return false;
}


static void test_union()
{
  double cont1[] = { 0,0,  5,0,  5,5,  0,5 };
  vgl_polygon<double> poly1(cont1, 4);

  {
    vgl_polygon<double> poly2;

    vgl_polygon<double> result = vgl_clip( poly1, poly2, vgl_clip_type_union );
    TEST("union with null polygon 1",
         result.num_sheets() == 1 && result.num_vertices() == 4,  true);

    vgl_polygon<double> result2 = vgl_clip( poly2, poly1, vgl_clip_type_union );
    TEST("union with null polygon 2",
         result2.num_sheets() == 1 && result2.num_vertices() == 4, true);
  }

  {
    double cont2[] = { 6,0,  8,1,  6,2 };
    vgl_polygon<double> poly2(cont2, 3);

    vgl_polygon<double> result = vgl_clip( poly1, poly2, vgl_clip_type_union );
    TEST("disjoint union",
         result.num_sheets() == 2 && result.num_vertices() == 7, true);
  }

#ifdef BUILD_NONCOMMERCIAL
  {
    double cont2[] = { 4,1,  8,1,  8,6,  4,6 };
    vgl_polygon<double> poly2(cont2, 4);

    vgl_polygon<double> result = vgl_clip( poly1, poly2, vgl_clip_type_union );
    TEST("overlapping union",
         result.num_sheets() == 1 && result.num_vertices() == 8 &&
         is_vertex(result, 5.0,1.0) && is_vertex(result, 4.0,6.0), true);
  }
#endif // BUILD_NONCOMMERCIAL
}


static void test_intersection()
{
  float cont1[] = { 0,0,  5,0,  5,5,  0,5 };
  vgl_polygon<float> poly1(cont1, 4);

  {
    vgl_polygon<float> poly2;

    vgl_polygon<float> result = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    TEST("intersection with null polygon 1",
         result.num_sheets() == 0 && result.num_vertices() == 0, true);

#if 0  //This test is identical to the previous.  Why?
    vgl_polygon<float> result2 = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    TEST("intersection with null polygon 2",
         result2.num_sheets() == 0 && result2.num_vertices() == 0, true);
#endif
  }
  {
    float cont2[] = { 6,0,  8,1,  6,2 };
    vgl_polygon<float> poly2(cont2, 3);

    vgl_polygon<float> result = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    TEST("disjoint simple intersection",
         result.num_sheets() == 0 && result.num_vertices() == 0, true);
  }

#ifdef BUILD_NONCOMMERCIAL
  {
    float cont2[] = { 4,1,  8,1,  8,6,  4,6 };
    vgl_polygon<float> poly2(cont2, 4);

    vgl_polygon<float> result = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    TEST("overlapping simple intersection",
         result.num_sheets() == 1 && result.num_vertices() == 4 &&
         is_vertex(result, 4.f,1.f), true);
  }
#endif // BUILD_NONCOMMERCIAL

  {
    float cont2[] = { -3,-3,  8,-3,  8,8,  -3,8 };
    float cont3[] = { -1,-1,  6,-1,  6,6,  -1,6 };
    vgl_polygon<float> poly2(cont2, 4);
    poly2.add_contour(cont3, 4);

    vgl_polygon<float> result = vgl_clip( poly1, poly2, vgl_clip_type_intersect );
    TEST("disjoint holey intersection",
         result.num_sheets() == 0 && result.num_vertices() == 0, true);
  }
}

static void test_clip()
{
  test_union();
  test_intersection();
}

TESTMAIN(test_clip);
