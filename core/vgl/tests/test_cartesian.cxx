// This is core/vgl/tests/test_cartesian.cxx
#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Peter Vanroose, ESAT, KULeuven.
// \date  4 July, 2001
//
// \verbatim
//  Modifications
//   PVr -  4 May 2009 - added tests for operator>>
//   PVr - 21 May 2009 - added more tests for operator>>

#include <vcl_compiler.h>
#include <vgl/vgl_tolerance.h>
#include <vgl/vgl_point_2d.hxx>
#include <vgl/vgl_point_3d.hxx>
#include <vgl/vgl_line_2d.hxx>
#include <vgl/vgl_line_segment_2d.hxx>
#include <vgl/vgl_line_3d_2_points.hxx>
#include <vgl/vgl_line_segment_3d.hxx>
#include <vgl/vgl_plane_3d.hxx>
#include <vgl/vgl_vector_2d.hxx>
#include <vgl/vgl_vector_3d.hxx>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_clip.hxx>
#include <vgl/vgl_distance.hxx>
#include <vgl/vgl_lineseg_test.hxx>
#include <vgl/vgl_1d_basis.h>
#include <vgl/vgl_intersection.hxx>

static void test_vector_2d()
{
  // standard constructor
  vgl_vector_2d<float> const v(1.5f, 0.625f);
  std::cout << v << std::endl;

  // default constructor
  vgl_vector_2d<float> v0; // == (0,0)

  // comparison
  TEST("inequality", (v != v0), true);
  // length
  TEST("length", v0.length(), 0.0);

  v0.set(1.5, 0.625);
  TEST("equality", (v == v0), true);
  v0 = v; // assignment
  TEST("equality", (v == v0), true);

  TEST("length", v.length(), 1.625);
  // should be "exact" ! (all these numbers are exactly representable in base 2)

  v0.set(1.5, 0);
  TEST("inequality", (v != v0), true);
  vgl_vector_2d<float> v1 (3,0.625);
  TEST("sum", (v+v0), v1);
  TEST("difference", (v1-v0), v);
  TEST("scale", (-v1+2*v0).x(), 0.0f);
  TEST("scale", (-v1+2*v0).y(), -0.625f);

  TEST("dot_product", dot_product(v,v1), 4.890625);
  TEST("cross_product", cross_product(v1,v), 0.625*1.5);
  TEST_NEAR("angle", angle(v1,v), 0.18939573, 1e-8); // 10^51'06"

  TEST("parallel", (parallel(v,v)), true);
  TEST("parallel", (parallel(v,vgl_vector_2d<float>())), true); // parallel to (0,0)
  TEST("parallel", (parallel(v,v1,0.1)), false); // not parallel, even with tol=0.1

  TEST("ratio", v/v, 1);
  TEST("ratio", (-v*3.5)/v, -3.5);

  TEST_NEAR("ratio", v1/v, 1.852071, 1e-6);

  TEST_NEAR("normalized", length(length(v1)*normalized(v1) -v1), 0.0, 1e-6);
  v0=v1;
  normalize(v1);
  TEST_NEAR("normalize", length(length(v0)*v1 - v0), 0.0, 1e-6);

  TEST("orthogonal", orthogonal(v,vgl_vector_2d<float>()), true); // orthogonal to (0,0)
  TEST("!orthogonal", orthogonal(v,v1,0.1), false); // even not with tolorance
  TEST("orthogonal", orthogonal(v,vgl_vector_2d<float>(0.625,-1.5)), true);

  {
    std::stringstream is; is << "4.4 -5 7e1";
    vgl_vector_2d<double> p(0.0,0.0); is >> p;
    std::cout << p << std::endl;
    TEST("istream vgl_vector_2d (blank-separated)", p, vgl_vector_2d<double>(4.4,-5));
  }
  {
    std::stringstream is; is << "7e1, 11 , blabla";
    vgl_vector_2d<double> p(0.0,0.0); is >> p;
    std::cout << p << std::endl;
    TEST("istream vgl_vector_2d (comma-separated)", p, vgl_vector_2d<double>(70,11));
  }
  {
    std::stringstream is; is << " (12,13 ) !";
    vgl_vector_2d<double> p(0.0,0.0); is >> p;
    std::cout << p << std::endl;
    TEST("istream vgl_vector_2d (parenthesized)", p, vgl_vector_2d<double>(12,13));
  }
}

