//*****************************************************************************
// File name: test_cartesian_2d.cxx
// Description: Test the vcsl_cartesian_2d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| Francois BERTEL          |Creation
// 1.1     |2002/11/13| Peter Vanroose           |converted to use TESTMAIN
//*****************************************************************************

#include <testlib/testlib_test.h>
#include <vcsl/vcsl_cartesian_2d.h>

static void test_cartesian_2d()
{
  vcsl_cartesian_2d_sptr c=new vcsl_cartesian_2d;
  TEST("vcsl_cartesian_2d_sptr", bool(c), true);
}

TESTMAIN(test_cartesian_2d);
