//*****************************************************************************
// File name: test_cylindrical.cxx
// Description: Test the vcsl_cylindrical class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/29| Francois BERTEL          |Creation
// 1.1     |2002/11/13| Peter Vanroose           |converted to use TESTMAIN
//*****************************************************************************

#include <testlib/testlib_test.h>
#include <vcsl/vcsl_cylindrical.h>

static void test_cylindrical()
{
  vcsl_cylindrical_sptr p=new vcsl_cylindrical;
  TEST("test_cylindrical_sptr", bool(p), true);
}

TESTMAIN(test_cylindrical);
