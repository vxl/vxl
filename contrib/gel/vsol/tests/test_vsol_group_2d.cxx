//*****************************************************************************
// File name: test_vsol_group_2d.cxx
// Description: Test the vsol_group_2d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/04/26| François BERTEL          |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vsol/vsol_group_2d.h>
#include <vsol/vsol_point_2d.h>

//-----------------------------------------------------------------------------
// -- Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int result=0;
  vsol_group_2d_ref group1=new vsol_group_2d;
  vsol_point_2d_ref p=new vsol_point_2d(10,4);
  vsol_group_2d_ref group2;

  vcl_cout<<"size()"<<vcl_endl;
  vcl_cout<<group1->size()<<vcl_endl;
  assert(group1->size()==0);

  vcl_cout<<"deep_size()"<<vcl_endl;
  vcl_cout<<group1->deep_size()<<vcl_endl;
  assert(group1->deep_size()==0);

  group1->add_object(p.ptr());

  vcl_cout<<"size()"<<vcl_endl;
  vcl_cout<<group1->size()<<vcl_endl;
  assert(group1->size()==1);

  vcl_cout<<"deep_size()"<<vcl_endl;
  vcl_cout<<group1->deep_size()<<vcl_endl;
  assert(group1->deep_size()==1);

  group2=new vsol_group_2d;

  group1->add_object(group2.ptr());

  vcl_cout<<"size()"<<vcl_endl;
  vcl_cout<<group1->size()<<vcl_endl;
  assert(group1->size()==2);

  vcl_cout<<"deep_size()"<<vcl_endl;
  vcl_cout<<group1->deep_size()<<vcl_endl;
  assert(group1->deep_size()==1);

  vcl_cout<<"object()"<<vcl_endl;
  assert(*(group1->object(0))==*p);

  vcl_cout<<"remove_object()"<<vcl_endl;
  group1->remove_object(0);
  assert(group1->size()==1);
  assert(group1->object(0)->cast_to_group()!=0); // It is group2 now
  return result;
}
