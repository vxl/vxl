//*****************************************************************************
// File name: test_vsol_tetrahedron.cxx
// Description: Test the vsol_tetrahedron class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2001/06/06| Peter Vanroose           |Creation
//*****************************************************************************

//:
//  \file

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vsol/vsol_tetrahedron.h>
#include <vsol/vsol_point_3d.h>

//-----------------------------------------------------------------------------
//: Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  vcl_cout<<"Constructor"<<vcl_endl;
  vsol_point_3d_sptr p=new vsol_point_3d(45,80,-96);
  vsol_point_3d_sptr q=new vsol_point_3d(35,-88,3);
  vsol_point_3d_sptr r=new vsol_point_3d(15,61,96);
  vsol_point_3d_sptr s=new vsol_point_3d(-45,8,0);
  // This tetrahedron has three right angles in corner point s.

  vsol_tetrahedron_sptr t=new vsol_tetrahedron(p,q,r,s);

  vcl_cout<<"p0()"<<vcl_endl;
  assert(*(t->p0())==*p);
  vcl_cout<<"p1()"<<vcl_endl;
  assert(*(t->p1())==*q);
  vcl_cout<<"p2()"<<vcl_endl;
  assert(*(t->p2())==*r);
  vcl_cout<<"p3()"<<vcl_endl;
  assert(*(t->p3())==*s);

  vcl_cout<<"volume() = "<<t->volume()<<vcl_endl;
  assert(t->volume()==390625);

  return 0;
}
