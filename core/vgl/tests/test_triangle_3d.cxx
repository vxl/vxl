#include <iostream>
#include <ctime>
#include <testlib/testlib_test.h>
//:
// \file
// \verbatim
//  Modifications
//   2009-03-08 Peter Vanroose - Increased test coverage: added "inside" & "plane intersection" tests
// \endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vgl/vgl_triangle_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_line_segment_3d.h>

//==============================================================================
inline void test_non_intersecting()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1(0, 0, 2);
  vgl_point_3d<double> b_p2(0, 4, 2);
  vgl_point_3d<double> b_p3(4, 0, 2);

  vgl_line_segment_3d<double> iline;
  unsigned edge_p1, edge_p2;
  vgl_triangle_3d_intersection_t ret
 = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3, iline, edge_p1, edge_p2);
  TEST("Non-intersecting 1", ret, None);
  ret = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);
  TEST("Non-intersecting 1 faster version", ret, None);

  b_p1.set(-1,0,0);
  b_p2.set(-1,4,0);
  b_p3.set(-4,0,0);

  ret = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3, iline, edge_p1, edge_p2);
  TEST("Non-intersecting 2", ret, None);
  ret = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);
  TEST("Non-intersecting 2 faster version", ret, None);
}


//==============================================================================
inline void test_intersecting1()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1(0, 0, 0);
  vgl_point_3d<double> b_p2(0, 4, 0);
  vgl_point_3d<double> b_p3(4, 4, 0);

  vgl_triangle_3d_intersection_t ret
 = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);
  TEST("Intersecting 1 coplanar", ret, Coplanar);
  vgl_line_segment_3d<double> iline;
  unsigned edge_p1, edge_p2;
  ret = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3, iline, edge_p1, edge_p2);
  TEST("Intersecting coplanar with iline", ret, Coplanar);
}


//==============================================================================
inline void test_intersecting2()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1(1, 1, 4);
  vgl_point_3d<double> b_p2(1, 1,-4);
  vgl_point_3d<double> b_p3(0, 0, 0);

  vgl_triangle_3d_intersection_t ret
 = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);
  TEST("Intersecting 2 Skew", ret, Skew);

  vgl_line_segment_3d<double> i_line;
  unsigned edge_p1, edge_p2;
  ret = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3, i_line, edge_p1, edge_p2);
  TEST("Intersecting Skew with iline", ret, Skew);
  double p1_err = vgl_distance(i_line.point1(), vgl_point_3d<double>(0,0,0));
  double p2_err = vgl_distance(i_line.point2(), vgl_point_3d<double>(1,1,0));
  //handle ambiguity of intersecting line segment ordering.
  if (p1_err>p2_err) std::swap(edge_p1, edge_p2);
  TEST_NEAR("Intersecting Skew, iline correct", p1_err + p2_err, 0, 1e-8);
  // handle ambiguity of intersecting line point 1 , which lies on a vertex of each input triangle.
  TEST("Intersecting Skew, edge label 1 correct", edge_p1==0 || edge_p1==2 || edge_p1==4 || edge_p1==5, true);
  TEST("Intersecting Skew, edge label 2 correct", edge_p2==3, true);
}


//==============================================================================
inline void test_intersecting3()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1(0, 0, 0);
  vgl_point_3d<double> b_p2(0, 4, 0);
  vgl_point_3d<double> b_p3(0, 0, 4);

  vgl_triangle_3d_intersection_t ret
 = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);
  TEST("Intersecting 3 Skew", ret, Skew);

  vgl_line_segment_3d<double> i_line;
  unsigned edge_p1, edge_p2;
  ret = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3,i_line, edge_p1, edge_p2);

  double p1_err = vgl_distance(i_line.point1(), vgl_point_3d<double>(0,0,0));
  double p2_err = vgl_distance(i_line.point2(), vgl_point_3d<double>(0,4,0));
  //handle ambiguity of intersecting line segment ordering.
  if (p1_err > 1e-4)
  {
    std::swap(edge_p1, edge_p2);
    p1_err = vgl_distance(i_line.point2(), vgl_point_3d<double>(0,0,0));
    p2_err = vgl_distance(i_line.point1(), vgl_point_3d<double>(0,4,0));
  }

  TEST("Intersecting Skew - with isect line", ret, Skew);
  TEST_NEAR("Intersecting Skew, iline correct", p1_err + p2_err, 0, 1e-8);
  TEST("Intersecting Skew, edge label 1 correct", edge_p1==2 || edge_p1==5, true);
  TEST("Intersecting Skew, edge label 2 correct", edge_p2==1 || edge_p2==4, true);
}


