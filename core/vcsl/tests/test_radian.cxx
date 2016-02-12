//*****************************************************************************
// File name: test_radian.cxx
// Description: Test the vcsl_radian class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| Francois BERTEL          |Creation
// 1.1     |2002/11/13| Peter Vanroose           |converted to use TESTMAIN
//*****************************************************************************

#include <testlib/testlib_test.h>
#include <vcsl/vcsl_radian.h>

static void test_radian()
{
  vcsl_radian_sptr r=vcsl_radian::instance();
  TEST("test_radian_sptr", bool(r), true);
}

TESTMAIN(test_radian);
