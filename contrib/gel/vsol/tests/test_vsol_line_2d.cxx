//*****************************************************************************
// File name: test_vsol_line_2d.cxx
// Description: Test the vsol_line_2d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/02| François BERTEL          |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
//-----------------------------------------------------------------------------
//: Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int result=0;

  vcl_cout<<"Constructor from extremities"<<vcl_endl;
  vsol_point_2d_sptr p=new vsol_point_2d(10,4);
  vsol_point_2d_sptr q=new vsol_point_2d(5,1);
  vsol_line_2d_sptr a=new vsol_line_2d(p,q);

  assert(*(a->p0())==*p);
  assert(*(a->p1())==*q);

  vcl_cout<<"Copy constructor"<<vcl_endl;
  vsol_line_2d_sptr b=new vsol_line_2d(*a);

  vcl_cout<<"== operator"<<vcl_endl;
  assert(*a==*b);

  vcl_cout<<"Constructor from direction and middle point"<<vcl_endl;
  vnl_vector_fixed<double,2> *v=new vnl_vector_fixed<double,2>();
  (*v)[0]=10;
  (*v)[1]=-5;

  vsol_line_2d_sptr c=new vsol_line_2d(*v,p);

  vcl_cout<<"middle()"<<vcl_endl;
  assert(*(c->middle())==*p);
  vcl_cout<<"direction()"<<vcl_endl;
  assert(*(c->direction())==*v);

  vcl_cout<<"length()"<<vcl_endl;
  vcl_cout<<c->length()<<vcl_endl;

  vcl_cout<<"set_length()"<<vcl_endl;
  c->set_length(100);

  assert(*(c->middle())==*p); // set_length() doesn't change the middle point

  vcl_cout<<"set_p0()"<<vcl_endl;
  c->set_p0(p);
  assert(*(c->p0())==*p);

  vcl_cout<<"set_p1()"<<vcl_endl;
  c->set_p1(q);
  assert(*(c->p1())==*q);

  delete v;
  return result;
}
