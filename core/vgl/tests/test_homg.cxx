// \file
// \author Peter.Vanroose@esat.kuleuven.ac.be
// \date  4 July, 2001

#include <vgl/vgl_test.h>
#include <vcl_iostream.h>

#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_homg_plane_3d.h>

static void test_homg_point_2d()
{
  int d[] = {5,5,1};
  vgl_homg_point_2d<int> p1(3,7,1), p2(d), p3(-1,-8,1);
  vcl_cout << p3;

  TEST("inequality", (p1 != p3), true);

  p3.set(-6,-14,-2);
  TEST("equality", (p1 == p3), true);

  vgl_vector_2d<int> d1 = p1 - p2;
  TEST("sum; difference", (p2+d1), p1);

  TEST("+=", (p2+=d1), p1);
  TEST("+=", p2, p1);
  
  p2.set(4,5,1);
  p3.set(7,-1,1);
  bool b = collinear(p1,p2,p3);
  TEST("collinear", b, true);
  double r = ratio(p1,p2,p3);
  TEST("ratio", r, 4.0);
  vgl_homg_point_2d<int> m = midpoint(p1,p2,4);
  TEST("midpoint", m, p3);

  vgl_homg_point_2d<int> c = centre(p1,p3);
  vgl_homg_point_2d<int> cc(5,3,1);
  TEST("centre", c, cc);
  vcl_vector<vgl_homg_point_2d<int> > v1;
  v1.push_back(p1); v1.push_back(p2); v1.push_back(c);
  c = centre(v1); // assignment
  TEST("centre", c, p2);
  vcl_vector<vgl_homg_point_2d<int> > v2;
  v2.push_back(p2); v2.push_back(p3); v2.push_back(cc); v2.push_back(p2);
  c = centre(v2);
  TEST("centre", c, cc);

  vgl_homg_line_2d<double> l1(0,0,1), l2(0,1,0);
  vgl_homg_point_2d<double> pi(l1,l2); // intersection
  vgl_homg_point_2d<double> pp(1,0,0); // point at infinity
  TEST("intersection", pi, pp);
  TEST("ideal", pi.ideal(), true);
  TEST("ideal", p2.ideal(), false);
}

static void test_homg_point_3d()
{
  int d[] = {5,5,5,1};
  vgl_homg_point_3d<int> p1(3,7,-1,1), p2(d), p3(-1,-8,7,1);
  vcl_cout << p3;

  TEST("inequality", (p1 != p3), true);

  p3.set(-6,-14,2,-2);
  TEST("equality", (p1 == p3), true);

  vgl_vector_3d<int> d1 = p1 - p2;
  TEST("sum; difference", (p2+d1), p1);

  TEST("+=", (p2+=d1), p1);
  TEST("+=", p2, p1);
  
  p2.set(4,5,2,1);
  p3.set(7,-1,11,1);
  bool b = collinear(p1,p2,p3);
  TEST("collinear", b, true);
  double r = ratio(p1,p2,p3);
  TEST("ratio", r, 4.0);
  vgl_homg_point_3d<int> m = midpoint(p1,p2,4);
  TEST("midpoint", m, p3);

  vgl_homg_point_3d<int> c = centre(p1,p3);
  vgl_homg_point_3d<int> cc(5,3,5,1);
  TEST("centre", c, cc);
  vcl_vector<vgl_homg_point_3d<int> > v1;
  v1.push_back(p1); v1.push_back(p2); v1.push_back(c);
  c = centre(v1); // assignment
  TEST("centre", c, p2);
  vcl_vector<vgl_homg_point_3d<int> > v2;
  v2.push_back(p2); v2.push_back(p3); v2.push_back(cc); v2.push_back(p2);
  c = centre(v2);
  TEST("centre", c, cc);

  vgl_homg_plane_3d<double> pl1(0,0,0,1), pl2(0,0,1,0), pl3(0,1,0,0);
  vgl_homg_point_3d<double> pi(pl1,pl2,pl3); // intersection
  vgl_homg_point_3d<double> pp(1,0,0,0); // point at infinity
  TEST("intersection", pi, pp);
  TEST("ideal", pi.ideal(), true);
  TEST("ideal", p2.ideal(), false);
}

void test_homg() {
  test_homg_point_2d();
  test_homg_point_3d();
}

TESTMAIN(test_homg);
