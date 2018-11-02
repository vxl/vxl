//*****************************************************************************
// File name: test_vsol_polygon_3d.cxx
// Description: Test the vsol_polygon_3d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/09| Francois BERTEL          |Creation
// 1.1     |2001/07/03| Peter Vanroose           |Thorough check of is_convex
// 1.2     |2003/01/08| Peter Vanroose           |Now using testlib macros
//*****************************************************************************
#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>

void test_vsol_polygon_3d()
{
  std::vector<vsol_point_3d_sptr> vertices(5);
  vertices[0]=new vsol_point_3d(0,0,5);
  vertices[1]=new vsol_point_3d(3,1,5);
  vertices[2]=new vsol_point_3d(5,3,5);
  vertices[3]=new vsol_point_3d(3,6,5);
  vertices[4]=new vsol_point_3d(1,4,5);

  vsol_polygon_3d_sptr poly=new vsol_polygon_3d(vertices);
  TEST("Constructor", !poly, false);

  TEST("vsol_polygon_3d::size()", poly->size(), 5);

  vsol_point_3d_sptr p=poly->vertex(0);
  TEST("vsol_polygon_3d::vertex(0)", p->x(), 0);
  TEST("vsol_polygon_3d::vertex(0)", p->y(), 0);
  TEST("vsol_polygon_3d::vertex(0)", p->z(), 5);

  p=poly->vertex(1);
  TEST("vsol_polygon_3d::vertex(1)", p->x(), 3);
  TEST("vsol_polygon_3d::vertex(1)", p->y(), 1);
  TEST("vsol_polygon_3d::vertex(1)", p->z(), 5);

  p=poly->vertex(2);
  TEST("vsol_polygon_3d::vertex(2)", p->x(), 5);
  TEST("vsol_polygon_3d::vertex(2)", p->y(), 3);
  TEST("vsol_polygon_3d::vertex(2)", p->z(), 5);

  p=poly->vertex(3);
  TEST("vsol_polygon_3d::vertex(3)", p->x(), 3);
  TEST("vsol_polygon_3d::vertex(3)", p->y(), 6);
  TEST("vsol_polygon_3d::vertex(3)", p->z(), 5);

  p=poly->vertex(4);
  TEST("vsol_polygon_3d::vertex(4)", p->x(), 1);
  TEST("vsol_polygon_3d::vertex(4)", p->y(), 4);
  TEST("vsol_polygon_3d::vertex(4)", p->z(), 5);

  TEST("vsol_polygon_3d::is_convex()", poly->is_convex(), true);

  vsol_polygon_3d_sptr poly2=new vsol_polygon_3d(*poly);
  TEST("Copy constructor", !poly2, false);

  TEST("== operator", *poly2, *poly);

  // An example of a non-convex polygon with "non-convexity" between
  // start and end vertex.
  // An earlier implementation of is_convex() will fail here.
  vertices[0]=new vsol_point_3d(1,1,5);
  vertices[1]=new vsol_point_3d(2,1,5);
  vertices[2]=new vsol_point_3d(1,0,5);
  vertices[3]=new vsol_point_3d(0,1,5);
  vertices[4]=new vsol_point_3d(1,2,5);
  poly=new vsol_polygon_3d(vertices);

  TEST("!vsol_polygon_3d::is_convex()", poly->is_convex(), false);

  // An example of a non-convex polygon with two consecutive parallel
  // edges.  An earlier implementation of is_convex() will fail here.
  vertices[0]=new vsol_point_3d(0,2,5);
  vertices[1]=new vsol_point_3d(0,1,5);
  vertices[2]=new vsol_point_3d(1,1,5);
  vertices[3]=new vsol_point_3d(2,1,5);
  vertices[4]=new vsol_point_3d(2,0,5);
  poly=new vsol_polygon_3d(vertices);

  TEST("!vsol_polygon_3d::is_convex()", poly->is_convex(), false);
}

TESTMAIN(test_vsol_polygon_3d);
