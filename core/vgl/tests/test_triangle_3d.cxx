

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_ctime.h>
#include <vgl/vgl_triangle_3d.h>


//==================================================================================
inline void test_non_intersecting()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1(0, 0, 2);
  vgl_point_3d<double> b_p2(0, 4, 2);
  vgl_point_3d<double> b_p3(4, 0, 2);

  vgl_triangle_3d_intersection_t ret = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);
  TEST("Non-intersecting 1", ret, None);

  b_p1.set(-1,0,0);
  b_p2.set(-1,4,0);
  b_p3.set(-4,0,0);

  ret = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);
  TEST("Non-intersecting 2", ret, None);
}


//==================================================================================
inline void test_intersecting1()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1(0, 0, 0);
  vgl_point_3d<double> b_p2(0, 4, 0);
  vgl_point_3d<double> b_p3(4, 4, 0);

  vgl_triangle_3d_intersection_t ret = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);
  TEST("Intersecting coplanar", ret, Coplanar);
}


//==================================================================================
inline void test_intersecting2()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1(1, 1, 4);
  vgl_point_3d<double> b_p2(1, 1,-4);
  vgl_point_3d<double> b_p3(0, 0, 0);

  vgl_triangle_3d_intersection_t ret = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);
  TEST("Intersecting Skew", ret, Skew);
}


//==================================================================================
inline void test_intersecting3()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1(0, 0, 0);
  vgl_point_3d<double> b_p2(0, 4, 0);
  vgl_point_3d<double> b_p3(0, 0, 4);

  vgl_line_segment_3d<double> i_line;
  vgl_triangle_3d_intersection_t ret = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3,i_line);

  vgl_line_segment_3d<double> exp_line(vgl_point_3d<double>(0,0,0), vgl_point_3d<double>(0,4,0));
  bool intline_correct = i_line == exp_line;

  TEST("Intersecting Skew - w/isect line", ret, Skew);
  TEST("Intersecting Skew - isect line correct", intline_correct, true);
}


//==================================================================================
inline void test_intersecting4()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1( 0, 0, 0);
  vgl_point_3d<double> b_p2( 0, 4, 0);
  vgl_point_3d<double> b_p3(-4, 0, 0);

  vgl_line_segment_3d<double> i_line;
  vgl_triangle_3d_intersection_t ret = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3,i_line);

  TEST("Intersecting coplanar - w/isect line", ret, Coplanar);
  //TEST("Intersecting 4b", coplanar, true);
  //TEST("Intersecting 4c", intline_correct, true);
}





