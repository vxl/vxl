//*****************************************************************************
// File name: test_cylindrical.cxx
// Description: Test the vcsl_cylindrical class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/29| François BERTEL          |Creation
//*****************************************************************************

//:
// \file

#include <vcsl/vcsl_cylindrical.h>

//-----------------------------------------------------------------------------
//: Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  vcsl_cylindrical_sptr p=new vcsl_cylindrical;

  return p?0:1;
}
