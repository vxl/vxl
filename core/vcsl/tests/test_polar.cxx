//*****************************************************************************
// File name: test_polar.cxx
// Description: Test the vcsl_polar class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/29| Francois BERTEL          |Creation
// 1.1     |2002/11/13| Peter Vanroose           |converted to use TESTMAIN
//*****************************************************************************

#include <testlib/testlib_test.h>
#include <vcsl/vcsl_polar.h>

static void test_polar()
{
  vcsl_polar_sptr p=new vcsl_polar;
  TEST("test_polar_sptr", bool(p), true);
}

TESTMAIN(test_polar);
