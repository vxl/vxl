//*****************************************************************************
// File name: test_spherical.cxx
// Description: Test the vcsl_spherical class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/29| Francois BERTEL          |Creation
// 1.1     |2002/11/13| Peter Vanroose           |converted to use TESTMAIN
//*****************************************************************************

#include <testlib/testlib_test.h>
#include <vcsl/vcsl_spherical.h>

static void test_spherical()
{
  vcsl_spherical_sptr p=new vcsl_spherical;
  TEST("test_spherical_sptr", bool(p), true);
}

TESTMAIN(test_spherical);
