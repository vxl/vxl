//*****************************************************************************
// File name: test_vsol_point_2d.cxx
// Description: Test the vsol_point_2d class
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

#include <vsol/vsol_point_2d.h>

void test_vsol_point_2d()
{
  vsol_point_2d_sptr p=new vsol_point_2d(10,4);
  TEST("Constructor from coordinates", !p, false);

  TEST("vsol_point_2d::x()", p->x(), 10);
  TEST("vsol_point_2d::y()", p->y(), 4);

  vsol_point_2d_sptr q=new vsol_point_2d(*p);
  TEST("Copy constructor", !q, false);

  TEST("== operator", (*p), (*q));

  q->set_x(6);
  TEST("vsol_point_2d::set_x()", q->x(), 6);
  q->set_y(7);
  TEST("vsol_point_2d::set_y()", q->y(), 7);

  TEST("!= operator", (*p)!=(*q), true);

  TEST_NEAR("vsol_point_2d::distance(point)", p->distance(q), 5, 1e-9);

  vsol_point_2d_sptr r=p->middle(*q);
  TEST("vsol_point_2d::middle()", r->x(), (p->x()+q->x())/2);
  TEST("vsol_point_2d::middle()", r->y(), (p->y()+q->y())/2);
}

TESTMAIN(test_vsol_point_2d);