//==================================================================================
inline void test_intersecting_degenerate_triangles1()
{
  //the valid triangle
  vgl_point_3d<double> valid_p1(0, 0, 0);
  vgl_point_3d<double> valid_p2(0, 4, 0);
  vgl_point_3d<double> valid_p3(4, 0, 0);

  //Test on a number of different types of degenerate triangle

  const unsigned NUM_TESTS = 6;

  vgl_point_3d<double> degen[][NUM_TESTS] = {
    { vgl_point_3d<double>(-4, 5, 0), vgl_point_3d<double>(-3, 4, 0), vgl_point_3d<double>(-2, 0, 0) }, //Coplanar non-intersecting
    { vgl_point_3d<double>(2, 5, 0), vgl_point_3d<double>(2, 1, 0), vgl_point_3d<double>(2, -3, 0) },   //Coplanar intersecting
    { vgl_point_3d<double>(0, 8, 0), vgl_point_3d<double>(0, 6, 0), vgl_point_3d<double>(0, 5, 0) },    //Coplanar non-intersecting edge collinear
    { vgl_point_3d<double>(0, 8, 0), vgl_point_3d<double>(0, 3, 0), vgl_point_3d<double>(0, 5, 0) },    //Coplanar intersecting edge collinear
    { vgl_point_3d<double>(2, 1, 5), vgl_point_3d<double>(2, 1, 3), vgl_point_3d<double>(2, 1, 1) },    //Non-coplanar non-intersecting
    { vgl_point_3d<double>(2, 1, 3), vgl_point_3d<double>(2, 1, 1), vgl_point_3d<double>(2, 1, -2) }    //Non-coplanar intersecting
  };

  char* degen_desc[NUM_TESTS] = {
    "Coplanar non-intersecting",
    "Coplanar intersecting",
    "Coplanar non-intersecting edge collinear",
    "Coplanar intersecting edge collinear",
    "Non-coplanar non-intersecting",
    "Non-coplanar intersecting"
  };

  vgl_triangle_3d_intersection_t exp_result[][NUM_TESTS] = {
    { None, None, None },
    { Coplanar, Coplanar, Coplanar },
    { None, None, None },
    { Coplanar, Coplanar, Coplanar },
    { None, None, None },
    { None, Skew, Skew }
  };


  unsigned degen_tests[][3] = {
    { 1, 1, 1 }, //all vertices the same point
    { 1, 1, 2 }, // one vertex distinct
    { 0, 1, 2 }  // all vertices distinct
  };

  char* degen_tests_desc[3] = {
    "point",
    "one vertex distinct",
    "all vertices distinct"
  };

  for (unsigned i = 0; i < NUM_TESTS; ++i)
  {
    for (unsigned j = 0; j < 3; ++j)
    {
      vgl_triangle_3d_intersection_t ret =
        vgl_triangle_3d_triangle_intersection(
          valid_p1,
          valid_p2,
          valid_p3,
          degen[i][degen_tests[j][0]],
          degen[i][degen_tests[j][1]],
          degen[i][degen_tests[j][2]]);

      vgl_triangle_3d_intersection_t ret_reversed =
        vgl_triangle_3d_triangle_intersection(
          degen[i][degen_tests[j][0]],
          degen[i][degen_tests[j][1]],
          degen[i][degen_tests[j][2]],
          valid_p1,
          valid_p2,
          valid_p3);

      vcl_string test_desc = degen_desc[i];
      test_desc += " degenerate (";
      test_desc += degen_tests_desc[j];
      test_desc += ")";

      TEST(test_desc.c_str(), ret, exp_result[i][j]);

      test_desc += " reversed";

      TEST(test_desc.c_str(), ret_reversed, exp_result[i][j]);
    }
  }
}

//==================================================================================
inline void test_intersecting_degenerate_triangles2()
{
  //test intersection of 2 degenerate triangles
  vgl_point_3d<double> d1_p1(0, 0, 0);
  vgl_point_3d<double> d1_p2(2, 0, 0);
  vgl_point_3d<double> d1_p3(4, 0, 0);

  vgl_point_3d<double> d2_p1(-1, 0, 0);
  vgl_point_3d<double> d2_p2(-3, 0, 0);
  vgl_point_3d<double> d2_p3(-2, 0, 0);

  vgl_triangle_3d_intersection_t ret =
    vgl_triangle_3d_triangle_intersection(d1_p1,d1_p2,d1_p3,d2_p1,d2_p2,d2_p3);

  TEST("Non-Intersecting two degenerate triangles", ret, None);

  ret =
    vgl_triangle_3d_triangle_intersection(d2_p1,d2_p2,d2_p3,d1_p1,d1_p2,d1_p3);

  TEST("Non-Intersecting two degenerate triangles reversed", ret, None);

  d2_p1.set(0, 0, 0);
  d2_p2.set(1, 1, 1);
  d2_p3.set(2, 2, 2);

  ret =
    vgl_triangle_3d_triangle_intersection(d1_p1,d1_p2,d1_p3,d2_p1,d2_p2,d2_p3);

  TEST("Intersecting two degenerate triangles", ret, Coplanar);

  ret =
    vgl_triangle_3d_triangle_intersection(d2_p1,d2_p2,d2_p3,d1_p1,d1_p2,d1_p3);

  TEST("Intersecting two degenerate triangles reversed", ret, Coplanar);
}

//==================================================================================
inline void test_coincident_edges1()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1( 0, 0, 0);
  vgl_point_3d<double> b_p2(-4, 0, 0);
  vgl_point_3d<double> b_p3( 0,-4, 0);

  vcl_vector<vcl_pair<unsigned,unsigned> > coinc =
    vgl_triangle_3d_coincident_edges(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);

  TEST("Coincident_edges non-coincident", coinc.empty(), true);

}