//==============================================================================
inline void test_intersecting4()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1( 0, 0, 0);
  vgl_point_3d<double> b_p2( 0, 4, 0);
  vgl_point_3d<double> b_p3(-4, 0, 0);

  vgl_triangle_3d_intersection_t ret
 = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);
  TEST("Intersecting 4 coplanar", ret, Coplanar);

  vgl_line_segment_3d<double> i_line;
  unsigned edge_p1, edge_p2;
  ret = vgl_triangle_3d_triangle_intersection(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3,i_line,edge_p1, edge_p2);


  double p1_err = vgl_distance(i_line.point1(), vgl_point_3d<double>(0,0,0));
  double p2_err = vgl_distance(i_line.point2(), vgl_point_3d<double>(0,4,0));
  //handle ambiguity of intersecting line segment ordering.
  if (p1_err > 1e-4)
  {
    std::swap(edge_p1, edge_p2);
    p1_err = vgl_distance(i_line.point2(), vgl_point_3d<double>(0,0,0));
    p2_err = vgl_distance(i_line.point1(), vgl_point_3d<double>(0,4,0));
  }

  TEST("Intersecting Coplanar - with isect line", ret, Coplanar);
  TEST_NEAR("Intersecting Coplanar, iline correct", p1_err + p2_err, 0, 1e-8);
  // Due to coplanarity, edge labels are very ambiguous
  TEST("Intersecting Coplanar, edge label 1 correct", edge_p1==2 || edge_p1==5 || edge_p1==0 || edge_p1==3, true);
  TEST("Intersecting Coplanar, edge label 2 correct", edge_p2==1 || edge_p2==4 || edge_p2==0 || edge_p2==3, true);
}


inline void test_intersecting5_arrangement(
  const vgl_point_3d<double>& a_A, const vgl_point_3d<double>& a_B, const vgl_point_3d<double>& a_C,
  const vgl_point_3d<double>& b_A, const vgl_point_3d<double>& b_B, const vgl_point_3d<double>& b_C,
  unsigned expected_edge_1, unsigned expected_edge_2)
{
  vgl_triangle_3d_intersection_t ret
 = vgl_triangle_3d_triangle_intersection(a_A,a_B,a_C,b_A,b_B,b_C);
  TEST("Intersecting Skew", ret, Skew);

  vgl_line_segment_3d<double> i_line;
  unsigned edge_p1, edge_p2;
  ret = vgl_triangle_3d_triangle_intersection(a_A,a_B,a_C,b_A,b_B,b_C,i_line, edge_p1, edge_p2);

  double p1_err = vgl_distance(i_line.point1(), vgl_point_3d<double>(1,1,0));
  double p2_err = vgl_distance(i_line.point2(), vgl_point_3d<double>(2,2,0));
  //handle ambiguity of intersecting line segment ordering.
  if (p1_err > 1e-2)
  {
    std::swap(edge_p1, edge_p2);
    p1_err = vgl_distance(i_line.point2(), vgl_point_3d<double>(1,1,0));
    p2_err = vgl_distance(i_line.point1(), vgl_point_3d<double>(2,2,0));
  }

  TEST("Intersecting Skew - with isect line", ret, Skew);
  TEST_NEAR("Intersecting Skew, iline correct", p1_err + p2_err, 0, 1e-8);
  TEST("Intersecting Skew, edge label 1 correct", edge_p1==expected_edge_1, true);
  TEST("Intersecting Skew, edge label 2 correct", edge_p2==expected_edge_2, true);
}


