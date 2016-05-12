//*****************************************************************************
// File name: test_degree.cxx
// Description: Test the vcsl_degree class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| Francois BERTEL          |Creation
// 1.1     |2002/11/13| Peter Vanroose           |converted to use TESTMAIN
//*****************************************************************************

#include <testlib/testlib_test.h>
#include <vcsl/vcsl_degree.h>

static void test_degree()
{
  vcsl_degree_sptr r=vcsl_degree::instance();
  TEST("test_degree_sptr", bool(r), true);
}

TESTMAIN(test_degree);
