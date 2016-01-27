//*****************************************************************************
// File name: test_vsol_triangle_3d.cxx
// Description: Test the vsol_triangle_3d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/08| Francois BERTEL          |Creation
// 1.1     |2003/01/08| Peter Vanroose           |Now using testlib macros
//*****************************************************************************
#include <testlib/testlib_test.h>
//:
// \file

#include <vsol/vsol_triangle_3d.h>
#include <vsol/vsol_triangle_3d_sptr.h>
#include <vsol/vsol_point_3d.h>

void test_vsol_triangle_3d()
{
  vsol_point_3d_sptr p=new vsol_point_3d(0,0,10);
  vsol_point_3d_sptr q=new vsol_point_3d(1,0,10);
  vsol_point_3d_sptr r=new vsol_point_3d(0,1,10);

  vsol_triangle_3d_sptr t=new vsol_triangle_3d(p,q,r);
  TEST("Constructor", !t, false);

  TEST("vsol_triangle_3d::p0()", *(t->p0()), *p);
  TEST("vsol_triangle_3d::p1()", *(t->p1()), *q);
  TEST("vsol_triangle_3d::p2()", *(t->p2()), *r);

  TEST("vsol_triangle_3d::area()", t->area(), 0.5);

  vsol_triangle_3d_sptr t2=new vsol_triangle_3d(*t);
  TEST("Copy constructor", !t2, false);

  TEST("== operator", *t2, *t);

  r=new vsol_point_3d(10,1,-7);
  t2->set_p0(r);
  TEST("vsol_triangle_2d::set_p0()", *t2==*t, false);
}

TESTMAIN(test_vsol_triangle_3d);
