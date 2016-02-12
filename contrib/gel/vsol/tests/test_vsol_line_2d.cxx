//*****************************************************************************
// File name: test_vsol_line_2d.cxx
// Description: Test the vsol_line_2d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/02| Francois BERTEL          |Creation
// 1.1     |2003/01/08| Peter Vanroose           |Now using testlib macros
//*****************************************************************************
#include <testlib/testlib_test.h>
//:
// \file

#include <vgl/vgl_vector_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

void test_vsol_line_2d()
{
  vsol_point_2d_sptr p=new vsol_point_2d(10,4);
  vsol_point_2d_sptr q=new vsol_point_2d(5,1);
  vsol_line_2d_sptr a=new vsol_line_2d(p,q);
  TEST("Constructor from extremities", !a, false);

  TEST("vsol_line_2d::p0()", *(a->p0()), *p);
  TEST("vsol_line_2d::p1()", *(a->p1()), *q);

  vsol_line_2d_sptr b=new vsol_line_2d(*a);
  TEST("Copy constructor", !b, false);

  TEST("== operator", *a, *b);

  vgl_vector_2d<double> v(8,-6);

  vsol_line_2d_sptr c=new vsol_line_2d(v,p);
  TEST("Constructor from direction and middle point", !c, false);

  TEST("vsol_line_2d::middle()", *(c->middle()), *p);
  TEST("vsol_line_2d::direction()", c->direction(), v);
  TEST_NEAR("vsol_line_2d::length()", c->length(), 10, 1e-9);

  c->set_length(90);
  TEST_NEAR("vsol_line_2d::set_length()", c->length(), 90, 1e-9);

  TEST("set_length() doesn't change the middle point", *(c->middle()), *p);

  c->set_p0(p);
  TEST("vsol_line_2d::set_p0()", *(c->p0()), *p);
  c->set_p1(q);
  TEST("vsol_line_2d::set_p1()", *(c->p1()), *q);
}

TESTMAIN(test_vsol_line_2d);
