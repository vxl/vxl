//*****************************************************************************
// File name: test_meter.cxx
// Description: Test the vcsl_meter class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/16| François BERTEL          |Creation
//*****************************************************************************

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cassert.h>
#include <vcsl/vcsl_meter.h>

//-----------------------------------------------------------------------------
// Name: main
// Task: Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int result;

  result=0;

  vcsl_meter_ref m;
  m=vcsl_meter::instance();


  return result;
}