//==============================================================================
inline void test_intersecting5()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1(4, 4, 0);
  vgl_point_3d<double> b_p2(1, 1, -4);
  vgl_point_3d<double> b_p3(1, 1, 4);

  std::cout << "Test skew intersection with multiple arrangements.\n";

  test_intersecting5_arrangement(a_p1, a_p2, a_p3, b_p1, b_p2, b_p3, 4, 1);
  test_intersecting5_arrangement(a_p2, a_p1, a_p3, b_p1, b_p2, b_p3, 4, 2);
  test_intersecting5_arrangement(a_p2, a_p3, a_p1, b_p1, b_p2, b_p3, 4, 0);
  test_intersecting5_arrangement(a_p3, a_p1, a_p2, b_p1, b_p2, b_p3, 4, 2);
  test_intersecting5_arrangement(a_p1, a_p2, a_p3, b_p2, b_p3, b_p1, 3, 1);
  test_intersecting5_arrangement(a_p2, a_p1, a_p3, b_p2, b_p3, b_p1, 3, 2);
  test_intersecting5_arrangement(a_p2, a_p3, a_p1, b_p2, b_p3, b_p1, 3, 0);
  test_intersecting5_arrangement(a_p3, a_p1, a_p2, b_p2, b_p3, b_p1, 3, 2);
  test_intersecting5_arrangement(a_p1, a_p2, a_p3, b_p3, b_p1, b_p2, 5, 1);
  test_intersecting5_arrangement(a_p2, a_p1, a_p3, b_p3, b_p1, b_p2, 5, 2);
  test_intersecting5_arrangement(a_p2, a_p3, a_p1, b_p3, b_p1, b_p2, 5, 0);
  test_intersecting5_arrangement(a_p3, a_p1, a_p2, b_p3, b_p1, b_p2, 5, 2);
  test_intersecting5_arrangement(a_p1, a_p2, a_p3, b_p2, b_p1, b_p3, 5, 1);
  test_intersecting5_arrangement(a_p2, a_p1, a_p3, b_p2, b_p1, b_p3, 5, 2);
  test_intersecting5_arrangement(a_p2, a_p3, a_p1, b_p2, b_p1, b_p3, 5, 0);
  test_intersecting5_arrangement(a_p3, a_p1, a_p2, b_p2, b_p1, b_p3, 5, 2);

  test_intersecting5_arrangement(b_p1, b_p2, b_p3, a_p1, a_p2, a_p3, 1, 4);
  test_intersecting5_arrangement(b_p1, b_p2, b_p3, a_p2, a_p1, a_p3, 1, 5);
  test_intersecting5_arrangement(b_p1, b_p2, b_p3, a_p2, a_p3, a_p1, 1, 3);
  test_intersecting5_arrangement(b_p1, b_p2, b_p3, a_p3, a_p1, a_p2, 1, 5);
  test_intersecting5_arrangement(b_p2, b_p3, b_p1, a_p1, a_p2, a_p3, 0, 4);
  test_intersecting5_arrangement(b_p2, b_p3, b_p1, a_p2, a_p1, a_p3, 0, 5);
  test_intersecting5_arrangement(b_p2, b_p3, b_p1, a_p2, a_p3, a_p1, 0, 3);
  test_intersecting5_arrangement(b_p2, b_p3, b_p1, a_p3, a_p1, a_p2, 0, 5);
  test_intersecting5_arrangement(b_p3, b_p1, b_p2, a_p1, a_p2, a_p3, 2, 4);
  test_intersecting5_arrangement(b_p3, b_p1, b_p2, a_p2, a_p1, a_p3, 2, 5);
  test_intersecting5_arrangement(b_p3, b_p1, b_p2, a_p2, a_p3, a_p1, 2, 3);
  test_intersecting5_arrangement(b_p3, b_p1, b_p2, a_p3, a_p1, a_p2, 2, 5);
  test_intersecting5_arrangement(b_p2, b_p1, b_p3, a_p1, a_p2, a_p3, 2, 4);
  test_intersecting5_arrangement(b_p2, b_p1, b_p3, a_p2, a_p1, a_p3, 2, 5);
  test_intersecting5_arrangement(b_p2, b_p1, b_p3, a_p2, a_p3, a_p1, 2, 3);
  test_intersecting5_arrangement(b_p2, b_p1, b_p3, a_p3, a_p1, a_p2, 2, 5);
}


