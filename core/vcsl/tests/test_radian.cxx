//*****************************************************************************
// File name: test_radian.cxx
// Description: Test the vcsl_radian class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| François BERTEL          |Creation
//*****************************************************************************

//:
// \file

#include <vcl_iostream.h>
#include <vcsl/vcsl_radian.h>

//-----------------------------------------------------------------------------
//: Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  vcsl_radian_sptr r=vcsl_radian::instance();

  return 0;
}
