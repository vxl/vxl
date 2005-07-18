//*****************************************************************************
// File name: test_vsol_polygon_2d.cxx
// Description: Test the vsol_polygon_2d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/09| François BERTEL          |Creation
// 1.1     |2001/07/03| Peter Vanroose           |Thorough check of is_convex
// 1.2     |2003/01/08| Peter Vanroose           |Now using testlib macros
//*****************************************************************************
#include <testlib/testlib_test.h>
//:
// \file

#include <vcl_vector.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

void test_vsol_polygon_2d()
{
  vcl_vector<vsol_point_2d_sptr> vertices(5);
  vertices[0]=new vsol_point_2d(0,0);
  vertices[1]=new vsol_point_2d(3,1);
  vertices[2]=new vsol_point_2d(5,3);
  vertices[3]=new vsol_point_2d(3,6);
  vertices[4]=new vsol_point_2d(1,4);

  vsol_polygon_2d_sptr poly=new vsol_polygon_2d(vertices);
  TEST("Constructor", !poly, false);

  TEST("vsol_polygon_2d::size()", poly->size(), 5);

  vsol_point_2d_sptr p=poly->vertex(0);
  TEST("vsol_polygon_2d::vertex(0)", p->x(), 0);
  TEST("vsol_polygon_2d::vertex(0)", p->y(), 0);

  p=poly->vertex(1);
  TEST("vsol_polygon_2d::vertex(1)", p->x(), 3);
  TEST("vsol_polygon_2d::vertex(1)", p->y(), 1);

  p=poly->vertex(2);
  TEST("vsol_polygon_2d::vertex(2)", p->x(), 5);
  TEST("vsol_polygon_2d::vertex(2)", p->y(), 3);

  p=poly->vertex(3);
  TEST("vsol_polygon_2d::vertex(3)", p->x(), 3);
  TEST("vsol_polygon_2d::vertex(3)", p->y(), 6);

  p=poly->vertex(4);
  TEST("vsol_polygon_2d::vertex(4)", p->x(), 1);
  TEST("vsol_polygon_2d::vertex(4)", p->y(), 4);

  TEST("vsol_polygon_2d::is_convex()", poly->is_convex(), true);

  TEST("vsol_polygon_2d::area()", poly->area(), 15.5);

  vsol_polygon_2d_sptr poly2=new vsol_polygon_2d(*poly);
  TEST("Copy constructor", !poly2, false);

  TEST("== operator", *poly2, *poly);

  // An example of a non-convex polygon with "non-convexity" between
  // start and end vertex.
  // An earlier implementation of is_convex() will fail here.
  vertices[0]=new vsol_point_2d(1,1);
  vertices[1]=new vsol_point_2d(2,1);
  vertices[2]=new vsol_point_2d(1,0);
  vertices[3]=new vsol_point_2d(0,1);
  vertices[4]=new vsol_point_2d(1,2);
  poly=new vsol_polygon_2d(vertices);

  TEST("!vsol_polygon_2d::is_convex()", poly->is_convex(), false);

  TEST("vsol_polygon_2d::area() - non-convex", poly->area(), 1.5);

  // An example of a non-convex polygon with two consecutive parallel
  // edges.  An earlier implementation of is_convex() will fail here.
  vertices[0]=new vsol_point_2d(0,2);
  vertices[1]=new vsol_point_2d(0,1);
  vertices[2]=new vsol_point_2d(1,1);
  vertices[3]=new vsol_point_2d(2,1);
  vertices[4]=new vsol_point_2d(2,0);
  poly=new vsol_polygon_2d(vertices);

  TEST("!vsol_polygon_2d::is_convex()", poly->is_convex(), false);

  // Test polygon centroid for a square
  vcl_vector<vsol_point_2d_sptr> verts(4);  
  verts[0]=new vsol_point_2d(0,0);
  verts[1]=new vsol_point_2d(2,0);
  verts[2]=new vsol_point_2d(2,2);
  verts[3]=new vsol_point_2d(0,2);
  poly = new vsol_polygon_2d(verts);
  vsol_point_2d_sptr c = poly->centroid();
  vcl_cout << "centroid " << *c << '\n';
  TEST_NEAR("centroid of a square " , (c->x()-1)*(c->y()-1) ,0.0, 1e-05);
  // Test non-convex polygon centroid
  vcl_vector<vsol_point_2d_sptr> vnon(5);  
  vnon[0]=new vsol_point_2d(0,0);
  vnon[1]=new vsol_point_2d(2,0);
  vnon[2]=new vsol_point_2d(2,2);
  vnon[3]=new vsol_point_2d(1,1);
  vnon[4]=new vsol_point_2d(0,2);
  poly = new vsol_polygon_2d(vnon);
  c = poly->centroid();
  vcl_cout << "non-convex centroid " << *c << '\n';
  TEST_NEAR("non-convex centroid " , (c->x()-1)*(c->y()-(2+(1/3))/3) ,0.0, 1e-05);
}

TESTMAIN(test_vsol_polygon_2d);