//==============================================================================
inline void test_intersecting_degenerate_triangles1()
{
  std::cout << '\n'
           << "----------------------------------------------\n"
           << " test vgl_triangle_3d_triangle_intersection()\n"
           << "----------------------------------------------\n";

  //the valid triangle
  vgl_point_3d<double> valid_p1(0, 0, 0);
  vgl_point_3d<double> valid_p2(0, 4, 0);
  vgl_point_3d<double> valid_p3(4, 0, 0);

  //Test on a number of different types of degenerate triangle

  constexpr unsigned NUM_TESTS = 7;
  constexpr unsigned NUM_VARIANTS = 3;

  vgl_point_3d<double> degen[NUM_TESTS][3] = {
    { vgl_point_3d<double>(-4, 5, 0), vgl_point_3d<double>(-3, 4, 0), vgl_point_3d<double>(-2, 0, 0) }, //Coplanar non-intersecting
    { vgl_point_3d<double>(4, 5, 0), vgl_point_3d<double>(-3, 4, 0), vgl_point_3d<double>(2, -1, 0) }, //Coplanar non-intersecting
    { vgl_point_3d<double>(2, 5, 0), vgl_point_3d<double>(2, 1, 0), vgl_point_3d<double>(2, -3, 0) },   //Coplanar intersecting
    { vgl_point_3d<double>(0, 8, 0), vgl_point_3d<double>(0, 6, 0), vgl_point_3d<double>(0, 5, 0) },    //Coplanar non-intersecting edge collinear
    { vgl_point_3d<double>(0, 8, 0), vgl_point_3d<double>(0, 3, 0), vgl_point_3d<double>(0, 5, 0) },    //Coplanar intersecting edge collinear
    { vgl_point_3d<double>(2, 1, 5), vgl_point_3d<double>(2, 1, 3), vgl_point_3d<double>(2, 1, 1) },    //Non-coplanar non-intersecting
    { vgl_point_3d<double>(2, 1, 3), vgl_point_3d<double>(2, 1, 1), vgl_point_3d<double>(2, 1, -2) }    //Non-coplanar intersecting
  };

  const char* degen_desc[NUM_TESTS] = {
    "Coplanar non-intersecting possibly non-degenerate",
    "Coplanar intersecting possibly non-degenerate",
    "Coplanar intersecting degenerate",
    "Coplanar non-intersecting edge collinear degenerate",
    "Coplanar intersecting edge collinear degenerate",
    "Non-coplanar non-intersecting degenerate",
    "Non-coplanar intersecting degenerate"
  };

  vgl_triangle_3d_intersection_t exp_result[NUM_TESTS][NUM_VARIANTS] = {
    { None, None, None },
    { None, Coplanar, Coplanar },
    { Coplanar, Coplanar, Coplanar },
    { None, None, None },
    { Coplanar, Coplanar, Coplanar },
    { None, None, None },
    { None, Skew, Skew }
  };

  // Expected edges [0-5]=>correct values.  7=>Any repreated values would be correct
  // 8=>Any repeated values from the degenerate triange would be correct
  // 9=>No expected answer( since no intersection)
  unsigned exp_edges[NUM_TESTS][NUM_VARIANTS][2] = {
    { {9, 9}, {9, 9}, {9, 9} }, // unspecified if non-intersecting
    { {9, 9}, {8, 8}, {7, 7} }, // repeated edges if coplanar
    { {3, 3}, {8, 8}, {8, 8} }, // repeated edges if coplanar
    { {9, 9}, {9, 9}, {9, 9} }, // unspecified if non-intersecting
    { {3, 3}, {8, 8}, {8, 8} },
    { {9, 9}, {9, 9}, {9, 9} }, // unspecified if non-intersecting
    { {9, 9}, {4, 5}, {4, 5} }
  };


  unsigned degen_tests[NUM_VARIANTS][3] = {
    { 1, 1, 1 }, //all vertices the same point
    { 1, 1, 2 }, // one vertex distinct
    { 0, 1, 2 }  // all vertices distinct
  };

  const char* degen_tests_desc[3] = {
    "point",
    "one vertex distinct",
    "all vertices distinct"
  };

  for (unsigned i = 0; i < NUM_TESTS; ++i)
  {
    for (unsigned j = 0; j < NUM_VARIANTS; ++j)
    {
      if (exp_result[i][j]!=None) // Check for any unwarranted looseness in test.
        assert(exp_edges[i][j][0]!=9);

      std::string test_desc = degen_desc[i];
      test_desc += " (";
      test_desc += degen_tests_desc[j];
      test_desc += ")";

      vgl_line_segment_3d<double> i_line;
      auto point1_edge = (unsigned) -1, point2_edge = (unsigned) -2;
      vgl_triangle_3d_intersection_t ret =
        vgl_triangle_3d_triangle_intersection(
          valid_p1,
          valid_p2,
          valid_p3,
          degen[i][degen_tests[j][0]],
          degen[i][degen_tests[j][1]],
          degen[i][degen_tests[j][2]],
          i_line, point1_edge, point2_edge);

      TEST(test_desc.c_str(), ret, exp_result[i][j]);
      if (exp_edges[i][j][0]!=9)
      {
        if (point1_edge > point2_edge) // Canonicalise edge results.
          std::swap(point1_edge, point2_edge);
        if (exp_edges[i][j][0]==8)
          TEST("Edge 1 == Edge 2 for no definite edge result",
               point1_edge == point2_edge && point1_edge >= 3, true);
        else if (exp_edges[i][j][0]==7)
          TEST("Edge 1 == Edge 2 for no definite edge result",
               point1_edge == point2_edge, true);
        else
        {
          TEST_NEAR("Edge 1 correct", point1_edge, exp_edges[i][j][0], 0);
          TEST_NEAR("Edge 2 correct", point2_edge, exp_edges[i][j][1], 0);
        }
      }
      test_desc += " reversed";
      ret = vgl_triangle_3d_triangle_intersection(
          degen[i][degen_tests[j][0]],
          degen[i][degen_tests[j][1]],
          degen[i][degen_tests[j][2]],
          valid_p1,
          valid_p2,
          valid_p3,
          i_line, point1_edge, point2_edge);


      TEST(test_desc.c_str(), ret, exp_result[i][j]);
      if (exp_edges[i][j][0]!=9)
      {
        if (point1_edge > point2_edge) // Canonicalise edge results.
          std::swap(point1_edge, point2_edge);
        if (exp_edges[i][j][0]==8)
          TEST("Edge 1 == Edge 2 for no definite edge result",
               point1_edge == point2_edge && point1_edge < 3, true);
        else if (exp_edges[i][j][0]==7)
          TEST("Edge 1 == Edge 2 for no definite edge result",
               point1_edge == point2_edge, true);
        else
        {
        // modify exp_edge values to point to other triangle.
          TEST_NEAR("Edge 1 correct", point1_edge, (exp_edges[i][j][0]+3)%6, 0);
          TEST_NEAR("Edge 2 correct", point2_edge, (exp_edges[i][j][1]+3)%6, 0);
        }
      }
    }
  }
}


