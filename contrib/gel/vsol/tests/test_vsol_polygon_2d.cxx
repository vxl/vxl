//*****************************************************************************
// File name: test_vsol_polygon_2d.cxx
// Description: Test the vsol_polygon_2d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/09| François BERTEL          |Creation
// 1.1     |2001/07/03| Peter Vanroose           |Thorough check of is_convex
//*****************************************************************************

//:
//  \file

#include <vcl_iostream.h>
#include <vcl_vector.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }

//-----------------------------------------------------------------------------
//: Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int success=0, failures=0;

  vcl_vector<vsol_point_2d_sptr> vertices(5);
  vertices[0]=new vsol_point_2d(0,0);
  vertices[1]=new vsol_point_2d(3,1);
  vertices[2]=new vsol_point_2d(5,3);
  vertices[3]=new vsol_point_2d(3,6);
  vertices[4]=new vsol_point_2d(1,4);

  vcl_cout<<"Constructor"<<vcl_endl;
  vsol_polygon_2d_sptr poly=new vsol_polygon_2d(vertices);

  vcl_cout<<"size"<<vcl_endl;
  Assert(poly->size()==5);

  vcl_cout<<"vertex"<<vcl_endl;

  vsol_point_2d_sptr
  p=poly->vertex(0);
  Assert(p->x()==0);
  Assert(p->y()==0);

  p=poly->vertex(1);
  Assert(p->x()==3);
  Assert(p->y()==1);

  p=poly->vertex(2);
  Assert(p->x()==5);
  Assert(p->y()==3);

  p=poly->vertex(3);
  Assert(p->x()==3);
  Assert(p->y()==6);

  p=poly->vertex(4);
  Assert(p->x()==1);
  Assert(p->y()==4);

  vcl_cout<<"is_convex"<<vcl_endl;
  Assert(poly->is_convex());

  vcl_cout<<"Copy constructor"<<vcl_endl;
  vsol_polygon_2d_sptr poly2=new vsol_polygon_2d(*poly);

  vcl_cout<<"=="<<vcl_endl;
  Assert(*poly2==*poly);

  // An example of a non-convex polygon with "non-convexity" between
  // start and end vertex.
  // An earlier implementation of is_convex() will fail here.
  vertices[0]=new vsol_point_2d(1,1);
  vertices[1]=new vsol_point_2d(2,1);
  vertices[2]=new vsol_point_2d(1,0);
  vertices[3]=new vsol_point_2d(0,1);
  vertices[4]=new vsol_point_2d(1,2);
  poly=new vsol_polygon_2d(vertices);

  vcl_cout<<"!is_convex"<<vcl_endl;
  Assert(!poly->is_convex());

  // An example of a non-convex polygon with two consecutive parallel
  // edges.  An earlier implementation of is_convex() will fail here.
  vertices[0]=new vsol_point_2d(0,2);
  vertices[1]=new vsol_point_2d(0,1);
  vertices[2]=new vsol_point_2d(1,1);
  vertices[3]=new vsol_point_2d(2,1);
  vertices[4]=new vsol_point_2d(2,0);
  poly=new vsol_polygon_2d(vertices);

  vcl_cout<<"!is_convex"<<vcl_endl;
  Assert(!poly->is_convex());

  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
