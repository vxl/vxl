//*****************************************************************************
// File name: test_vsol_group_3d.cxx
// Description: Test the vsol_group_3d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/09| François BERTEL          |Creation
//*****************************************************************************

//:
//  \file

#include <vcl_iostream.h>
#include <vsol/vsol_group_3d.h>
#include <vsol/vsol_group_3d_sptr.h>
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

  vsol_group_3d_sptr group1=new vsol_group_3d;
  vsol_point_3d_sptr p=new vsol_point_3d(10,4,5);
  vsol_group_3d_sptr group2;

  vcl_cout<<"group1->size() = "<<group1->size()<<vcl_endl;
  Assert(group1->size()==0);

  vcl_cout<<"group1->deep_size() = "<<group1->deep_size()<<vcl_endl;
  Assert(group1->deep_size()==0);

  group1->add_object(p.ptr());

  vcl_cout<<"group1->size() = "<<group1->size()<<vcl_endl;
  Assert(group1->size()==1);

  vcl_cout<<"group1->deep_size() = "<<group1->deep_size()<<vcl_endl;
  Assert(group1->deep_size()==1);

  vcl_cout<<"add_object()"<<vcl_endl;
  group2=new vsol_group_3d;
  group1->add_object(group2.ptr());

  vcl_cout<<"group1->size() = "<<group1->size()<<vcl_endl;
  Assert(group1->size()==2);

  vcl_cout<<"group1->deep_size() = "<<group1->deep_size()<<vcl_endl;
  Assert(group1->deep_size()==1);

  Assert(*(group1->object(0))==*p);

  vcl_cout<<"remove_object()"<<vcl_endl;
  group1->remove_object(0);
  vcl_cout<<"group1->size() = "<<group1->size()<<vcl_endl;
  Assert(group1->size()==1);

  Assert(group1->object(0)->cast_to_group()!=0); // It is group2 now

  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
