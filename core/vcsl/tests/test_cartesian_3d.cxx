//*****************************************************************************
// File name: test_cartesian_3d.cxx
// Description: Test the vcsl_cartesian_3d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| François BERTEL          |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcsl/vcsl_cartesian_3d.h>

//-----------------------------------------------------------------------------
//: Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int result;

  result=0;

  vcsl_cartesian_3d_sptr c;
  c=new vcsl_cartesian_3d;

  return result;
}
