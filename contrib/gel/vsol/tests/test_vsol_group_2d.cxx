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

//:
//  \file

#include <vcl_iostream.h>
#include <vsol/vsol_group_2d.h>
#include <vsol/vsol_point_2d.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }

//-----------------------------------------------------------------------------
//: Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int success=0, failures=0;

  vsol_group_2d_sptr group1=new vsol_group_2d;
  vsol_point_2d_sptr p=new vsol_point_2d(10,4);
  vsol_group_2d_sptr group2;

  vcl_cout<<"size()"<<vcl_endl;
  vcl_cout<<group1->size()<<vcl_endl;
  Assert(group1->size()==0);

  vcl_cout<<"deep_size()"<<vcl_endl;
  vcl_cout<<group1->deep_size()<<vcl_endl;
  Assert(group1->deep_size()==0);

  group1->add_object(p.ptr());

  vcl_cout<<"size()"<<vcl_endl;
  vcl_cout<<group1->size()<<vcl_endl;
  Assert(group1->size()==1);

  vcl_cout<<"deep_size()"<<vcl_endl;
  vcl_cout<<group1->deep_size()<<vcl_endl;
  Assert(group1->deep_size()==1);

  group2=new vsol_group_2d;

  group1->add_object(group2.ptr());

  vcl_cout<<"size()"<<vcl_endl;
  vcl_cout<<group1->size()<<vcl_endl;
  Assert(group1->size()==2);

  vcl_cout<<"deep_size()"<<vcl_endl;
  vcl_cout<<group1->deep_size()<<vcl_endl;
  Assert(group1->deep_size()==1);

  vcl_cout<<"object()"<<vcl_endl;
  Assert(*(group1->object(0))==*p);

  vcl_cout<<"remove_object()"<<vcl_endl;
  group1->remove_object(0);
  Assert(group1->size()==1);
  Assert(group1->object(0)->cast_to_group()!=0); // It is group2 now

  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