static void test_vector_3d()
{
  // standard constructor
  vgl_vector_3d<float> v(1.5f, 0.625f, 0.0f);
  std::cout << v << std::endl;

  // default constructor
  vgl_vector_3d<float> v0; // == (0,0,0)

  // comparison
  TEST("inequality", (v != v0), true);
  // length
  TEST("length", v0.length(), 0.0);

  v0.set(1.5, 0.625, 0);
  TEST("equality", (v == v0), true);
  v0 = v; // assignment
  TEST("equality", (v == v0), true);

  TEST("length", v.length(), 1.625);
  // should be "exact" ! (all these numbers are exactly representable in base 2)

  v0.set(1.5, 0, 0);
  TEST("inequality", (v != v0), true);
  vgl_vector_3d<float> v1 (3,0.625, 0);
  TEST("sum", (v+v0), v1);
  TEST("difference", (v1-v0), v);
  TEST("scale", (-v1+2*v0).x(), 0.0f);
  TEST("scale", (-v1+2*v0).y(), -0.625f);
  TEST("scale", (-v1+2*v0).z(), 0);

  TEST("dot_product", dot_product(v,v1), 4.890625);
  TEST("cross_product", cross_product(v1,v).z(), 0.625*1.5);
  TEST_NEAR("angle", angle(v1,v), 0.18939573, 1e-8); // 10^51'06"
  vgl_vector_3d<double> v3(-0.20177410616759889,0,0.97943208548641414),
                        v4(-0.17524370403971115,0,0.85065080835203988);
  TEST_NEAR("pathological angle", angle(v3,v4), 0, 2e-8); // "

  TEST("parallel", (parallel(v,v)), true);
  TEST("parallel", (parallel(v,vgl_vector_3d<float>())), true); // parallel to (0,0)
  TEST("parallel", (parallel(v,v1,0.1)), false); // not parallel, even with tol=0.1

  TEST("ratio", v/v, 1);
  TEST("ratio", (-v*3.5)/v, -3.5);
  TEST_NEAR("ratio", v1/v, 1.852071, 1e-6);

  TEST_NEAR("normalized", length(length(v1)*normalized(v1) -v1), 0.0, 1e-7);
  v0=v1;
  normalize(v1);
  TEST_NEAR("normalize", length(length(v0)*v1 - v0), 0.0, 1e-7);

  TEST("orthogonal", orthogonal(v,vgl_vector_3d<float>()), true); // orthogonal to (0,0)
  TEST("!orthogonal", orthogonal(v,v1,0.1), false); // even not with tolorance
  TEST("orthogonal", orthogonal(v,cross_product(v1,v)), true);
  TEST("orthogonal", orthogonal(v1,cross_product(v1,v)), true);

  vgl_vector_3d<double> tr[4];
  double check = 0.0;

  // Orthogonal vectors test case 1
  vgl_vector_3d<double> n1(1.0, 0.0, 0.0);
  check = 0.0;
  tr[0] = orthogonal_vectors(n1,0.0);
  tr[1] = orthogonal_vectors(n1,0.25);
  tr[2] = orthogonal_vectors(n1,0.5);
  tr[3] = orthogonal_vectors(n1,0.75);
  for (unsigned i = 0; i<4; ++i)
  {
    double d = dot_product<double>(n1, tr[i]);
    check += d*d;
    d = dot_product<double>(tr[i], tr[i])-1.0;
    check += d*d;
  }
  TEST_NEAR("orthogonal vectors case 1", check, 0.0, 1e-8);

  // Orthogonal vectors test case 2
  vgl_vector_3d<double> n2(0.0, 1.0, 0.0);
  check = 0.0;
  tr[0] = orthogonal_vectors(n2,0.0);
  tr[1] = orthogonal_vectors(n2,0.25);
  tr[2] = orthogonal_vectors(n2,0.5);
  tr[3] = orthogonal_vectors(n2,0.75);
  for (unsigned i = 0; i<4; ++i)
  {
    double d = dot_product<double>(n2, tr[i]);
    check += d*d;
    d = dot_product<double>(tr[i], tr[i])-1.0;
    check += d*d;
  }
  TEST_NEAR("orthogonal vectors case 2", check, 0.0, 1e-8);

  // Orthogonal vectors test case 3
  vgl_vector_3d<double> n3(0.0, 0.0, 1.0);
  check = 0.0;
  tr[0] = orthogonal_vectors(n3,0.0);
  tr[1] = orthogonal_vectors(n3,0.25);
  tr[2] = orthogonal_vectors(n3,0.5);
  tr[3] = orthogonal_vectors(n3,0.75);
  for (unsigned i = 0; i<4; ++i)
  {
    double d = dot_product<double>(n3, tr[i]);
    check += d*d;
    d = dot_product<double>(tr[i], tr[i])-1.0;
    check += d*d;
  }
  TEST_NEAR("orthogonal vectors case 3", check, 0.0, 1e-8);

  // Orthogonal vectors test case 4
  vgl_vector_3d<double> n4(0.0, 1.0, 1.0); // NB input vector not normalized
  check = 0.0;
  tr[0] = orthogonal_vectors(n4,0.0);
  tr[1] = orthogonal_vectors(n4,0.25);
  tr[2] = orthogonal_vectors(n4,0.5);
  tr[3] = orthogonal_vectors(n4,0.75);
  for (unsigned i = 0; i<4; ++i)
  {
    double d = dot_product<double>(n4, tr[i]);
    check += d*d;
    d = dot_product<double>(tr[i], tr[i])-1.0;
    check += d*d;
  }
  TEST_NEAR("orthogonal vectors case 4", check, 0.0, 1e-8);

  // Orthogonal vectors test case 5
  vgl_vector_3d<double> n5(1.0, 1.0, 0.0); // NB input vector not normalized
  check = 0.0;
  tr[0] = orthogonal_vectors(n5,0.0);
  tr[1] = orthogonal_vectors(n5,0.25);
  tr[2] = orthogonal_vectors(n5,0.5);
  tr[3] = orthogonal_vectors(n5,0.75);
  for (unsigned i = 0; i<4; ++i)
  {
    double d = dot_product<double>(n5, tr[i]);
    check += d*d;
    d = dot_product<double>(tr[i], tr[i])-1.0;
    check += d*d;
  }
  TEST_NEAR("orthogonal vectors case 5", check, 0.0, 1e-8);

  std::stringstream is; is << "4.4 -5 7e1 10,11 , 12 (12 , 13,14 )";
  vgl_vector_3d<float> p; is >> p;
  TEST("istream vgl_vector_3d (blank-separated)", p, vgl_vector_3d<float>(4.4f,-5,70));
  is >> p;
  TEST("istream vgl_vector_3d (comma-separated)", p, vgl_vector_3d<float>(10,11,12));
  is >> p;
  TEST("istream vgl_vector_3d (parenthesized)", p, vgl_vector_3d<float>(12,13,14));
}

static void test_point_2d()
{
  int d[] = {5,5};
  vgl_point_2d<int> p1(3,7), p2(d), p3(-1,-8);
  std::cout << p3 << std::endl;

  TEST("constructor", p1.x()==3 && p1.y()==7, true);

  TEST("inequality", (p1 != p3), true);

  p3.set(3,7);
  TEST("set", p3.x()==3 && p3.y()==7, true);

  TEST("equality", (p1 == p3), true);

  vgl_vector_2d<int> d1 = p1 - p2;
  TEST("sum; difference", (p2+d1), p1);

  TEST("+=", (p2+=d1), p1);
  TEST("+=", p2, p1);

  p2.x()=4; p2.y()=5;
  TEST("set individually", p2.x()==4 && p2.y()==5, true);
  p3.set(7,-1);
  bool b = collinear(p1,p2,p3);
  TEST("collinear", b, true);
  double r = ratio(p1,p2,p3);
  TEST("ratio", r, 4.0);
  vgl_point_2d<int> m = midpoint(p1,p2,4);
  TEST("midpoint", m, p3);

  vgl_point_2d<int> c = centre(p1,p3);
  vgl_point_2d<int> cc(5,3);
  TEST("centre", c, cc);
  c = centre(p1,p2,c); // assignment
  TEST("centre", c, p2);
  c = centre(p2,p3,cc,p2);
  TEST("centre", c, cc);

  r = cross_ratio(p1,p2,c,p3);
  TEST("cross_ratio", r, 1.5);

  vgl_line_2d<double> l1(3,4,5), l2(3,2,1);
  vgl_point_2d<double> pi(l1,l2); // intersection
  vgl_point_2d<double> pp(1,-2);
  TEST("intersection", pi, pp);

  vgl_line_2d<int> l3(1,2,3), l4(3,2,1);
  vgl_point_2d<int> pj(l3,l4); // intersection
  vgl_point_2d<int> pq(1,-2);
  TEST("intersection", pj, pq);

  TEST("vgl_distance_origin", vgl_distance_origin(l1), 1);

  {
    std::stringstream is; is << "4.4 -5 7e1";
    vgl_point_2d<double> p(0.0,0.0); is >> p;
    std::cout << p << std::endl;
    TEST("istream vgl_point_2d (blank-separated)", p, vgl_point_2d<double>(4.4,-5));
  }
  {
    std::stringstream is; is << "7e1, 11 , blabla";
    vgl_point_2d<double> p(0.0,0.0); is >> p;
    std::cout << p << std::endl;
    TEST("istream vgl_point_2d (comma-separated)", p, vgl_point_2d<double>(70,11));
  }
  {
    std::stringstream is; is << " (12,13 ) !";
    vgl_point_2d<double> p(0.0,0.0); is >> p;
    std::cout << p << std::endl;
    TEST("istream vgl_point_2d (parenthesized)", p, vgl_point_2d<double>(12,13));
  }
}

