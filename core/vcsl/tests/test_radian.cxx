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

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cassert.h>
#include <vcsl/vcsl_radian.h>

//-----------------------------------------------------------------------------
// -- Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int result;

  result=0;

  vcsl_radian_ref r;
  r=vcsl_radian::instance();

  return result;
}
