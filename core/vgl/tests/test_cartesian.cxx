// This is vxl/vgl/tests/test_cartesian.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \author Peter.Vanroose@esat.kuleuven.ac.be
// \date  4 July, 2001

#define APPROX(a,b,c) TEST(a,((b)-(c)<1e-6 && (c)-(b)<1e-6),true)
#include <vcl_iostream.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_distance.h>
//#include <vgl/vgl_polygon.h>
//#include <vgl/vgl_polygon_scan_iterator.h>
//#include <vgl/vgl_region_scan_iterator.h>
//#include <vgl/vgl_triangle_scan_iterator.h>
//#include <vgl/vgl_window_scan_iterator.h>
#include <vgl/vgl_lineseg_test.h>
//#include <vgl/vgl_polygon_test.h>
//#include <vgl/vgl_triangle_test.h>
#include <vgl/vgl_1d_basis.h>

static void test_vector_2d()
{
  // standard constructor
  vgl_vector_2d<float> const v(1.5f, 0.625f);
  vcl_cout << v << vcl_endl;

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
  APPROX("angle", angle(v1,v), 0.18939573); // 10^51'06"

  TEST("parallel", (parallel(v,v)), true);
  TEST("parallel", (parallel(v,vgl_vector_2d<float>())), true); // parallel to (0,0)
  TEST("parallel", (parallel(v,v1,0.1)), false); // not parallel, even with tol=0.1

  TEST("ratio", v/v, 1);
  TEST("ratio", (-v*3.5)/v, -3.5);

  APPROX("ratio", v1/v, 1.852071);

  APPROX("normalized", length(length(v1)*normalized(v1) -v1), 0.0);
  v0=v1;
  normalize(v1);
  APPROX("normalize", length(length(v0)*v1 - v0), 0.0);

  TEST("orthogonal", orthogonal(v,vgl_vector_2d<float>()), true); // orthogonal to (0,0)
  TEST("!orthogonal", orthogonal(v,v1,0.1), false); // even not with tolorance
  TEST("orthogonal", orthogonal(v,vgl_vector_2d<float>(0.625,-1.5)), true);
}

static void test_vector_3d()
{
  // standard constructor
  vgl_vector_3d<float> v(1.5f, 0.625f, 0.0f);
  vcl_cout << v << vcl_endl;

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
  APPROX("angle", angle(v1,v), 0.18939573); // 10^51'06"

  TEST("parallel", (parallel(v,v)), true);
  TEST("parallel", (parallel(v,vgl_vector_3d<float>())), true); // parallel to (0,0)
  TEST("parallel", (parallel(v,v1,0.1)), false); // not parallel, even with tol=0.1

  TEST("ratio", v/v, 1);
  TEST("ratio", (-v*3.5)/v, -3.5);
  APPROX("ratio", v1/v, 1.852071);

  APPROX("normalized", length(length(v1)*normalized(v1) -v1), 0.0);
  v0=v1;
  normalize(v1);
  APPROX("normalize", length(length(v0)*v1 - v0), 0.0);

  TEST("orthogonal", orthogonal(v,vgl_vector_3d<float>()), true); // orthogonal to (0,0)
  TEST("!orthogonal", orthogonal(v,v1,0.1), false); // even not with tolorance
  TEST("orthogonal", orthogonal(v,cross_product(v1,v)), true);
  TEST("orthogonal", orthogonal(v1,cross_product(v1,v)), true);
}

static void test_point_2d()
{
  int d[] = {5,5};
  vgl_point_2d<int> p1(3,7), p2(d), p3(-1,-8);
  vcl_cout << p3 << vcl_endl;

  TEST("constructor", p1.x() == 3 && p1.y()==7, true);

  TEST("inequality", (p1 != p3), true);

  p3.set(3,7);
  TEST("set", p3.x() == 3 && p3.y()==7, true);

  TEST("equality", (p1 == p3), true);

  vgl_vector_2d<int> d1 = p1 - p2;
  TEST("sum; difference", (p2+d1), p1);

  TEST("+=", (p2+=d1), p1);
  TEST("+=", p2, p1);

  p2.set(4,5);
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
}