//==============================================================================
inline void test_intersecting_degenerate_triangles2()
{
  std::cout << '\n'
           << "----------------------------------------------\n"
           << " test vgl_triangle_3d_triangle_intersection()\n"
           << "----------------------------------------------\n";

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


//==============================================================================
inline void test_coincident_edges1()
{
  std::cout << '\n'
           << "-----------------------------------------\n"
           << " test vgl_triangle_3d_coincident_edges()\n"
           << "-----------------------------------------\n";

  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1( 0, 0, 0);
  vgl_point_3d<double> b_p2(-4, 0, 0);
  vgl_point_3d<double> b_p3( 0,-4, 0);

  std::vector<std::pair<unsigned,unsigned> > coinc =
    vgl_triangle_3d_coincident_edges(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);

  TEST("Coincident_edges non-coincident", coinc.empty(), true);
}


//==============================================================================
inline void test_coincident_edges2()
{
  vgl_point_3d<double> a_p1(0, 0, 0);
  vgl_point_3d<double> a_p2(0, 4, 0);
  vgl_point_3d<double> a_p3(4, 0, 0);

  vgl_point_3d<double> b_p1( 1, 0, 0);
  vgl_point_3d<double> b_p2(0, 0, 4);
  vgl_point_3d<double> b_p3( 2,0, 0);

  std::vector<std::pair<unsigned,unsigned> > coinc =
    vgl_triangle_3d_coincident_edges(a_p1,a_p2,a_p3,b_p1,b_p2,b_p3);

  std::pair<unsigned,unsigned> exp_edge(2,2);

  TEST("Coincident_edges 1 coincident edge",
       coinc.size() == 1 && coinc[0] == exp_edge, true);
}

//==============================================================================
inline void test_intersect_plane()
{
  std::cout << '\n'
           << "-------------------------------------------\n"
           << " test vgl_triangle_3d_plane_intersection()\n"
           << "-------------------------------------------\n";

  vgl_point_3d<double> p1(0,  0,  0);
  vgl_point_3d<double> p2(0, 15, 25);
  vgl_point_3d<double> p3(0, 45, 10);

  vgl_plane_3d<double> pl(0,0,1,-20); // the plane z=20 ==> normal intersection
  vgl_line_segment_3d<double> l; // the returned intersection
  vgl_point_3d<double> p(0,12,20), q(0,25,20);
  vgl_line_segment_3d<double> l0(p,q); // the correct intersection
  vgl_triangle_3d_intersection_t ret;

  ret = vgl_triangle_3d_plane_intersection(p1,p2,p3,pl,l);
  TEST("normal intersection", ret == Skew && l == l0, true);

  pl.set(0,0,1,1); // z=-1 ==> no intersection
  ret = vgl_triangle_3d_plane_intersection(p1,p2,p3,pl,l);
  TEST("no intersection", ret, None);

  pl.set(0,0,1,-25); // z=25 ==> corner point intersection
  l0.set(p2,p2);
  ret = vgl_triangle_3d_plane_intersection(p1,p2,p3,pl,l);
  TEST("corner point intersection", ret == Skew && l == l0, true);

  pl.set(1,0,0,0); // x=0 ==> coplanar with triangle
  ret = vgl_triangle_3d_plane_intersection(p1,p2,p3,pl,l);
  TEST("coplanar intersection", ret, Coplanar);
}

//==============================================================================
inline void test_point_containment()
{
  std::cout << '\n'
           << "------------------------------------\n"
           << " test vgl_triangle_3d_test_inside()\n"
           << "------------------------------------\n";


  // three arbitrary coplanar points (note that coplanarity is assumed by the two methods!)
  {
    vgl_point_3d<double> p1(0,0,0);
    vgl_point_3d<double> p2(5,0,3);
    vgl_point_3d<double> p3(2,0,10);
    vgl_point_3d<double> in1 = centre(p1,p2,p3); // barycentre
    vgl_point_3d<double> in2(2.5,0,1.51); // point almost on the edge p1--p2
    vgl_point_3d<double> in3(0.01,0,0.01); // almost a corner point
    vgl_point_3d<double> out1(2,0,1); // same plane, outside triangle
    vgl_point_3d<double> out2(2,1,1); // not coplanar, still outside triangle
    vgl_point_3d<double> in4(0.01,0.1,0.01); // not quite coplanar, but otherwise inside triangle
    TEST("inside 1 - barycentric method", vgl_triangle_3d_test_inside(in1,p1,p2,p3), true);
    TEST("inside 1 - barycentric method with coplanar tol", vgl_triangle_3d_test_inside(in1,p1,p2,p3, 1e-10), true);
    TEST("inside 1 - cosine method", vgl_triangle_3d_test_inside_simple(in1,p1,p2,p3), true);
    TEST("inside 2 - barycentric method", vgl_triangle_3d_test_inside(in2,p1,p2,p3), true);
    TEST("inside 2 - barycentric method with coplanar tol", vgl_triangle_3d_test_inside(in2,p1,p2,p3, 1e-10), true);
    TEST("inside 2 - cosine method", vgl_triangle_3d_test_inside_simple(in2,p1,p2,p3), true);
    TEST("inside 3 - barycentric method", vgl_triangle_3d_test_inside(in3,p1,p2,p3), true);
    TEST("inside 3 - cosine method", vgl_triangle_3d_test_inside_simple(in3,p1,p2,p3), true);
    TEST("not inside 1 - barycentric method", vgl_triangle_3d_test_inside(out1,p1,p2,p3), false);
    TEST("not inside 1 - cosine method", vgl_triangle_3d_test_inside_simple(out1,p1,p2,p3), false);
    TEST("not inside 2 - barycentric method", vgl_triangle_3d_test_inside(out2,p1,p2,p3), false);
    TEST("not inside 2 - cosine method", vgl_triangle_3d_test_inside_simple(out2,p1,p2,p3), false);
    TEST("sort of inside 4 - barycentric method with coplanar tol", vgl_triangle_3d_test_inside(in4,p1,p2,p3, 0.2), true);
    TEST("sort of inside 4 - barycentric method", vgl_triangle_3d_test_inside(in4,p1,p2,p3), false);
  }
  // three collinear points -- the cosine method will always fail in this case
  {
    vgl_point_3d<double> p1(0,0,0);
    vgl_point_3d<double> p2(5,0,3);
    vgl_point_3d<double> p3(15,0,9);
    vgl_point_3d<double> in1(2.5,0,1.5); // barycentre
    vgl_point_3d<double> in2(2,0,1.2);
    vgl_point_3d<double> in3(0.05,0,0.03); // almost a corner point
    vgl_point_3d<double> out1(15.05,0,9.03); // collinear, almost a corner point
    vgl_point_3d<double> out2(20,0,12); // collinear
    vgl_point_3d<double> out3(2,0,1); // not collinear
    TEST("inside 1 (collinear points) - barycentric method", vgl_triangle_3d_test_inside(in1,p1,p2,p3), true);
    TEST("inside 2 (collinear points) - barycentric method", vgl_triangle_3d_test_inside(in2,p1,p2,p3), true);
    TEST("inside 3 (collinear points) - barycentric method", vgl_triangle_3d_test_inside(in3,p1,p2,p3), true);
    TEST("not inside 1 (collinear points) - barycentric method", vgl_triangle_3d_test_inside(out1,p1,p2,p3), false);
    TEST("not inside 2 (collinear points) - barycentric method", vgl_triangle_3d_test_inside(out2,p1,p2,p3), false);
    TEST("not inside 3 (collinear points) - barycentric method", vgl_triangle_3d_test_inside(out3,p1,p2,p3), false);
  }
  // three coincident points
  {
    vgl_point_3d<double> p1(5,0,3);
    vgl_point_3d<double> p2(5,0,3);
    vgl_point_3d<double> p3(5,0,3);
    vgl_point_3d<double> in1(5,0,3);
    vgl_point_3d<double> out1(5,0,2);
    TEST("inside (coincident points) - barycentric method", vgl_triangle_3d_test_inside(in1,p1,p2,p3), true);
    TEST("not inside (coincident points) - barycentric method", vgl_triangle_3d_test_inside(out1,p1,p2,p3), false);
  }
  // almost degenerate triangle
  {
    vgl_point_3d<double> p1(-250.00000027628425, 307.98258024874724, 359.57738152050911);
    vgl_point_3d<double> p2(-239.99999999497970, 314.64924710295026, 359.57738152050911);
    vgl_point_3d<double> p3(-240.00000000000000, 314.64924709960337, 359.57738152050911);

    vgl_point_3d<double> in1(-245.00000013828425, 311.31591367584872, 359.57738152050911);
    vgl_point_3d<double> out1(-245.00000013828425, 311.31591367584872, 359.575);
//    TEST("inside (High AR triangle) - barycentric method", vgl_triangle_3d_test_inside(in1,p1,p2,p3), true);
    TEST("not inside (High AR triangle) - barycentric method", vgl_triangle_3d_test_inside(out1,p1,p2,p3), false);
  }
}

//==============================================================================
//: A simple performance test on the different triangle point containment algorithms
inline void test_point_containment_algo_perf()
{
  vgl_point_3d<double> p1(0,0,0);
  vgl_point_3d<double> p2(5,0,3);
  vgl_point_3d<double> p3(2,0,10);

  vgl_point_3d<double> test_pt = centre(p1,p2,p3);

  unsigned tries = 10000000;

  std::clock_t bary_st = std::clock();
  for (unsigned i = 0; i < tries; ++i)
    vgl_triangle_3d_test_inside(test_pt,p1,p2,p3);
  std::clock_t bary_en = std::clock();

  std::clock_t ang_st = std::clock();
  for (unsigned i = 0; i < tries; ++i)
    vgl_triangle_3d_test_inside_simple(test_pt,p1,p2,p3);
  std::clock_t ang_en = std::clock();

  unsigned long bary_time = bary_en - bary_st;
  unsigned long ang_time = ang_en - ang_st;

  unsigned long bary_ps = (tries * CLOCKS_PER_SEC) / bary_time;
  unsigned long ang_ps = (tries * CLOCKS_PER_SEC) / ang_time;

  std::cout<<"Barycentric method: "<< bary_ps << "/sec ("<<bary_time<<" ticks)\n"
          <<"Angles method: "<< ang_ps << "/sec ("<<ang_time<<" ticks)\n";
  std::cout.precision(2);
  std::cout<<"Barycentric "<<std::fixed<< double(bary_ps)/ang_ps << " times faster than angles\n";

  // usually get Barycentric about 2.5 to 3 times faster than angles KOM (Aug 07)
}


//==============================================================================
// Test function vgl_triangle_3d_closest_point()
//==============================================================================
inline void test_closest_point()
{
  std::cout << '\n'
           << "----------------------\n"
           << " test_closest_point()\n"
           << "----------------------\n";

  {
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
  TEST("To be sure that we get here (Mac)", 1, 1);

  // Nearly degenerate triangle
  {
    vgl_point_3d<double> p1(-250.00000027628425, 307.98258024874724, 359.57738152050911);
    vgl_point_3d<double> p2(-239.99999999497970, 314.64924710295026, 359.57738152050911);
    vgl_point_3d<double> p3(-240.00000000000000, 314.64924709960337, 359.57738152050911);
    std::cout << "Nearly degenerate triangle " << p1 << p2 << p3 << std::endl;

    vgl_point_3d<double> in1(-245.00000013828425, 311.31591367584872, 359.57738152050911);
    vgl_point_3d<double> out1(-245.00000013828425, 311.31591367584872, 380);
    vgl_point_3d<double> c = vgl_triangle_3d_closest_point(in1, p1, p2, p3);
    TEST_NEAR("Point inside High AR triangle", vgl_distance(c, in1), 0, 1e-8);
    c = vgl_triangle_3d_closest_point(out1, p1, p2, p3);
    TEST_NEAR("Point outside High AR triangle", vgl_distance(c, in1), 0, 1e-8);
  }
}


//==============================================================================
// Test function vgl_triangle_3d_distance()
//==============================================================================
inline void test_distance()
{
  std::cout << '\n'
           << "---------------------------------\n"
           << " test vgl_triangle_3d_distance()\n"
           << "---------------------------------\n";

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
    double true_d = 1.4142135623730950488; // sqrt2
    TEST_NEAR("Point outside and different plane", d, true_d, 1e-6);
  }
}


//==============================================================================
// Test function vgl_triangle_3d_area()
//==============================================================================
inline void test_area()
{
  std::cout << '\n'
           << "-----------------------------\n"
           << " test vgl_triangle_3d_area()\n"
           << "-----------------------------\n";

  vgl_point_3d<double> p1(0,0,0);
  vgl_point_3d<double> p2(1,0,0);
  vgl_point_3d<double> p3(0,1,0);

  double area = vgl_triangle_3d_area( p1, p2, p3 );
  TEST_NEAR("Triangle area", area, 0.5, 1e-6);
}


//==============================================================================
// Test function vgl_triangle_3d_aspect_ratio()
//==============================================================================
inline void test_aspect_ratio()
{
  std::cout << '\n'
           << "-------------------------------------\n"
           << " test vgl_triangle_3d_aspect_ratio()\n"
           << "-------------------------------------\n";

  vgl_point_3d<double> p1(0,0,0);
  vgl_point_3d<double> p2(1,0,0);
  vgl_point_3d<double> p3(0,1,0);

  double ratio = vgl_triangle_3d_aspect_ratio( p1, p2, p3 );
  TEST_NEAR("Triangle area", ratio, std::sqrt(2.0),1e-6);
}


//==============================================================================
// Main test function
//==============================================================================
static void test_triangle_3d()
{
  test_closest_point();

  test_non_intersecting();

  test_intersecting1();
  test_intersecting2();
  test_intersecting3();
  test_intersecting4();
  test_intersecting5();

  test_intersect_plane();

  test_intersecting_degenerate_triangles1();
  test_intersecting_degenerate_triangles2();

  test_coincident_edges1();
  test_coincident_edges2();

  test_distance();
  test_area();
  test_aspect_ratio();

  test_point_containment();
#ifdef PERFORMANCE_TEST // Performance Test disabled by default - KOM
  test_point_containment_algo_perf();
#endif
}

TESTMAIN(test_triangle_3d);
