//*****************************************************************************
// File name: test_vsol_tetrahedron.cxx
// Description: Test the vsol_tetrahedron class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2001/06/06| Peter Vanroose           |Creation
// 1.1     |2003/01/08| Peter Vanroose           |Now using testlib macros
//*****************************************************************************
#include <testlib/testlib_test.h>

#include <vsol/vsol_tetrahedron.h>
#include <vsol/vsol_tetrahedron_sptr.h>
#include <vsol/vsol_point_3d.h>

void test_vsol_tetrahedron()
{
  vsol_point_3d_sptr p=new vsol_point_3d(45,80,-96);
  vsol_point_3d_sptr q=new vsol_point_3d(35,-88,3);
  vsol_point_3d_sptr r=new vsol_point_3d(15,61,96);
  vsol_point_3d_sptr s=new vsol_point_3d(-45,8,0);
  // This tetrahedron has three right angles in corner point s.

  vsol_tetrahedron_sptr t=new vsol_tetrahedron(p,q,r,s);
  TEST("Constructor", !t, false);

  TEST("vsol_tetrahedron::p0()", *(t->p0()), *p);
  TEST("vsol_tetrahedron::p1()", *(t->p1()), *q);
  TEST("vsol_tetrahedron::p2()", *(t->p2()), *r);
  TEST("vsol_tetrahedron::p3()", *(t->p3()), *s);

  TEST_NEAR("vsol_tetrahedron::volume()", t->volume(), 390625.0, 1e-10);
}

TESTMAIN(test_vsol_tetrahedron);