//==================================================================================
inline void test_coincident_edges2()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1( 1, 0, 0);
  vgl_point_3d<double> b_p2(0, 0, 4);
  vgl_point_3d<double> b_p3( 2,0, 0);

  vcl_vector<vcl_pair<unsigned,unsigned> > coinc =
    vgl_triangle_3d_coincident_edges(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);

  vcl_pair<unsigned,unsigned> exp_edge(2,2);


  TEST("Coincident_edges 1 coincident edge", coinc.size() == 1 && coinc[0] == exp_edge, true);

}

//==================================================================================
//! A simple performance test on the different triangle point containment algorithms
inline void test_point_containment_algo_perf()
{
  vgl_point_3d<double> p1(0,0,0);
  vgl_point_3d<double> p2(5,0,3);
  vgl_point_3d<double> p3(2,0,10);

  vgl_point_3d<double> test_pt = centre(p1,p2,p3);

  unsigned tries = 10000000;

  vcl_clock_t bary_st = vcl_clock();
  for (unsigned i = 0; i < tries; ++i)
    vgl_triangle_3d_test_inside(test_pt,p1,p2,p3);
  vcl_clock_t bary_en = vcl_clock();

  vcl_clock_t ang_st = vcl_clock();
  for (unsigned i = 0; i < tries; ++i)
    vgl_triangle_3d_test_inside_simple(test_pt,p1,p2,p3);
  vcl_clock_t ang_en = vcl_clock();

  unsigned long bary_time = bary_en - bary_st;
  unsigned long ang_time = ang_en - ang_st;

  unsigned long bary_ps = (tries * CLOCKS_PER_SEC) / bary_time;
  unsigned long ang_ps = (tries * CLOCKS_PER_SEC) / ang_time;

  vcl_cout<<"Barycentric method: "<< bary_ps << "/sec ("<<bary_time<<" ticks)\n";
  vcl_cout<<"Angles method: "<< ang_ps << "/sec ("<<ang_time<<" ticks)\n";
  vcl_cout.precision(2);
  vcl_cout<<"Barycentric "<<vcl_fixed<< double(bary_ps)/ang_ps << " times faster than angles\n";

  // usually get Barycentric about 2.5 to 3 times faster than angles KOM (Aug 07)
}



//==================================================================================
// Test function vgl_triangle_3d_closest_point()
//==================================================================================
inline void test_closest_point()
{
  vcl_cout << "\n";
  vcl_cout << "--------------------------------------------------\n";
  vcl_cout << "test_closest_point() \n";
  vcl_cout << "--------------------------------------------------\n";

  // Consider a triangle in the XY plane.
  vgl_point_3d<double> p1(0,0,0);
  vgl_point_3d<double> p2(1,0,0);
  vgl_point_3d<double> p3(0,1,0);

  // Test one of the actual vertices
  {
    vgl_point_3d<double> q(0,0,0);
    vgl_point_3d<double> c = vgl_triangle_3d_closest_point(q, p1, p2, p3);
    TEST_NEAR("Vertex point", (c-p1).length(), 0.0, 1e-6);
  }

  // Test a point along a triangle edge
  {
    vgl_point_3d<double> q(0.5, 0.0, 0.0);
    vgl_point_3d<double> c = vgl_triangle_3d_closest_point(q, p1, p2, p3);
    TEST_NEAR("Edge point", (c-q).length(), 0.0, 1e-6);
  }

  // Test a point within the triangle
  {
    vgl_point_3d<double> q(0.1, 0.1, 0.0);
    vgl_point_3d<double> c = vgl_triangle_3d_closest_point(q, p1, p2, p3);
    TEST_NEAR("Point inside", (c-q).length(), 0.0, 1e-6);
  }

  // Test a point outside the triangle (but in same plane)
  {
    vgl_point_3d<double> q(-1.0, 0.0, 0.0);
    vgl_point_3d<double> c = vgl_triangle_3d_closest_point(q, p1, p2, p3);
    TEST_NEAR("Point outside but same plane", (c-p1).length(), 0.0, 1e-6);
  }

  // Test a point outside the triangle (in a different plane, but would project inside the triangle)
  {
    vgl_point_3d<double> q(0.1, 0.1, 1.0);
    vgl_point_3d<double> c = vgl_triangle_3d_closest_point(q, p1, p2, p3);
    vgl_point_3d<double> r(0.1, 0.1, 0.0); // The expected closest point
    TEST_NEAR("Point inside", (c-r).length(), 0.0, 1e-6);
  }

  // Test a point outside the triangle (in a different plane, and would project outside the triangle)
  {
    vgl_point_3d<double> q(-1.0, 0.0, 1.0);
    vgl_point_3d<double> c = vgl_triangle_3d_closest_point(q, p1, p2, p3);
    TEST_NEAR("Point outside and different plane", (c-p1).length(), 0.0, 1e-6);
  }
}


