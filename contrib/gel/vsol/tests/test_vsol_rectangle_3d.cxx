//*****************************************************************************
// File name: test_vsol_rectangle_3d.cxx
// Description: Test the vsol_rectangle_3d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/08| François BERTEL          |Creation
//*****************************************************************************

//:
//  \file

#include <vcl_iostream.h>
#include <vsol/vsol_rectangle_3d.h>
#include <vsol/vsol_rectangle_3d_sptr.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }

//-----------------------------------------------------------------------------
//: Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int success=0, failures=0;

  vcl_cout<<"Constructor"<<vcl_endl;
  vsol_point_3d_sptr p=new vsol_point_3d(0,0,10);
  vcl_cout<<"p created"<<vcl_endl;
  vsol_point_3d_sptr q=new vsol_point_3d(1,0,10);
  vcl_cout<<"q created"<<vcl_endl;
  vsol_point_3d_sptr r=new vsol_point_3d(1,1,10);
  vcl_cout<<"r created"<<vcl_endl;
  vsol_rectangle_3d_sptr t=new vsol_rectangle_3d(p,q,r);
  vcl_cout<<"t created"<<vcl_endl;
  vcl_cout<<"p0()"<<vcl_endl;
  Assert(*(t->p0())==*p);
  vcl_cout<<"p1()"<<vcl_endl;
  Assert(*(t->p1())==*q);
  vcl_cout<<"p2()"<<vcl_endl;
  Assert(*(t->p2())==*r);

  vcl_cout<<"area()"<<vcl_endl;
  vcl_cout<<t->area()<<vcl_endl;
  Assert(t->area()==1);

  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
