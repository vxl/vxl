//*****************************************************************************
// File name: test_vsol_rectangle_3d.cxx
// Description: Test the vsol_rectangle_3d class
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

#include <vsol/vsol_rectangle_3d.h>
#include <vsol/vsol_rectangle_3d_sptr.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>

void test_vsol_rectangle_3d()
{
  vsol_point_3d_sptr p=new vsol_point_3d(0,0,10);
  vsol_point_3d_sptr q=new vsol_point_3d(1,0,10);
  vsol_point_3d_sptr r=new vsol_point_3d(1,1,10);
  vsol_rectangle_3d_sptr t=new vsol_rectangle_3d(p,q,r);

  TEST("vsol_rectangle_3d::p0()", *(t->p0()), *p);
  TEST("vsol_rectangle_3d::p1()", *(t->p1()), *q);
  TEST("vsol_rectangle_3d::p2()", *(t->p2()), *r);

  TEST("vsol_rectangle_3d::area()", t->area(), 1);
}

TESTMAIN(test_vsol_rectangle_3d);
