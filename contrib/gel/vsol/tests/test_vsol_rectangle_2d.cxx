//*****************************************************************************
// File name: test_vsol_rectangle_2d.cxx
// Description: Test the vsol_rectangle_2d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/08| François BERTEL          |Creation
// 1.1     |2003/01/08| Peter Vanroose           |Now using testlib macros
//*****************************************************************************
#include <testlib/testlib_test.h>
//:
// \file

#include <vsol/vsol_rectangle_2d.h>
#include <vsol/vsol_rectangle_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

void test_vsol_rectangle_2d()
{
  vsol_point_2d_sptr p=new vsol_point_2d(0,0);
  vsol_point_2d_sptr q=new vsol_point_2d(1,0);
  vsol_point_2d_sptr r=new vsol_point_2d(1,1);
  vsol_rectangle_2d_sptr t=new vsol_rectangle_2d(p,q,r);

  TEST("vsol_rectangle_2d::p0()", *(t->p0()), *p);
  TEST("vsol_rectangle_2d::p1()", *(t->p1()), *q);
  TEST("vsol_rectangle_2d::p2()", *(t->p2()), *r);

  TEST("vsol_rectangle_2d::area()", t->area(), 1);
}

TESTMAIN(test_vsol_rectangle_2d);
