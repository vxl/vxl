//*****************************************************************************
// File name: test_cartesian_3d.cxx
// Description: Test the vcsl_cartesian_3d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| Francois BERTEL          |Creation
// 1.1     |2002/11/13| Peter Vanroose           |converted to use TESTMAIN
//*****************************************************************************

#include <testlib/testlib_test.h>
#include <vcsl/vcsl_cartesian_3d.h>

static void test_cartesian_3d()
{
  vcsl_cartesian_3d_sptr c=new vcsl_cartesian_3d;
  TEST("test_cartesian_3d_sptr", bool(c), true);
}

TESTMAIN(test_cartesian_3d);
