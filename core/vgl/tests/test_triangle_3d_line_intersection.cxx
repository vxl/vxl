// Some tests for vgl_triangle_3d_line_intersection

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vgl/vgl_triangle_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_line_segment_3d.h>

static const double tol=1e-9;


//========================================================================
// Test vgl_triangle_3d_test_inside() function
//========================================================================
static void test_point_inside()
{
  std::cout << '\n'
           << "*************************************\n"
           << " Testing vgl_triangle_3d_test_inside\n"
           << "*************************************\n";

  // Define a triangle - arbitrary choice
  vgl_point_3d<double>  p1(2,1,-1), p2(1,-2,0), p3(-1,2,2);
  vgl_vector_3d<double> v1(2,1,-1), v2(1,-2,0), v3(-1,2,2);
  vgl_point_3d<double> o(0,0,0);

  vgl_point_3d<double> q;
  double a1, a2, a3;

  // test centroid
  {
    a1=1.0/3.0; a2=1.0/3.0; a3=1.0/3.0;
    q = o + a1*v1 + a2*v2 + a3*v3;
    TEST("Centroid inside?", vgl_triangle_3d_test_inside(q, p1, p2, p3), true);
  }

  // test off-centre point
  {
    a1=0.25; a2=0.25; a3=0.5;
    q = o + a1*v1 + a2*v2 + a3*v3;
    vgl_triangle_3d_test_inside(q, p1, p2, p3);
    TEST("Off-centre point inside?", vgl_triangle_3d_test_inside(q, p1, p2, p3), true);
  }

  // test edge point
  {
    a1=0.0; a2=0.5; a3=0.5;
    q = o + a1*v1 + a2*v2 + a3*v3;
    vgl_triangle_3d_test_inside(q, p1, p2, p3);
    TEST("Edge point inside?", vgl_triangle_3d_test_inside(q, p1, p2, p3), true);
  }

  // test vertex point
  {
    a1=0.0; a2=0.0; a3=1.0;
    q = o + a1*v1 + a2*v2 + a3*v3;
    vgl_triangle_3d_test_inside(q, p1, p2, p3);
    TEST("Vertex point inside?", vgl_triangle_3d_test_inside(q, p1, p2, p3), true);
  }

  // test outside point, near edge
  {
    a1=0.0; a2=0.501; a3=0.5;
    q = o + a1*v1 + a2*v2 + a3*v3;
    vgl_triangle_3d_test_inside(q, p1, p2, p3);
    TEST("Outside edge point outside?", vgl_triangle_3d_test_inside(q, p1, p2, p3), false);
  }

  // test outside point, near vertex
  {
    a1=0.0; a2=0.0; a3=1.01;
    q = o + a1*v1 + a2*v2 + a3*v3;
    vgl_triangle_3d_test_inside(q, p1, p2, p3);
    TEST("Outside vertex point outside?", vgl_triangle_3d_test_inside(q, p1, p2, p3), false);
  }

  // test off-plane point (but would project into triangle)
  {
    a1=1.0/3.0; a2=1.0/3.0; a3=1.0/3.0;
    q = o + a1*v1 + a2*v2 + a3*v3 + vgl_vector_3d<double>(0, 0, 2);
    TEST("Off-plane point outside?", vgl_triangle_3d_test_inside(q, p1, p2, p3), false);
  }
}

