//*****************************************************************************
// File name: test_vsol_point_3d.cxx
// Description: Test the vsol_point_3d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/04| François BERTEL          |Creation
//*****************************************************************************

//:
//  \file

#include <vcl_iostream.h>
//#include <vcl_rel_ops.h>
#include <vsol/vsol_point_3d.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }

//-----------------------------------------------------------------------------
//: Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int success=0, failures=0;

  // Constructor from coordinates
  vcl_cout<<"Constructor from coordinates"<<vcl_endl;
  vsol_point_3d_sptr p=new vsol_point_3d(10,4,1);

  vcl_cout<<"x(), y() and z()"<<vcl_endl;
  vcl_cout<<"x="<<p->x()<<", y="<<p->y()<<", z="<<p->z()<<vcl_endl;
  Assert(p->x()==10);
  Assert(p->y()==4);
  Assert(p->z()==1);

  vcl_cout<<"Copy constructor"<<vcl_endl;
  vsol_point_3d_sptr q=new vsol_point_3d(*p);

  vcl_cout<<"== operator"<<vcl_endl;
  Assert((*p)==(*q));

  vcl_cout<<"set_x()"<<vcl_endl;
  q->set_x(3);
  Assert(q->x()==3);

  vcl_cout<<"set_y()"<<vcl_endl;
  q->set_y(5);
  Assert(q->y()==5);

  vcl_cout<<"set_z()"<<vcl_endl;
  q->set_z(12);
  Assert(q->z()==12);

  vcl_cout<<"!= operator"<<vcl_endl;
  Assert((*p)!=(*q));

  vcl_cout<<"distance()="<<p->distance(q)<<vcl_endl;

  vsol_point_3d_sptr r=p->middle(*q);

  vcl_cout<<"middle()="<<r->x()<<','<<r->y()<<vcl_endl;

  Assert(r->x()==(p->x()+q->x())/2);
  Assert(r->y()==(p->y()+q->y())/2);
  Assert(r->z()==(p->z()+q->z())/2);

  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
