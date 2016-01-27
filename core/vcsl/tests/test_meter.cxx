//*****************************************************************************
// File name: test_meter.cxx
// Description: Test the vcsl_meter class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/16| Francois BERTEL          |Creation
// 1.1     |2002/11/13| Peter Vanroose           |converted to use TESTMAIN
//*****************************************************************************

#include <testlib/testlib_test.h>
#include <vcsl/vcsl_meter.h>

static void test_meter()
{
  vcsl_meter_sptr m=vcsl_meter::instance();
  TEST("test_meter_sptr", bool(m), true);
}

TESTMAIN(test_meter);
