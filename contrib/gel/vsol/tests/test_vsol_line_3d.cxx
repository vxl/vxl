//*****************************************************************************
// File name: test_vsol_line_3d.cxx
// Description: Test the vsol_line_3d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/08| François BERTEL          |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vsol/vsol_line_3d.h>
#include <vsol/vsol_point_3d.h>
//-----------------------------------------------------------------------------
// -- Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int result=0;

  vcl_cout<<"Constructor from extremities"<<vcl_endl;
  vsol_point_3d_ref p=new vsol_point_3d(10,4,1);
  vsol_point_3d_ref q=new vsol_point_3d(5,1,-23);
  vsol_line_3d_ref a=new vsol_line_3d(p,q);

  assert(*(a->p0())==*p);
  assert(*(a->p1())==*q);

  vcl_cout<<"Copy constructor"<<vcl_endl;
  vsol_line_3d_ref b=new vsol_line_3d(*a);

  vcl_cout<<"== operator"<<vcl_endl;
  assert(*a==*b);

  vcl_cout<<"Constructor from direction and middle point"<<vcl_endl;
  vnl_vector_fixed<double,3> *v=new vnl_vector_fixed<double,3>();
  (*v)[0]=10;
  (*v)[1]=-5;
  (*v)[2]=2;

  vsol_line_3d_ref c=new vsol_line_3d(*v,p);

  vcl_cout<<"middle()"<<vcl_endl;
  assert(*(c->middle())==*p);
  vcl_cout<<"direction()"<<vcl_endl;
  assert(*(c->direction())==*v);

  vcl_cout<<"length()"<<vcl_endl;
  vcl_cout<<c->length()<<vcl_endl;
  
  vcl_cout<<"set_length()"<<vcl_endl;
  c->set_length(100);
  assert(c->length()==100);
  // set_length() doesn't change the middle point
  assert(*(c->middle())==*p);

  vcl_cout<<"set_p0()"<<vcl_endl;
  c->set_p0(p);
  assert(*(c->p0())==*p);

  vcl_cout<<"set_p1()"<<vcl_endl;
  c->set_p1(q);
  assert(*(c->p1())==*q);

  delete v;
  return result;
}