static void test_point_3d()
{
  int d[] = {5,5,5};
  vgl_point_3d<int> p1(3,7,-1), p2(d), p3(-1,-8,7);
  std::cout << p3 << std::endl;

  TEST("inequality", (p1 != p3), true);

  p3.set(3,7,-1);
  TEST("equality", (p1 == p3), true);

  vgl_vector_3d<int> d1 = p1 - p2;
  TEST("sum; difference", (p2+d1), p1);

  TEST("+=", (p2+=d1), p1);
  TEST("+=", p2, p1);

  p2.set(4,5,2);
  p2.x()=4; p2.y()=5; p2.z()=2;
  TEST("set individually", p2.x()==4 && p2.y()==5 && p2.z()==2, true);
  p3.set(7,-1,11);
  bool b = collinear(p1,p2,p3);
  TEST("collinear", b, true);
  double r = ratio(p1,p2,p3);
  TEST("ratio", r, 4.0);
  vgl_point_3d<int> m = midpoint(p1,p2,4);
  TEST("midpoint", m, p3);

  vgl_point_3d<int> c = centre(p1,p3);
  vgl_point_3d<int> cc(5,3,5);
  TEST("centre", c, cc);
  c = centre(p1,p2,c); // assignment
  TEST("centre", c, p2);
  c = centre(p2,p3,cc,p2);
  TEST("centre", c, cc);

  r = cross_ratio(p1,p2,c,p3);
  TEST("cross_ratio", r, 1.5);

  vgl_plane_3d<double> pl1(1,0,0,0), pl2(0,1,0,0), pl3(0,0,1,0);
  vgl_point_3d<double> pi(pl1,pl2,pl3); // intersection
  vgl_point_3d<double> pp(0,0,0);
  TEST("intersection", pi, pp);

  std::stringstream is; is << "4.4 -5 7e1 10,11 , 12 (12 , 13,14 )";
  vgl_point_3d<float> p; is >> p;
  TEST("istream vgl_point_3d (blank-separated)", p, vgl_point_3d<float>(4.4f,-5,70));
  is >> p;
  TEST("istream vgl_point_3d (comma-separated)", p, vgl_point_3d<float>(10,11,12));
  is >> p;
  TEST("istream vgl_point_3d (parenthesized)", p, vgl_point_3d<float>(12,13,14));
}