static void test_point_3d()
{
  int d[] = {5,5,5};
  vgl_point_3d<int> p1(3,7,-1), p2(d), p3(-1,-8,7);
  vcl_cout << p3 << vcl_endl;

  TEST("inequality", (p1 != p3), true);

  p3.set(3,7,-1);
  TEST("equality", (p1 == p3), true);

  vgl_vector_3d<int> d1 = p1 - p2;
  TEST("sum; difference", (p2+d1), p1);

  TEST("+=", (p2+=d1), p1);
  TEST("+=", p2, p1);

  p2.set(4,5,2);
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
}

static void test_line_2d()
{
  double d[] = {5,5,-1};
  vgl_line_2d<double> l1(3,7,0), l2(d), l3(0,-1,-8);
  vcl_cout << l3 << vcl_endl;

  TEST("inequality", (l1 != l3), true);

  l3.set(3,7,0);
  TEST("equality", (l1 == l3), true);

  l2.set(4,5,0);
  l3.set(7,-1,0);
  bool b = concurrent(l1,l2,l3); // because they share the point (0,0)
  TEST("concurrent", b, true);

  vgl_point_2d<double> p1(1,0), p2(0,1);
  vgl_line_2d<double> li(p1,p2); // line through these two points
  vcl_cout << li << vcl_endl;
  vgl_line_2d<double> ll(1,1,-1);
  TEST("join", li, ll);

  vgl_line_segment_2d<double> ls(p1,p2); // line segment through these two points
  vcl_cout << ls << '\n';
  TEST("line segment", ls.point1(), p1);
  TEST("line segment", ls.point2(), p2);

  vgl_line_segment_2d<double> ls2(p2,p1); // inverse line segment through these points
  vcl_cout << ls2 << '\n';
  TEST("line segment intersection", vgl_lineseg_test(ls,ls2), true);
  // should be more extensively tested - TODO

  vgl_box_2d<double> bx(0,2,0,3);
  vgl_line_segment_2d<double> ls3 = vgl_clip_line_to_box(li,bx);
  vcl_cout << ls3 << '\n';
  TEST("line segment equality", ls3, ls);
  TEST("line segment equality", ls3, ls2);
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
  p2 = intersection(l,l2);
  TEST("intersection", p2, p1-dir*0.5);
}

static void test_plane_3d()
{
  double d[] = {0,3,4,1};
  vgl_plane_3d<double> pl1(3,7,-1,1), pl2(d), pl3(-1,-8,7,1);
  vcl_cout << pl3 << vcl_endl;

  TEST("inequality", (pl1 != pl3), true);

  pl3.set(3,7,-1,1);
  TEST("equality", (pl1 == pl3), true);

  vgl_vector_3d<double> d1 = pl2.normal();
  vgl_vector_3d<double> d2 = vgl_vector_3d<double>(0,0.6,0.8);
  vcl_cout << d1 << vcl_endl;
  TEST_NEAR("normal", (d1-d2).sqr_length(), 0.0, 1e-12);

  vgl_point_3d<double> p1(1,0,0), p2(0,1,0), p3(0,0,1);
  vgl_plane_3d<double> pl(p1,p2,p3); // plane through 3 points
  vcl_cout << pl << vcl_endl;
  vgl_plane_3d<double> pp(1,1,1,-1);
  TEST("join", pl, pp);
}

