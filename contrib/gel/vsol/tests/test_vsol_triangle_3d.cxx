//*****************************************************************************
// File name: test_vsol_triangle_3d.cxx
// Description: Test the vsol_triangle_3d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/08| François BERTEL          |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vsol/vsol_triangle_3d.h>
#include <vsol/vsol_point_3d.h>

//-----------------------------------------------------------------------------
// -- Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int result=0;

  vcl_cout<<"Constructor"<<vcl_endl;
  vsol_point_3d_ref p=new vsol_point_3d(0,0,10);
  vsol_point_3d_ref q=new vsol_point_3d(1,0,10);
  vsol_point_3d_ref r=new vsol_point_3d(0,1,10); 

  vsol_triangle_3d_ref t=new vsol_triangle_3d(p,q,r);

  vcl_cout<<"p0()"<<vcl_endl;
  assert(*(t->p0())==*p);
  vcl_cout<<"p1()"<<vcl_endl;
  assert(*(t->p1())==*q);
  vcl_cout<<"p2()"<<vcl_endl;
  assert(*(t->p2())==*r);

  vcl_cout<<"area()"<<vcl_endl;
  vcl_cout<<t->area()<<vcl_endl;
  assert(t->area()==0.5);
 
  return result;
}