static void test_line_2d()
{
  double d[] = {5,5,-1};
  vgl_line_2d<double> l1(3,7,0), l2(d), l3(0,-1,-8);
  std::cout << l3 << std::endl;

  TEST("inequality", (l1 != l3), true);

  l3.set(3,7,0);
  TEST("equality", (l1 == l3), true);

  l2.set(3,4,0);
  l3.set(7,-1,0);
  bool b = concurrent(l1,l2,l3); // because they share the point (0,0)
  TEST("concurrent", b, true);

  vgl_point_2d<double> p00(0,0), p01(0,1), p02(0,2), p03(0,3),
                       p10(1,0), p11(1,1), p12(1,2), p13(1,3),
                       p20(2,0), p21(2,1), p30(3,0), p31(3,1);
  vgl_line_2d<double> li(p10,p01); // line through these two points
  std::cout << li << std::endl;
  vgl_line_2d<double> ll(1,1,-1);
  TEST("join", li, ll);

  l3.set(0,-1,-8);
  TEST("vgl_distance(line,point)", vgl_distance(l3,p03), 11);
  TEST_NEAR("vgl_distance(line,point)", vgl_distance(l2,p03), 2.4, 1e-9);
  TEST_NEAR("vgl_distance(point,line)", vgl_distance(p03,l2), 2.4, 1e-9);

  vgl_line_segment_2d<double> ls(p10,p01); // line segment through these two points
  TEST("line segment first end point", ls.point1(), p10);
  TEST("line segment second end point", ls.point2(), p01);

  vgl_line_segment_2d<double> ls2(p01,p10); // inverse line segment through these points
  TEST("line segment first end point", ls2.point1(), p01);
  TEST("line segment second end point", ls2.point2(), p10);

  // line segment intersections:
  // 1. with points:
  vgl_line_segment_2d<double> ls1(p12,p30); // line segment containing p21; its support line contains p03.
  TEST("end point is coincident", vgl_lineseg_test_point(p12,ls1), true);
  TEST("mid point is coincident", vgl_lineseg_test_point(p21,ls1), true);
  TEST("point on support line is not coincident", vgl_lineseg_test_point(p03,ls1), false);
  TEST("arbitrary point is not coincident", vgl_lineseg_test_point(p11,ls1), false);
  TEST("very nearby point is not coincident", vgl_lineseg_test_point(vgl_point_2d<double>(2.01,1.0),ls1), false);
  // 2. with each other:
  TEST("identical line segments: intersect", vgl_lineseg_test_lineseg(ls,ls2), true);
  TEST("identical line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p01, p11), vgl_line_segment_2d<double>(p11, p01)), true);
  TEST("disjoint horizontal collinear line segments: do not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p01, p11), vgl_line_segment_2d<double>(p21, p31)), false);
  TEST("touching horizontal collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p01, p11), vgl_line_segment_2d<double>(p11, p21)), true);
  TEST("touching horizontal collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p01, p11), vgl_line_segment_2d<double>(p21, p11)), true);
  TEST("overlapping horizontal collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p01, p21), vgl_line_segment_2d<double>(p11, p31)), true);
  TEST("internally touching horizontal collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p01, p21), vgl_line_segment_2d<double>(p21, p11)), true);
  TEST("internally overlapping horizontal collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p01, p31), vgl_line_segment_2d<double>(p21, p11)), true);
  TEST("parallel horizontal line segments: do not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p01, p11), vgl_line_segment_2d<double>(p12, p02)), false);
  TEST("disjoint vertical collinear line segments: do not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p10, p11), vgl_line_segment_2d<double>(p12, p13)), false);
  TEST("touching vertical collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p10, p11), vgl_line_segment_2d<double>(p11, p12)), true);
  TEST("touching vertical collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p10, p11), vgl_line_segment_2d<double>(p12, p11)), true);
  TEST("overlapping vertical collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p10, p12), vgl_line_segment_2d<double>(p11, p13)), true);
  TEST("internally touching vertical collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p10, p12), vgl_line_segment_2d<double>(p12, p11)), true);
  TEST("internally overlapping vertical collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p10, p13), vgl_line_segment_2d<double>(p12, p11)), true);
  TEST("parallel vertical line segments: do not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p10, p11), vgl_line_segment_2d<double>(p21, p20)), false);
  TEST("disjoint oblique collinear line segments: do not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p30, p21), vgl_line_segment_2d<double>(p12, p03)), false);
  TEST("touching oblique collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p20, p11), vgl_line_segment_2d<double>(p11, p02)), true);
  TEST("touching oblique collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p20, p11), vgl_line_segment_2d<double>(p02, p11)), true);
  TEST("overlapping oblique collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p30, p12), vgl_line_segment_2d<double>(p21, p03)), true);
  TEST("internally touching oblique collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p30, p12), vgl_line_segment_2d<double>(p12, p21)), true);
  TEST("internally overlapping oblique collinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p30, p03), vgl_line_segment_2d<double>(p12, p21)), true);
  TEST("parallel oblique line segments: do not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p20, p11), vgl_line_segment_2d<double>(p21, p30)), false);
  TEST("disjoint noncollinear line segments: do not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p10, p21), vgl_line_segment_2d<double>(p00, p01)), false);
  TEST("touching noncollinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p10, p21), vgl_line_segment_2d<double>(p10, p01)), true);
  TEST("intersecting noncollinear line segments: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p00, p21), vgl_line_segment_2d<double>(p01, p20)), true);
  // Degenerate cases: line segments of length 0
  TEST("point on horizontal line segment: intersects",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p01, p21), vgl_line_segment_2d<double>(p11, p11)), true);
  TEST("point collinear with horizontal line segment: does not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p01, p21), vgl_line_segment_2d<double>(p31, p31)), false);
  TEST("point not collinear with horizontal line segment: does not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p01, p21), vgl_line_segment_2d<double>(p12, p12)), false);
  TEST("point on vertical line segment: intersects",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p10, p12), vgl_line_segment_2d<double>(p11, p11)), true);
  TEST("point collinear with vertical line segment: does not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p10, p12), vgl_line_segment_2d<double>(p13, p13)), false);
  TEST("point not collinear with vertical line segment: does not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p10, p12), vgl_line_segment_2d<double>(p21, p21)), false);
  TEST("point on oblique line segment: intersects",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p03, p21), vgl_line_segment_2d<double>(p12, p12)), true);
  TEST("point collinear with oblique line segment: does not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p03, p21), vgl_line_segment_2d<double>(p30, p30)), false);
  TEST("point not collinear with oblique line segment: does not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p03, p21), vgl_line_segment_2d<double>(p11, p11)), false);
  TEST("two identical points: intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p01, p01), vgl_line_segment_2d<double>(p01, p01)), true);
  TEST("two different points: does not intersect",
       vgl_lineseg_test_lineseg(vgl_line_segment_2d<double>(p01, p01), vgl_line_segment_2d<double>(p12, p12)), false);

  TEST("four almost collinear points: do not intersect",
       vgl_lineseg_test_lineseg<double>(328.99996948242187,127.99999237060547,271.98755895963365,178.20118501723579,
                                        181.43250686733217,257.93771209135798,102.99999237060547,326.99996948242187), false);

  vgl_box_2d<double> bx(0,2,0,3);
  vgl_line_segment_2d<double> ls3 = vgl_clip_line_to_box(li,bx);
  std::cout << ls3 << '\n';
  TEST("line segment equality", ls3, ls);
  TEST("line segment equality", ls3, ls2);

  TEST("normalize", l2.normalize(), true);
  TEST_NEAR("normalize: a()", l2.a(), 0.6, 1e-12);
  TEST_NEAR("normalize: b()", l2.b(), 0.8, 1e-12);

  {
    // check reason for error on other platforms - JLM
    std::stringstream is; is << "4.5 -5 7e1" << std::ends;
    std::cout << "stringstr "<< is.str() << '\n';
    vgl_line_2d<float> line;
    is >> line;
    std::cout << "formatted line " << line << '\n';;
    TEST("istream vgl_line_2d", line, vgl_line_2d<float>(4.5f,-5,70));
#if 0  // HACK This test is always failing
    std::stringstream is1;
    is1 << "9x+7y-8=0" << std::ends;
    std::cout << "stringstr 1 "<< is1.str() << '\n';
    vgl_line_2d<float> line1;
    is1 >> line1;
    std::cout << "formatted line " << line1 << '\n';
    TEST("istream vgl_line_2d formatted", line1, vgl_line_2d<float>(9,7,-8));
#endif
  }

  std::stringstream is; is << "\n4 6 7 9";
  vgl_line_segment_2d<float> l_s; is >> l_s;
  TEST("istream line_segment_2d", l_s, vgl_line_segment_2d<float>(vgl_point_2d<float>(4,6), vgl_point_2d<float>(7,9)));

  std::stringstream is1;
  vgl_point_2d<double> pf0(1.0, 2.0), pf1(3.0,4.0);
  vgl_line_segment_2d<double> lf(pf0, pf1), lfr; 
  is1<<lf;
  is1>>lfr;
  TEST("istream native out stream line_segment_2d", lfr == lf,true);
}

static void test_line_3d()
{
  vgl_point_3d<double> p1(1,0,0), p2(0,1,2);
  vgl_line_segment_3d<double> ls(p1,p2); // line segment through these two points
  TEST("line segment: point1()", ls.point1(), p1);
  TEST("line segment: point2()", ls.point2(), p2);
  ls.set(p2,p1);
  TEST("line segment: point1()", ls.point1(), p2);
  TEST("line segment: point2()", ls.point2(), p1);

  vgl_line_3d_2_points<double> l(p1,p2);
  TEST("3D line: point1()", l.point1(), p1);
  TEST("3D line: point2()", l.point2(), p2);
  l.set(p2,p1);
  TEST("3D line: point1()", l.point1(), p2);
  TEST("3D line: point2()", l.point2(), p1);
  vgl_vector_3d<double> dir = l.direction();
  TEST("3D line direction", dir, vgl_vector_3d<double>(1,-1,-2));

  TEST("line through point", collinear(l,p1), true);
  TEST("line through point", collinear(l,p1+dir), true);
  TEST("line through point", collinear(l,vgl_point_3d<double>(0,0,0)), false);

  vgl_line_3d_2_points<double> l2(p1+dir,p2-dir*1.5);
  TEST("3D line: equality", l, l2);
  l2.set(vgl_point_3d<double>(1,1,2), vgl_point_3d<double>(0,0,0));
  TEST("3D line: equality", l==l2, false);

  TEST("concurrent", concurrent(l,l2), true);
  TEST("concurrent", coplanar(l,l2), true);
  p2 = vgl_intersection(l,l2);

  bool intersection_ok = ( p2 == p1-dir*0.5 );
  TEST("intersection", intersection_ok, true);

  std::stringstream is; is << "4 5 6  (7, 8, 9)";
  vgl_line_segment_3d<float> l_s; is >> l_s;
  std::cout << l_s << std::endl;
  TEST("istream vgl_line_segment_3d", l_s, vgl_line_segment_3d<float>(vgl_point_3d<float>(4,5,6), vgl_point_3d<float>(7,8,9)));
}

static void test_plane_3d()
{
  // Define a horizontal plane in z=10 using 3 points
  vgl_plane_3d<double> pl1(vgl_point_3d<double>( 0,  0, 10),
                           vgl_point_3d<double>(10,  0, 10),
                           vgl_point_3d<double>( 0, 10, 10));
  std::cout << "plane1: " << pl1 << std::endl;

  // Define the same plane using the point+normal representation
  vgl_plane_3d<double> pl2(vgl_vector_3d<double>(0, 0, 1),
                           vgl_point_3d<double>(0, 0, 10));
  std::cout << "plane2: " << pl2 << std::endl;

  // Define the same plane using other points
  vgl_plane_3d<double> pl3(vgl_point_3d<double>( 0,  0, 10),
                           vgl_point_3d<double>(10,  0, 10),
                           vgl_point_3d<double>(10, 10, 10));
  std::cout << "plane3: " << pl3 << std::endl;

  // Are these planes considered equal?
  TEST("plane1==plane2", pl1, pl2);
  TEST("plane2==plane3", pl2, pl3);
  TEST("plane3==plane1", pl3, pl1);

  // Are certain points on all 3 planes?
  vgl_point_3d<double> pt1( 0,  0, 10); // yes
  vgl_point_3d<double> pt2( 1,  2, 10); // yes
  vgl_point_3d<double> pt3(-3,  7, 10); // yes
  vgl_point_3d<double> pt4( 5,  5,  5); // no

  double eqn11 = pl1.a()*pt1.x() + pl1.b()*pt1.y() + pl1.c()*pt1.z() + pl1.d();
  TEST("pt1 on pl1", eqn11==0, true);
  double eqn12 = pl1.a()*pt2.x() + pl1.b()*pt2.y() + pl1.c()*pt2.z() + pl1.d();
  TEST("pt2 on pl1", eqn12==0, true);
  double eqn13 = pl1.a()*pt3.x() + pl1.b()*pt3.y() + pl1.c()*pt3.z() + pl1.d();
  TEST("pt3 on pl1", eqn13==0, true);
  double eqn14 = pl1.a()*pt4.x() + pl1.b()*pt4.y() + pl1.c()*pt4.z() + pl1.d();
  TEST("pt4 NOT on pl1", eqn14==0, false);

  double eqn21 = pl2.a()*pt1.x() + pl2.b()*pt1.y() + pl2.c()*pt1.z() + pl2.d();
  TEST("pt1 on pl2", eqn21==0, true);
  double eqn22 = pl2.a()*pt2.x() + pl2.b()*pt2.y() + pl2.c()*pt2.z() + pl2.d();
  TEST("pt2 on pl2", eqn22==0, true);
  double eqn23 = pl2.a()*pt3.x() + pl2.b()*pt3.y() + pl2.c()*pt3.z() + pl2.d();
  TEST("pt3 on pl2", eqn23==0, true);
  double eqn24 = pl2.a()*pt4.x() + pl2.b()*pt4.y() + pl2.c()*pt4.z() + pl2.d();
  TEST("pt4 NOT on pl2", eqn24==0, false);

  double eqn31 = pl3.a()*pt1.x() + pl3.b()*pt1.y() + pl3.c()*pt1.z() + pl3.d();
  TEST("pt1 on pl3", eqn31==0, true);
  double eqn32 = pl3.a()*pt2.x() + pl3.b()*pt2.y() + pl3.c()*pt2.z() + pl3.d();
  TEST("pt2 on pl3", eqn32==0, true);
  double eqn33 = pl3.a()*pt3.x() + pl3.b()*pt3.y() + pl3.c()*pt3.z() + pl3.d();
  TEST("pt3 on pl3", eqn33==0, true);
  double eqn34 = pl3.a()*pt4.x() + pl3.b()*pt4.y() + pl3.c()*pt4.z() + pl3.d();
  TEST("pt4 NOT on pl3", eqn34==0, false);

  // And some other planes:
  double d[] = {0,3,4,1};
  vgl_plane_3d<double> plane1(3,7,-1,1), plane2(d), plane3(-1,-8,7,1);
  std::cout << plane3 << std::endl;

  TEST("inequality", (plane1 != plane3), true);

  plane3.set(3,7,-1,1);
  TEST("equality", (plane1 == plane3), true);

  vgl_vector_3d<double> d1 = plane2.normal();
  vgl_vector_3d<double> d2 = vgl_vector_3d<double>(0,0.6,0.8);
  std::cout << d1 << std::endl;
  TEST_NEAR("normal", (d1-d2).sqr_length(), 0.0, 1e-12);

  vgl_point_3d<double> p1(1,0,0), p2(0,1,0), p3(0,0,1);
  vgl_plane_3d<double> pl(p1,p2,p3); // plane through 3 points
  std::cout << pl << std::endl;
  vgl_plane_3d<double> pp(1,1,1,-1);
  TEST("join", pl, pp);

  TEST_NEAR("vgl_distance(plane,point)", vgl_distance(plane2,p2), 0.8, 1e-9);
  TEST("vgl_distance(point,plane)", vgl_distance(p3,plane2), 1);

  std::stringstream is; is << "4.5 -5 7e1 5e-1   -6x+7y-8z+9=0";
  // check error on other platforms - JLM
  std::cout << is.str();
  vgl_plane_3d<double> l; is >> l;
  TEST("istream vgl_plane_3d", l, vgl_plane_3d<double>(4.5,-5,70,0.5));
#if 0  // HACK This test is always failing
  is >> l;
  std::cout << "second form for l " << l << '\n';
  TEST("istream vgl_plane_3d formatted", l, vgl_plane_3d<double>(-6,7,-8,9));
#endif
}

static void test_box_2d()
{
  // Create empty box
  vgl_box_2d<double> b;
  std::cout << b << std::endl;
  TEST("box is empty", b.is_empty(), true);
  TEST("box has no volume", b.volume(), 0.0);

  vgl_point_2d<double> p0(0,0), p1(1,0), p2(0,1), p12(1,1);
  TEST("!contains(p0)", b.contains(p0), false);
  TEST("!contains(p1)", b.contains(p1), false);
  TEST("!contains(p2)", b.contains(p2), false);
  TEST("!contains(p12)", b.contains(p12), false);

  b.add(p0); std::cout << b << std::endl;
  TEST("contains(p0)", b.contains(p0), true);
  TEST("!contains(p1)", b.contains(p1), false);
  TEST("!contains(p2)", b.contains(p2), false);
  TEST("!contains(p12)", b.contains(p12), false);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p0);
  TEST("min_point", b.min_point(), p0);
  TEST("max_point", b.max_point(), p0);

  b.add(p1); std::cout << b << std::endl;
  TEST("contains(p0)", b.contains(p0), true);
  TEST("contains(p1)", b.contains(p1), true);
  TEST("!contains(p2)", b.contains(p2), false);
  TEST("!contains(p12)", b.contains(p12), false);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p0 + 0.5*(p1-p0));
  TEST("min_point", b.min_point(), p0);
  TEST("max_point", b.max_point(), p1);

  b.add(p2); std::cout << b << std::endl;
  TEST("contains(p0)", b.contains(p0), true);
  TEST("contains(p1)", b.contains(p1), true);
  TEST("contains(p2)", b.contains(p2), true);
  TEST("contains(p12)", b.contains(p12), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has volume 1", b.volume(), 1.0);
  TEST("centroid", b.centroid(), p1 + 0.5*(p2-p1));
  TEST("min_point", b.min_point(), p0);
  TEST("max_point", b.max_point(), p12);

  vgl_point_2d<double> p4(0.5,0.5), p5(2,2);
  vgl_box_2d<double> b2(p4,p5);
  TEST("box has volume 2.25", b2.volume(), 2.25);
  TEST("!contains(b2)", b.contains(b2), false);
  vgl_box_2d<double> b3(p5,p4);
  TEST("boxes are equal", b3, b2);
  b.add(b2); std::cout << b << std::endl;
  TEST("union box has volume 4", b.volume(), 4.0);
  TEST("contains(b2)", b.contains(b2), true);
  TEST("centroid", b.centroid(), p12);
  TEST("min_point", b.min_point(), p0);
  TEST("max_point", b.max_point(), p5);

  b2=b; b2.set_centroid(p0); std::cout << b2 << std::endl;
  TEST("set_centroid", b2.centroid(), p0);
  TEST("volume did not change", b2.volume(), 4.0);
  TEST("min_point", b2.min_point(), vgl_point_2d<double>(-1,-1));
  TEST("max_point", b2.max_point(), p12);

  b2.set_width(1.0); std::cout << b2 << std::endl;
  TEST("set_width", b2.centroid(), p0);
  TEST("volume is now 2", b2.volume(), 2.0);
  b2.set_height(1.0); std::cout << b2 << std::endl;
  TEST("set_height", b2.centroid(), p0);
  TEST("volume is now 1", b2.volume(), 1.0);
  b2.scale_about_centroid(2.0);
  TEST("scale_about_centroid", b2.centroid(), p0);
  TEST("volume is now 4", b2.volume(), 4.0);
  b2.scale_about_centroid(0.5);
  TEST("scale_about_centroid", b2.centroid(), p0);
  TEST("volume is now 1", b2.volume(), 1.0);
  b2.expand_about_centroid(1.0);
  TEST("expand_about_centroid", b2.centroid(), p0);
  TEST("volume is now 4", b2.volume(), 4.0);
  b2.expand_about_centroid(-1.0);
  TEST("expand_about_centroid", b2.centroid(), p0);
  TEST("volume is now 1", b2.volume(), 1.0);
  b2.set_centroid(p12);
  b2.set_min_point(p0);
  TEST("set_min_point", b2.volume(), 2.25);
  b2.set_max_point(b.max_point());
  TEST("set_max_point", b2, b);

  double d0[2] = {0.0,0.0}, d1[2] = {1.0,1.0}, d2[2] = {2.0,2.0};
  b = vgl_box_2d<double>(d0,d2); std::cout << b << std::endl;
  TEST("construct from two points", b, b2);
  b = vgl_box_2d<double>(d1,2,2,vgl_box_2d<double>::centre); std::cout << b << std::endl;
  TEST("construct from centroid", b, b2);
  b = vgl_box_2d<double>(p12,2,2,vgl_box_2d<double>::centre); std::cout << b << std::endl;
  TEST("construct from centroid", b, b2);
  p12 = b2.min_point();
  b = vgl_box_2d<double>(p12,2,2,vgl_box_2d<double>::min_pos); std::cout << b << std::endl;
  TEST("construct from min_pos", b, b2);
  p12 = b2.max_point();
  b = vgl_box_2d<double>(p12,2,2,vgl_box_2d<double>::max_pos); std::cout << b << std::endl;
  TEST("construct from max_pos", b, b2);

  b.empty(); std::cout << b << std::endl;
  TEST("empty", b2==b, false);
  TEST("!contains(p0)", b.contains(p0), false);
  TEST("!contains(p1)", b.contains(p1), false);
  TEST("!contains(p2)", b.contains(p2), false);
  TEST("box is empty", b.is_empty(), true);
  TEST("box has no volume", b.volume(), 0.0);

  vgl_box_2d<int> ib(10, 11, 10, 11);
  std::cout << ib << std::endl;
  TEST("Integer box centroid", ib.centroid_x()==10 && ib.centroid_y()==10, true);
  ib.set_width(1); ib.set_height(1);
  std::cout << ib << std::endl;
  TEST("Integer box centroid drift", ib.centroid_x()==10 && ib.centroid_y()==10, true);

  ib = vgl_box_2d<int>(10, 11, 10, 11);
  ib.set_width(4); ib.set_height(4);
  ib.set_width(3); ib.set_height(3);
  ib.set_width(4); ib.set_height(4);
  ib.set_width(3); ib.set_height(3);
  ib.set_width(4); ib.set_height(4);
  std::cout << ib << ib.centroid() << std::endl;
  TEST("Integer box centroid drift", ib.centroid_x()==10 && ib.centroid_y()==10, true);

  ib = vgl_box_2d<int>(9, 11, 9, 11);
  std::cout << ib << ib.centroid() << std::endl;
  ib.set_width(3); ib.set_height(3);
  std::cout << ib << ib.centroid() << std::endl;
  TEST("Integer box centroid", ib.centroid_x()==10 && ib.centroid_y()==10, true);

  ib = vgl_box_2d<int>(-11, -10, -11, -10);
  std::cout << ib << ib.centroid() << std::endl;
  ib.set_width(3); ib.set_height(3);
  ib.set_width(4); ib.set_height(4);
  ib.set_width(3); ib.set_height(3);
  ib.set_width(4); ib.set_height(4);
  ib.set_width(3); ib.set_height(3);
  std::cout << ib << ib.centroid() << std::endl;
  TEST("Integer box negative centroid drift", ib.centroid_x()==-10 && ib.centroid_y()==-10, true);

  ib = vgl_box_2d<int>(-11, -9, -11, -9);
  std::cout << ib << ib.centroid() << std::endl;
  ib.set_width(3); ib.set_height(3);
  ib.set_width(4); ib.set_height(4);
  ib.set_width(3); ib.set_height(3);
  ib.set_width(4); ib.set_height(4);
  std::cout << ib << ib.centroid() << std::endl;
  TEST("Integer box negative centroid drift", ib.centroid_x()==-10 && ib.centroid_y()==-10, true);

  vgl_point_2d<double> min1(10,10), max1(20,20),
                       min2(40,40), max2(50,50),
                       min3(45,45), max3(55,55);

  vgl_box_2d<double> box1(min1, max1);
  vgl_box_2d<double> box2(min2, max2);
  vgl_box_2d<double> box3(min3, max3);

  //no intersection case
  vgl_box_2d<double> i1 = vgl_intersection(box1, box2);
  TEST("vgl_intersection(box1, box2) = false", true, i1.is_empty());

  //intersection case
  vgl_box_2d<double> i2 = vgl_intersection(box2, box3);
  TEST("vgl_intersection(box2, box3) = true", false, i2.is_empty());
  TEST("vgl_intersection(box2, box3) volume", 25.0, i2.volume());

  std::stringstream is; is << "4.4 -5 7e1 5e-1";
  vgl_box_2d<double> l; is >> l;
  TEST("istream vgl_box_2d", l, vgl_box_2d<double>(4.4,70,-5,0.5)); // note different order!!
}

static void test_box_3d()
{
  // Create empty box
  vgl_box_3d<double> b;
  std::cout << b << std::endl;
  TEST("box is empty", b.is_empty(), true);
  TEST("box has no volume", b.volume(), 0.0);

  vgl_point_3d<double> p0(0,0,0), p1(1,0,0), p2(0,1,0), p3(0,0,1);
  vgl_point_3d<double> p12(1,1,0), p13(1,0,1), p23(0,1,1), p123(1,1,1);
  TEST("!contains(p0)", b.contains(p0), false);
  TEST("!contains(p1)", b.contains(p1), false);
  TEST("!contains(p2)", b.contains(p2), false);
  TEST("!contains(p3)", b.contains(p3), false);
  TEST("!contains(p12)", b.contains(p12), false);
  TEST("!contains(p13)", b.contains(p13), false);
  TEST("!contains(p23)", b.contains(p23), false);
  TEST("!contains(p123)", b.contains(p123), false);

  b.add(p0); std::cout << b << std::endl;
  TEST("contains(p0)", b.contains(p0), true);
  TEST("!contains(p1)", b.contains(p1), false);
  TEST("!contains(p2)", b.contains(p2), false);
  TEST("!contains(p3)", b.contains(p3), false);
  TEST("!contains(p12)", b.contains(p12), false);
  TEST("!contains(p13)", b.contains(p13), false);
  TEST("!contains(p23)", b.contains(p23), false);
  TEST("!contains(p123)", b.contains(p123), false);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p0);
  TEST("min_point", b.min_point(), p0);
  TEST("max_point", b.max_point(), p0);

  b.add(p1); std::cout << b << std::endl;
  TEST("contains(p0)", b.contains(p0), true);
  TEST("contains(p1)", b.contains(p1), true);
  TEST("!contains(p2)", b.contains(p2), false);
  TEST("!contains(p3)", b.contains(p3), false);
  TEST("!contains(p12)", b.contains(p12), false);
  TEST("!contains(p13)", b.contains(p13), false);
  TEST("!contains(p23)", b.contains(p23), false);
  TEST("!contains(p123)", b.contains(p123), false);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p0 + 0.5*(p1-p0));
  TEST("min_point", b.min_point(), p0);
  TEST("max_point", b.max_point(), p1);

  b.add(p2); std::cout << b << std::endl;
  TEST("contains(p0)", b.contains(p0), true);
  TEST("contains(p1)", b.contains(p1), true);
  TEST("contains(p2)", b.contains(p2), true);
  TEST("!contains(p3)", b.contains(p3), false);
  TEST("contains(p12)", b.contains(p12), true);
  TEST("!contains(p13)", b.contains(p13), false);
  TEST("!contains(p23)", b.contains(p23), false);
  TEST("!contains(p123)", b.contains(p123), false);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p1 + 0.5*(p2-p1));
  TEST("min_point", b.min_point(), p0);
  TEST("max_point", b.max_point(), p12);

  b.add(p3); std::cout << b << std::endl;
  TEST("contains(p0)", b.contains(p0), true);
  TEST("contains(p1)", b.contains(p1), true);
  TEST("contains(p2)", b.contains(p2), true);
  TEST("contains(p3)", b.contains(p3), true);
  TEST("contains(p12)", b.contains(p12), true);
  TEST("contains(p13)", b.contains(p13), true);
  TEST("contains(p23)", b.contains(p23), true);
  TEST("contains(p123)", b.contains(p123), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has volume 1", b.volume(), 1.0);
  TEST("centroid", b.centroid(), p1 + 0.5*(p2-p1) + 0.5*(p3-p0));
  TEST("min_point", b.min_point(), p0);
  TEST("max_point", b.max_point(), p123);

  vgl_point_3d<double> p4(0.5,0.5,0.5), p5(2,2,2);
  vgl_box_3d<double> b2(p4,p5);
  TEST("box has volume 3.375", b2.volume(), 3.375);
  TEST("!contains(b2)", b.contains(b2), false);
  vgl_box_3d<double> b3(p5,p4);
  TEST("boxes are equal", b3, b2);
  b.add(b2); std::cout << b << std::endl;
  TEST("union box has volume 8", b.volume(), 8.0);
  TEST("contains(b2)", b.contains(b2), true);
  TEST("centroid", b.centroid(), p123);
  TEST("min_point", b.min_point(), p0);
  TEST("max_point", b.max_point(), p5);

  b2=b; b2.set_centroid(p0); std::cout << b2 << std::endl;
  TEST("set_centroid", b2.centroid(), p0);
  TEST("volume did not change", b2.volume(), 8.0);
  TEST("min_point", b2.min_point(), vgl_point_3d<double>(-1,-1,-1));
  TEST("max_point", b2.max_point(), p123);

  b2.set_width(1.0); std::cout << b2 << std::endl;
  TEST("set_width", b2.centroid(), p0);
  TEST("volume is now 4", b2.volume(), 4.0);
  b2.set_height(1.0); std::cout << b2 << std::endl;
  TEST("set_height", b2.centroid(), p0);
  TEST("volume is now 2", b2.volume(), 2.0);
  b2.set_depth(1.0); std::cout << b2 << std::endl;
  TEST("set_depth", b2.centroid(), p0);
  TEST("volume is now 1", b2.volume(), 1.0);
  b2.scale_about_centroid(2.0);
  TEST("scale_about_centroid", b2.centroid(), p0);
  TEST("volume is now 8", b2.volume(), 8.0);
  b2.scale_about_centroid(0.5);
  TEST("scale_about_centroid", b2.centroid(), p0);
  TEST("volume is now 1", b2.volume(), 1.0);
  b2.expand_about_centroid(1.0);
  TEST("expand_about_centroid", b2.centroid(), p0);
  TEST("volume is now 8", b2.volume(), 8.0);
  b2.expand_about_centroid(-1.0);
  TEST("expand_about_centroid", b2.centroid(), p0);
  TEST("volume is now 1", b2.volume(), 1.0);

  b2.set_centroid(p123);
  b2.set_min_point(p0);
  TEST("set_min_point", b2.volume(), 3.375);
  b2.set_max_point(b.max_point());
  TEST("set_max_point", b2, b);

  double d0[3] = {0.0,0.0,0.0}, d1[3] = {1.0,1.0,1.0}, d2[3] = {2.0,2.0,2.0};
  b = vgl_box_3d<double>(d0,d2); std::cout << b << std::endl;
  TEST("construct from two points", b, b2);
  b = vgl_box_3d<double>(d1,2,2,2,vgl_box_3d<double>::centre); std::cout << b << std::endl;
  TEST("construct from centroid", b, b2);
  b = vgl_box_3d<double>(p123,2,2,2,vgl_box_3d<double>::centre); std::cout << b << std::endl;
  TEST("construct from centroid", b, b2);
  p123 = b2.min_point();
  b = vgl_box_3d<double>(p123,2,2,2,vgl_box_3d<double>::min_pos); std::cout << b << std::endl;
  TEST("construct from min_pos", b, b2);
  p123 = b2.max_point();
  b = vgl_box_3d<double>(p123,2,2,2,vgl_box_3d<double>::max_pos); std::cout << b << std::endl;
  TEST("construct from max_pos", b, b2);

  b.empty(); std::cout << b << std::endl;
  TEST("empty", b2==b, false);
  TEST("!contains(p0)", b.contains(p0), false);
  TEST("!contains(p1)", b.contains(p1), false);
  TEST("!contains(p2)", b.contains(p2), false);
  TEST("!contains(p3)", b.contains(p3), false);
  TEST("box is empty", b.is_empty(), true);
  TEST("box has no volume", b.volume(), 0.0);

  vgl_point_3d<double> min1(10,10,10), max1(20,20,20),
                       min2(40,40,40), max2(50,50,50),
                       min3(45,45,45), max3(55,55,55);

  vgl_box_3d<double> box1(min1, max1);
  vgl_box_3d<double> box2(min2, max2);
  vgl_box_3d<double> box3(min3, max3);

  //no intersection case
  vgl_box_3d<double> i1 = vgl_intersection(box1, box2);
  TEST("vgl_intersection(box1, box2) = false", true, i1.is_empty());

  //intersection case
  vgl_box_3d<double> i2 = vgl_intersection(box2, box3);
  TEST("vgl_intersection(box2, box3) = true", false, i2.is_empty());
  TEST("vgl_intersection(box2, box3) volume", 125.0, i2.volume());

  std::stringstream is; is << "4.4 -5 0 7e1 5e-1 0";
  vgl_box_3d<float> bx; is >> bx;
  TEST("istream vgl_box_3d", bx, vgl_box_3d<float>(4.4f,-5,0,70,0.5f,0));
}

inline bool collinear(vgl_line_2d<int> const& l1,
                      vgl_line_2d<int> const& l2,
                      vgl_line_2d<int> const& l3)
{
  return concurrent(l1,l2,l3);
}

inline bool collinear(float const&, float const&, float const&) { return true; }

inline double ratio(vgl_line_2d<int> const& l1,
                    vgl_line_2d<int> const& l2,
                    vgl_line_2d<int> const& l3)
{
  return (l3.a()-l1.a())/(l2.a()-l1.a());
}

inline double ratio(float f1, float f2, float f3)
{
  return (f3-f1)/(f2-f1);
}

inline double cross_ratio(vgl_line_2d<int>const& l1, vgl_line_2d<int>const& l2,
                          vgl_line_2d<int>const& l3, vgl_line_2d<int>const& l4)
{
  vgl_point_2d<int> p1(l1.a()/l1.c(),l1.b()/l1.c());
  vgl_point_2d<int> p2(l2.a()/l2.c(),l2.b()/l2.c());
  vgl_point_2d<int> p3(l3.a()/l3.c(),l3.b()/l3.c());
  vgl_point_2d<int> p4(l4.a()/l4.c(),l4.b()/l4.c());
  return cross_ratio(p1,p2,p3,p4);
}

inline double cross_ratio(float f1, float f2, float f3, float f4)
{
  return (f1-f3)*(f2-f4)/((f1-f4)*(f2-f3));
}

inline bool is_ideal(float) { return false; }

static void test_1d_basis()
{
  std::cout << "  TEST OF PROJECTIVE BASIS WITH 1D POINTS\n";

  float p11=0.f, p12=1.f, p13=1e33f; // almost infinity ;-)
  vgl_1d_basis<float> b_1_p(p11,p12,p13);
  // The following is essentially just a mapping
  // from float to vgl_homg_point_1d<double> :
  vgl_homg_point_1d<double> p = b_1_p.project(p11);
  TEST("origin", p, vgl_homg_point_1d<double>(0.0));
  p = b_1_p.project(p12);
  TEST("unit point", p, vgl_homg_point_1d<double>(1.0));
  p = b_1_p.project(p13);
  TEST("infinity point", p, vgl_homg_point_1d<double>(1.0,0.0));
  p = b_1_p.project(-3.f);
  TEST_NEAR("point at -3", vgl_distance(p, vgl_homg_point_1d<double>(-3.0)), 0.0, 1e-6);

  std::cout << "  TEST OF PROJECTIVE BASIS ON A 2D LINE\n";

  vgl_point_2d<int> p21(0,1), p22(1,3), p23(2,5); // On the line 2x-y+1=0
  vgl_1d_basis<vgl_point_2d<int> > b_2_p(p21,p22,p23);
  p = b_2_p.project(p21);
  TEST("origin", p, vgl_homg_point_1d<double>(0.0));
  p = b_2_p.project(p22);
  TEST("unit point", p, vgl_homg_point_1d<double>(1.0));
  p = b_2_p.project(p23);
  TEST("infinity point", p, vgl_homg_point_1d<double>(1.0,0.0));
  p = b_2_p.project(vgl_point_2d<int>(-1,-1));
  TEST("point at -1", p, vgl_homg_point_1d<double>(-1/3.0));

  std::cout << "  TEST OF PROJECTIVE BASIS ON A 3D LINE\n";

  vgl_point_3d<double> p31(0,1,3), p32(1,3,2), p33(2,5,1); // On the line 2x-y+1=0,x+z=3
  vgl_1d_basis<vgl_point_3d<double> > b_3_p(p31,p32,p33);
  p = b_3_p.project(p31);
  TEST("origin", p, vgl_homg_point_1d<double>(0.0));
  p = b_3_p.project(p32);
  TEST("unit point", p, vgl_homg_point_1d<double>(1.0));
  p = b_3_p.project(p33);
  TEST("infinity point", p, vgl_homg_point_1d<double>(1.0,0.0));
  p = b_3_p.project(vgl_point_3d<double>(-1,-1,4));
  TEST("point at -1", p, vgl_homg_point_1d<double>(-1/3.0));

  std::cout << "  TEST OF PROJECTIVE BASIS OF CONCURRENT 2D LINES\n";

  vgl_line_2d<int> l21(0,1,1), l22(1,3,1), l23(2,5,1); // Through the point (2,-1,1)
  vgl_1d_basis<vgl_line_2d<int> > b_2_l(l21,l22,l23);
  p = b_2_l.project(l21);
  TEST("origin", p, vgl_homg_point_1d<double>(0.0));
  p = b_2_l.project(l22);
  TEST("unit point", p, vgl_homg_point_1d<double>(1.0));
  p = b_2_l.project(l23);
  TEST("infinity point", p, vgl_homg_point_1d<double>(1.0,0.0));
  p = b_2_l.project(vgl_line_2d<int>(-1,-1,1));
  std::cout << p << '\n';
  TEST("point at -1", p, vgl_homg_point_1d<double>(-1/3.0));
}


static void test_cartesian()
{
  std::cout << "--- test_vector_2d ---\n";     test_vector_2d();
  std::cout << "--- test_vector_3d ---\n";     test_vector_3d();
  std::cout << "--- test_point_2d ---\n";      test_point_2d();
  std::cout << "--- test_point_3d ---\n";      test_point_3d();
  std::cout << "--- test_line_2d ---\n";       test_line_2d();
  std::cout << "--- test_line_3d ---\n";       test_line_3d();
  std::cout << "--- test_plane_3d ---\n";      test_plane_3d();
  std::cout << "--- test_box_2d ---\n";        test_box_2d();
  std::cout << "--- test_box_3d ---\n";        test_box_3d();
  std::cout << "-- Testing vgl_1d_basis --\n"; test_1d_basis();
}

TESTMAIN(test_cartesian);

#include <vgl/vgl_1d_basis.hxx>
VGL_1D_BASIS_INSTANTIATE(float);
VGL_1D_BASIS_INSTANTIATE(vgl_point_2d<int>);
VGL_1D_BASIS_INSTANTIATE(vgl_line_2d<int>);
VGL_1D_BASIS_INSTANTIATE(vgl_point_3d<double>);
