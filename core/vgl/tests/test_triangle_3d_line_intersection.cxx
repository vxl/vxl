// Some tests for vgl_triangle_3d_line_intersection

#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <vgl/vgl_triangle_3d.h>
#include <vgl/vgl_point_3d.h>


static void test_point_inside()
{
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


void test_triangle_3d_line_intersection()
{
  vcl_cout << "*******************************************\n"
           << " Testing vgl_triangle_3d_line_intersection \n"
           << "*******************************************\n\n";

  test_point_inside();
}

TESTMAIN(test_triangle_3d_line_intersection);