static void test_box_2d()
{
  // Create empty box
  vgl_box_2d<double> b;
  vcl_cout << b << vcl_endl;
  TEST("box is empty", b.is_empty(), true);
  TEST("box has no volume", b.volume(), 0.0);

  vgl_point_2d<double> p1(0,0), p2(0,1), p3(1,0);

  TEST("!contains", b.contains(p1), false);
  b.add(p1);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p1), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p1);

  TEST("!contains", b.contains(p2), false);
  b.add(p2);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p1), true);
  TEST("contains", b.contains(p2), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p1 + 0.5*(p2-p1));

  TEST("!contains", b.contains(p3), false);
  b.add(p3);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p1), true);
  TEST("contains", b.contains(p2), true);
  TEST("contains", b.contains(p3), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has volume 1", b.volume(), 1.0);
  TEST("centroid", b.centroid(), p2 + 0.5*(p3-p2));

  b.empty();
  vcl_cout << b << vcl_endl;
  TEST("!contains", b.contains(p1), false);
  TEST("!contains", b.contains(p2), false);
  TEST("!contains", b.contains(p3), false);
  TEST("box is empty", b.is_empty(), true);
  TEST("box has no volume", b.volume(), 0.0);
}

static void test_box_3d()
{
  // Create empty box
  vgl_box_3d<double> b;
  vcl_cout << b << vcl_endl;
  TEST("box is empty", b.is_empty(), true);
  TEST("box has no volume", b.volume(), 0.0);

  vgl_point_3d<double> p0(0,0,0), p1(1,0,0), p2(0,1,0), p3(0,0,1);

  TEST("!contains", b.contains(p0), false);
  b.add(p0);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p0), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p0);

  TEST("!contains", b.contains(p1), false);
  b.add(p1);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p0), true);
  TEST("contains", b.contains(p1), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p0 + 0.5*(p1-p0));

  TEST("!contains", b.contains(p2), false);
  b.add(p2);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p0), true);
  TEST("contains", b.contains(p1), true);
  TEST("contains", b.contains(p2), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has no volume", b.volume(), 0.0);
  TEST("centroid", b.centroid(), p1 + 0.5*(p2-p1));

  TEST("!contains", b.contains(p3), false);
  b.add(p3);
  vcl_cout << b << vcl_endl;
  TEST("contains", b.contains(p0), true);
  TEST("contains", b.contains(p1), true);
  TEST("contains", b.contains(p2), true);
  TEST("contains", b.contains(p3), true);
  TEST("box is not empty", b.is_empty(), false);
  TEST("box has volume 1", b.volume(), 1.0);
  TEST("centroid", b.centroid(), p1 + 0.5*(p2-p1) + 0.5*(p3-p0));

  b.empty();
  vcl_cout << b << vcl_endl;
  TEST("!contains", b.contains(p0), false);
  TEST("!contains", b.contains(p1), false);
  TEST("!contains", b.contains(p2), false);
  TEST("!contains", b.contains(p3), false);
  TEST("box is empty", b.is_empty(), true);
  TEST("box has no volume", b.volume(), 0.0);
}

inline bool collinear(vgl_line_2d<int> const& l1,
                      vgl_line_2d<int> const& l2,
                      vgl_line_2d<int> const& l3) {
  return concurrent(l1,l2,l3);
}

