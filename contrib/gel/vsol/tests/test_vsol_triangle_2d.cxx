//*****************************************************************************
// File name: test_vsol_triangle_2d.cxx
// Description: Test the vsol_triangle_2d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/02| François BERTEL          |Creation
//*****************************************************************************

//:
//  \file

#include <vcl_iostream.h>
#include <vsol/vsol_triangle_2d.h>
#include <vsol/vsol_triangle_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

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
  vsol_point_2d_sptr p=new vsol_point_2d(0,0);
  vsol_point_2d_sptr q=new vsol_point_2d(1,0);
  vsol_point_2d_sptr r=new vsol_point_2d(0,1);

  vsol_triangle_2d_sptr t=new vsol_triangle_2d(p,q,r);
  vsol_triangle_2d_sptr t2;

  vcl_cout<<"p0()"<<vcl_endl;
  Assert(*(t->p0())==*p);
  vcl_cout<<"p1()"<<vcl_endl;
  Assert(*(t->p1())==*q);
  vcl_cout<<"p2()"<<vcl_endl;
  Assert(*(t->p2())==*r);

  vcl_cout<<"area()"<<vcl_endl;
  vcl_cout<<t->area()<<vcl_endl;
  Assert(t->area()==0.5);

  vcl_cout<<"Copy constructor"<<vcl_endl;
  t2=new vsol_triangle_2d(*t);

  vcl_cout<<"=="<<vcl_endl;
  Assert(*t2==*t);

  r=new vsol_point_2d(10,1);
  vcl_cout<<"set_p0"<<vcl_endl;
  t2->set_p0(r);
  vcl_cout<<"!="<<vcl_endl;
  Assert(*t2!=*t);

  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
