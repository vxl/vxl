//*****************************************************************************
// File name: test_vsol_rectangle_2d.cxx
// Description: Test the vsol_rectangle_2d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/08| François BERTEL          |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vsol/vsol_rectangle_2d.h>
#include <vsol/vsol_point_2d.h>

//-----------------------------------------------------------------------------
// -- Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int result=0;

  vcl_cout<<"Constructor"<<vcl_endl;
  vsol_point_2d_sptr p=new vsol_point_2d(0,0);
  vsol_point_2d_sptr q=new vsol_point_2d(1,0);
  vsol_point_2d_sptr r=new vsol_point_2d(1,1);

  vsol_rectangle_2d_sptr t=new vsol_rectangle_2d(p,q,r);

  vcl_cout<<"p0()"<<vcl_endl;
  assert(*(t->p0())==*p);
  vcl_cout<<"p1()"<<vcl_endl;
  assert(*(t->p1())==*q);
  vcl_cout<<"p2()"<<vcl_endl;
  assert(*(t->p2())==*r);

  vcl_cout<<"area()"<<vcl_endl;
  vcl_cout<<t->area()<<vcl_endl;
  assert(t->area()==1);

  return result;
}
