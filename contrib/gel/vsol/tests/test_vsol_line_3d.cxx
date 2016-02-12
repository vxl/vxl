//*****************************************************************************
// File name: test_vsol_line_3d.cxx
// Description: Test the vsol_line_3d class
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

#include <vgl/vgl_vector_3d.h>
#include <vsol/vsol_line_3d.h>
#include <vsol/vsol_line_3d_sptr.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>

void test_vsol_line_3d()
{
  vsol_point_3d_sptr p=new vsol_point_3d(10,4,1);
  vsol_point_3d_sptr q=new vsol_point_3d(5,1,-23);
  vsol_line_3d_sptr a=new vsol_line_3d(p,q);
  TEST("Constructor from extremities", !a, false);

  TEST("vsol_line_3d::p0()", *(a->p0()), *p);
  TEST("vsol_line_3d::p1()", *(a->p1()), *q);

  vsol_line_3d_sptr b=new vsol_line_3d(*a);
  TEST("Copy constructor", !b, false);

  TEST("== operator", *a, *b);

  vgl_vector_3d<double> v(10,-2,11);
  // choose the above numbers to obtain a "round" length: 15.

  vsol_line_3d_sptr c=new vsol_line_3d(v,p);
  TEST("Constructor from direction and middle point", !c, false);

  TEST("vsol_line_3d::middle()", *(c->middle()), *p);
  TEST("vsol_line_3d::direction()", c->direction(), v);
  TEST_NEAR("vsol_line_3d::length()", c->length(), 15, 1e-9);

  c->set_length(90);
  TEST_NEAR("vsol_line_3d::set_length()", c->length(), 90, 1e-9);

  TEST("set_length() doesn't change the middle point", *(c->middle()), *p);

  c->set_p0(p);
  TEST("vsol_line_3d::set_p0()", *(c->p0()), *p);
  c->set_p1(q);
  TEST("vsol_line_3d::set_p1()", *(c->p1()), *q);
}

TESTMAIN(test_vsol_line_3d);
