//*****************************************************************************
// File name: test_vsol_point_3d.cxx
// Description: Test the vsol_point_3d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/04| Francois BERTEL          |Creation
// 1.1     |2003/01/08| Peter Vanroose           |Now using testlib macros
//*****************************************************************************
#include <testlib/testlib_test.h>
//:
// \file

#include <vsol/vsol_point_3d.h>

void test_vsol_point_3d()
{
  vsol_point_3d_sptr p=new vsol_point_3d(10,4,1);
  TEST("Constructor from coordinates", !p, false);

  TEST("vsol_point_3d::x()", p->x(), 10);
  TEST("vsol_point_3d::y()", p->y(), 4);
  TEST("vsol_point_3d::z()", p->z(), 1);

  vsol_point_3d_sptr q=new vsol_point_3d(*p);
  TEST("Copy constructor", !q, false);

  TEST("== operator", (*p), (*q));

  q->set_x(0);
  TEST("vsol_point_3d::set_x()", q->x(), 0);
  q->set_y(6);
  TEST("vsol_point_3d::set_y()", q->y(), 6);
  q->set_z(-10);
  TEST("vsol_point_3d::set_z()", q->z(), -10);

  TEST("!= operator", (*p)!=(*q), true);

  TEST_NEAR("vsol_point_3d::distance(point)", p->distance(q), 15, 1e-9);

  vsol_point_3d_sptr r=p->middle(*q);
  TEST("vsol_point_3d::middle()", r->x(), (p->x()+q->x())/2);
  TEST("vsol_point_3d::middle()", r->y(), (p->y()+q->y())/2);
  TEST("vsol_point_3d::middle()", r->z(), (p->z()+q->z())/2);
}

TESTMAIN(test_vsol_point_3d);