inline bool collinear(float, float, float) { return true; }

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
  vcl_cout << "  TEST OF PROJECTIVE BASIS WITH 1D POINTS\n";

  float p11=0.f, p12=1.f, p13=1e33f; // almost infinity ;-)
  vgl_1d_basis<float>  b_1_p(p11,p12,p13);
  // The following is essentially just a mapping
  // from float to vgl_homg_point_1d<double> :
  vgl_homg_point_1d<double> p = b_1_p.project(p11);
  TEST("origin", p, vgl_homg_point_1d<double>(0.0));
  p = b_1_p.project(p12);
  TEST("unit point", p, vgl_homg_point_1d<double>(1.0));
  p = b_1_p.project(p13);
  TEST("infinity point", p, vgl_homg_point_1d<double>(1.0,0.0));
  p = b_1_p.project(-3.f);
  TEST("point at -3", p, vgl_homg_point_1d<double>(-3.0));

  vcl_cout << "  TEST OF PROJECTIVE BASIS ON A 2D LINE\n";

  vgl_point_2d<int> p21(0,1), p22(1,3), p23(2,5); // On the line 2x-y+1=0
  vgl_1d_basis<vgl_point_2d<int> >  b_2_p(p21,p22,p23);
  p = b_2_p.project(p21);
  TEST("origin", p, vgl_homg_point_1d<double>(0.0));
  p = b_2_p.project(p22);
  TEST("unit point", p, vgl_homg_point_1d<double>(1.0));
  p = b_2_p.project(p23);
  TEST("infinity point", p, vgl_homg_point_1d<double>(1.0,0.0));
  p = b_2_p.project(vgl_point_2d<int>(-1,-1));
  TEST("point at -1", p, vgl_homg_point_1d<double>(-1/3.0));

  vcl_cout << "  TEST OF PROJECTIVE BASIS ON A 3D LINE\n";

  vgl_point_3d<double> p31(0,1,3), p32(1,3,2), p33(2,5,1); // On the line 2x-y+1=0,x+z=3
  vgl_1d_basis<vgl_point_3d<double> >  b_3_p(p31,p32,p33);
  p = b_3_p.project(p31);
  TEST("origin", p, vgl_homg_point_1d<double>(0.0));
  p = b_3_p.project(p32);
  TEST("unit point", p, vgl_homg_point_1d<double>(1.0));
  p = b_3_p.project(p33);
  TEST("infinity point", p, vgl_homg_point_1d<double>(1.0,0.0));
  p = b_3_p.project(vgl_point_3d<double>(-1,-1,4));
  TEST("point at -1", p, vgl_homg_point_1d<double>(-1/3.0));

  vcl_cout << "  TEST OF PROJECTIVE BASIS OF CONCURRENT 2D LINES\n";

  vgl_line_2d<int> l21(0,1,1), l22(1,3,1), l23(2,5,1); // Through the point (2,-1,1)
  vgl_1d_basis<vgl_line_2d<int> >  b_2_l(l21,l22,l23);
  p = b_2_l.project(l21);
  TEST("origin", p, vgl_homg_point_1d<double>(0.0));
  p = b_2_l.project(l22);
  TEST("unit point", p, vgl_homg_point_1d<double>(1.0));
  p = b_2_l.project(l23);
  TEST("infinity point", p, vgl_homg_point_1d<double>(1.0,0.0));
  p = b_2_l.project(vgl_line_2d<int>(-1,-1,1));
  vcl_cout << p << '\n';
  TEST("point at -1", p, vgl_homg_point_1d<double>(-1/3.0));
}


MAIN( test_cartesian )
{
  START( "test cartesian" );
  vcl_cout << "--- test_vector_2d ---\n";
  test_vector_2d();
  vcl_cout << "--- test_vector_3d ---\n";
  test_vector_3d();
  vcl_cout << "--- test_point_2d ---\n";
  test_point_2d();
  vcl_cout << "--- test_point_3d ---\n";
  test_point_3d();
  vcl_cout << "--- test_line_2d ---\n";
  test_line_2d();
  vcl_cout << "--- test_line_3d ---\n";
  test_line_3d();
  vcl_cout << "--- test_plane_3d ---\n";
  test_plane_3d();
  vcl_cout << "--- test_box_2d ---\n";
  test_box_2d();
  vcl_cout << "--- test_box_3d ---\n";
  test_box_3d();
  vcl_cout << "-- Testing vgl_1d_basis --\n";
  test_1d_basis();

  SUMMARY();
}

#include <vgl/vgl_1d_basis.txx>
VGL_1D_BASIS_INSTANTIATE(float);
VGL_1D_BASIS_INSTANTIATE(vgl_point_2d<int>);
VGL_1D_BASIS_INSTANTIATE(vgl_line_2d<int>);
VGL_1D_BASIS_INSTANTIATE(vgl_point_3d<double>);
