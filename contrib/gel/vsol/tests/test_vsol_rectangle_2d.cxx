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
// 2.0     |2004/05/16| Joseph Mundy             |Reworked rectangle interface
//*****************************************************************************
#include <testlib/testlib_test.h>
//:
// \file
#include <vnl/vnl_math.h>
#include <vcl_cmath.h> //for fabs
#include <vsol/vsol_rectangle_2d.h>
#include <vsol/vsol_rectangle_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
bool near_eq(double x, double y){return vcl_fabs(x-y)<1e-06;}
bool near_eq(vsol_point_2d_sptr const& a, vsol_point_2d_sptr const& b)
{
  bool eq = near_eq(a->x(), b->x())&&near_eq(a->y(), b->y());
  return eq;
}

void test_vsol_rectangle_2d()
{
  //test normal constructor
  vcl_cout << "Test vsol_rectangle_2d(p,q,r,s)" <<'\n';
  vsol_point_2d_sptr p=new vsol_point_2d(0,0);
  vsol_point_2d_sptr q=new vsol_point_2d(1,0);
  vsol_point_2d_sptr r=new vsol_point_2d(1,1);
  vsol_point_2d_sptr s=new vsol_point_2d(0,1);
  vsol_rectangle_2d_sptr t=new vsol_rectangle_2d(p,q,r,s);

  TEST("vsol_rectangle_2d::p0()", *(t->p0()), *p);
  TEST("vsol_rectangle_2d::p1()", *(t->p1()), *q);
  TEST("vsol_rectangle_2d::p2()", *(t->p2()), *r);
  TEST("vsol_rectangle_2d::p3()", *(t->p3()), *s);

  TEST("vsol_rectangle_2d::area()", t->area(), 1);

  //test three-point constructor
  vcl_cout << "Test vsol_rectangle_2d(center, half_width_vector, half_height_vector)" <<'\n';
  vsol_point_2d_sptr center = new vsol_point_2d(0.5,0.5);
  vsol_point_2d_sptr half_width_vector =  new vsol_point_2d(0.5,0);
  vsol_point_2d_sptr half_height_vector = new vsol_point_2d(0.0,0.5);
  vsol_rectangle_2d_sptr t1=
    new vsol_rectangle_2d(center, half_width_vector, half_height_vector);

  TEST("vsol_rectangle_2d::p0()", *(t1->p0()), *p);
  TEST("vsol_rectangle_2d::p1()", *(t1->p1()), *q);
  TEST("vsol_rectangle_2d::p2()", *(t1->p2()), *r);
  TEST("vsol_rectangle_2d::p3()", *(t1->p3()), *s);

  //test center, width, height, angle constructor
  vcl_cout << "Test vsol_rectangle_2d(center, half_width, half_height, angle, true)" <<'\n';
  double half_width = 0.5, half_height = 0.5, angle = 45.0;
  vsol_rectangle_2d_sptr t2=
    new vsol_rectangle_2d(center, half_width, half_height, angle, true);
  // using vnl_math::sqrt1_2 here causes a link error on Solaris gcc 3.0.4 with static libraries.
  double m = vcl_sqrt(0.5);
  vsol_point_2d_sptr q0 = new vsol_point_2d(0.5,0.5-m);
  vsol_point_2d_sptr q1 =new vsol_point_2d(0.5+m,0.5);
  vsol_point_2d_sptr q2 =new vsol_point_2d(0.5,0.5+m);
  vsol_point_2d_sptr q3 =new vsol_point_2d(0.5-m,0.5);
  
  TEST("vsol_rectangle_2d::p0()", near_eq(q0, t2->p0()), true);
  TEST("vsol_rectangle_2d::p1()", near_eq(q1, t2->p1()), true);
  TEST("vsol_rectangle_2d::p2()", near_eq(q2, t2->p2()), true);
  TEST("vsol_rectangle_2d::p3()", near_eq(q3, t2->p3()), true);
}

TESTMAIN(test_vsol_rectangle_2d);