//========================================================================
// Test vgl_triangle_3d_line_intersection() function
//========================================================================
static void test_line_intersection()
{
  std::cout << '\n'
           << "*******************************************\n"
           << " Testing vgl_triangle_3d_line_intersection\n"
           << "*******************************************\n";
  {
    // Define a triangle - arbitrary choice
    vgl_point_3d<double>  p1(2,1,-1), p2(1,-2,0), p3(-1,2,2);
    vgl_vector_3d<double> v1(2,1,-1), v2(1,-2,0), v3(-1,2,2);
    vgl_point_3d<double> o(0,0,0);

    // Define a line - arbitrary choice
    vgl_point_3d<double> centroid = o + v1/3.0 + v2/3.0 + v3/3.0;
    vgl_line_segment_3d<double> lineseg(o, o+2*(centroid-o));

    vgl_point_3d<double> q; // intersection point - should be centroid
    TEST("line intersects?", (Skew==vgl_triangle_3d_line_intersection(lineseg, p1, p2, p3, q)), true);
    TEST("intersection point correct?", (q-centroid).length()<tol, true);
  }
  {
    bool outside_fail = false, inside_fail = false;
    constexpr double rad = 0.01745329251994329577; // radians per degree, i.e., pi/180
    vgl_point_3d<double> i_pnt;
    for (double e=1e-12; e<0.0002; e *= 9.0) // height above/below triangle edge p1-p2
    {
      vgl_point_3d<double>  l1_in(1001,900,1000 - e), l2_in(1001,1100,1000 - e);
      vgl_point_3d<double>  l1_out(1001,900,1000 + e), l2_out(1001,1100,1000 + e);
      vgl_line_segment_3d<double> l_in(l1_in, l2_in), l_out(l1_out, l2_out);
      for (double a=-89.9; a<89.95; a+=0.1) // angle of triangle normal w.r.t to l1-l2
      {
        // Define a triangle that wobbles about p1-p2
        vgl_point_3d<double> p1(1000,1000,1000), p2(1002,1000,1000),
                             p3(1001, 1000+(20*std::sin(a*rad)), 1000-(20*std::cos(a*rad)));
        vgl_triangle_3d_intersection_t in_result1 = vgl_triangle_3d_line_intersection(l_in, p1, p2, p3, i_pnt);
        vgl_triangle_3d_intersection_t in_result2 = vgl_triangle_3d_line_intersection(l_in, p1, p3, p2, i_pnt);
        vgl_triangle_3d_intersection_t in_result3 = vgl_triangle_3d_line_intersection(l_in, p3, p1, p2, i_pnt);
        vgl_triangle_3d_intersection_t in_result4 = vgl_triangle_3d_line_intersection(l_in, p3, p2, p1, i_pnt);
        vgl_triangle_3d_intersection_t in_result5 = vgl_triangle_3d_line_intersection(l_in, p2, p3, p1, i_pnt);
        vgl_triangle_3d_intersection_t in_result6 = vgl_triangle_3d_line_intersection(l_in, p2, p1, p3, i_pnt);
        vgl_triangle_3d_intersection_t out_result1 = vgl_triangle_3d_line_intersection(l_out, p1, p2, p3, i_pnt);
        vgl_triangle_3d_intersection_t out_result2 = vgl_triangle_3d_line_intersection(l_out, p1, p3, p2, i_pnt);
        vgl_triangle_3d_intersection_t out_result3 = vgl_triangle_3d_line_intersection(l_out, p3, p1, p2, i_pnt);
        vgl_triangle_3d_intersection_t out_result4 = vgl_triangle_3d_line_intersection(l_out, p3, p2, p1, i_pnt);
        vgl_triangle_3d_intersection_t out_result5 = vgl_triangle_3d_line_intersection(l_out, p2, p3, p1, i_pnt);
        vgl_triangle_3d_intersection_t out_result6 = vgl_triangle_3d_line_intersection(l_out, p2, p1, p3, i_pnt);

        if (in_result1 != Skew || in_result2 != Skew || in_result3 != Skew ||
            in_result4 != Skew || in_result5 != Skew || in_result6 != Skew ||
            out_result1 != None || out_result2 != None || out_result3 != None ||
            out_result4 != None || out_result5 != None || out_result6 != None)
        {
          std::cout << "normal_distance: " << e << " angle: " << a;
          if (out_result1 != None)
          {
            outside_fail = true;
            std::cout << " outside 1 fail";
          }
          if (out_result2 != None)
          {
            outside_fail = true;
            std::cout << " outside 2 fail";
          }
          if (out_result3 != None)
          {
            outside_fail = true;
            std::cout << " outside 3 fail";
          }
          if (out_result4 != None)
          {
            outside_fail = true;
            std::cout << " outside 4 fail";
          }
          if (out_result5 != None)
          {
            outside_fail = true;
            std::cout << " outside 5 fail";
          }
          if (out_result6 != None)
          {
            outside_fail = true;
            std::cout << " outside 6 fail";
          }
          if (in_result1 != Skew)
          {
            inside_fail = true;
            std::cout << " inside 1 fail";
          }
          if (in_result2 != Skew)
          {
            inside_fail = true;
            std::cout << " inside 2 fail";
          }
          if (in_result3 != Skew)
          {
            inside_fail = true;
            std::cout << " inside 3 fail";
          }
          if (in_result4 != Skew)
          {
            inside_fail = true;
            std::cout << " inside 4 fail";
          }
          if (in_result5 != Skew)
          {
            inside_fail = true;
            std::cout << " inside 5 fail";
          }
          if (in_result6 != Skew)
          {
            inside_fail = true;
            std::cout << " inside 6 fail";
          }
          std::cout << std::endl;
        }
      }
    }
    TEST ("All inside rays ok", inside_fail, false);
    TEST ("All outside rays ok", outside_fail, false);
  }
}


//========================================================================
// Main testing function
//========================================================================
void test_triangle_3d_line_intersection()
{
  test_point_inside();
  test_line_intersection();
}


//========================================================================
// Define main()
//========================================================================
TESTMAIN(test_triangle_3d_line_intersection);