//==================================================================================
// Test function vgl_triangle_3d_distance()
//==================================================================================
inline void test_distance()
{
  vcl_cout << "\n";
  vcl_cout << "--------------------------------------------------\n";
  vcl_cout << "test vgl_triangle_3d_distance() \n";
  vcl_cout << "--------------------------------------------------\n";

  // Consider a triangle in the XY plane.
  vgl_point_3d<double> p1(0,0,0);
  vgl_point_3d<double> p2(1,0,0);
  vgl_point_3d<double> p3(0,1,0);

  // Test one of the actual vertices
  {
    vgl_point_3d<double> q(0,0,0);
    double d = vgl_triangle_3d_distance(q, p1, p2, p3);
    double true_d = 0.0;
    TEST_NEAR("Vertex point", d, true_d, 1e-6);
  }

  // Test a point along a triangle edge
  {
    vgl_point_3d<double> q(0.5, 0.0, 0.0);
    double d = vgl_triangle_3d_distance(q, p1, p2, p3);
    double true_d = 0.0;
    TEST_NEAR("Edge point", d, true_d, 1e-6);
  }

  // Test a point within the triangle
  {
    vgl_point_3d<double> q(0.1, 0.1, 0.0);
    double d = vgl_triangle_3d_distance(q, p1, p2, p3);
    double true_d = 0.0;
    TEST_NEAR("Point inside", d, true_d, 1e-6);
  }

  // Test a point outside the triangle (but in same plane)
  {
    vgl_point_3d<double> q(-1.0, 0.0, 0.0);
    double d = vgl_triangle_3d_distance(q, p1, p2, p3);
    double true_d = 1.0;
    TEST_NEAR("Point outside but same plane", d, true_d, 1e-6);
  }

  // Test a point outside the triangle (in a different plane, but would project inside the triangle)
  {
    vgl_point_3d<double> q(0.1, 0.1, 1.0);
    double d = vgl_triangle_3d_distance(q, p1, p2, p3);
    double true_d = 1.0;
    TEST_NEAR("Point inside", d, true_d, 1e-6);
  }

  // Test a point outside the triangle (in a different plane, and would project outside the triangle)
  {
    vgl_point_3d<double> q(-1.0, 0.0, 1.0);
    double d = vgl_triangle_3d_distance(q, p1, p2, p3);
    double true_d = 1.414213562;
    TEST_NEAR("Point outside and different plane", d, true_d, 1e-6);
  }
}

//==================================================================================
// Test function vgl_triangle_3d_area()
//==================================================================================
inline void test_area()
{
  vgl_point_3d<double> p1(0,0,0);
  vgl_point_3d<double> p2(1,0,0);
  vgl_point_3d<double> p3(0,1,0);

  double area = vgl_triangle_3d_area( p1, p2, p3 );
  TEST_NEAR("Triangle area", area, 0.5, 1e-6);
}

//==================================================================================
// Test function vgl_triangle_3d_aspect_ratio()
//==================================================================================
inline void test_aspect_ratio()
{
  vgl_point_3d<double> p1(0,0,0);
  vgl_point_3d<double> p2(1,0,0);
  vgl_point_3d<double> p3(0,1,0);

  double ratio = vgl_triangle_3d_aspect_ratio( p1, p2, p3 );
  TEST_NEAR("Triangle area", ratio, vcl_sqrt(2.0),1e-6);
}

//==================================================================================
// Main test function
//==================================================================================
MAIN( test_triangle_3d )
{
  START( "test_triangle_3d" );

  test_non_intersecting();

  test_intersecting1();
  test_intersecting2();
  test_intersecting3();
  test_intersecting4();

  test_intersecting_degenerate_triangles1();
  test_intersecting_degenerate_triangles2();

  test_coincident_edges1();
  test_coincident_edges2();

  test_closest_point();
  test_distance();
  test_area();
  test_aspect_ratio();

//  test_point_containment_algo_perf(); // Performance Test disabled by default - KOM

  SUMMARY();
}
