//*****************************************************************************
// File name: test_vsol_polygon_2d.cxx
// Description: Test the vsol_polygon_2d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/09| François BERTEL          |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>

//-----------------------------------------------------------------------------
// -- Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int result=0;

  vsol_point_2d_ref p;
  vcl_vector<vsol_point_2d_ref> *vertices;
  vsol_polygon_2d_ref poly;
  vsol_polygon_2d_ref poly2;

  vertices=new vcl_vector<vsol_point_2d_ref>(5);

  p=new vsol_point_2d(0,0);
  (*vertices)[0]=p;
  p=new vsol_point_2d(3,1);
  (*vertices)[1]=p;
  p=new vsol_point_2d(5,3);
  (*vertices)[2]=p;
  p=new vsol_point_2d(3,6);
  (*vertices)[3]=p;
  p=new vsol_point_2d(1,4);
  (*vertices)[4]=p;

  vcl_cout<<"Constructor"<<vcl_endl;
  poly=new vsol_polygon_2d(*vertices);
  delete vertices;

  vcl_cout<<"size"<<vcl_endl;
  assert(poly->size()==5);

  vcl_cout<<"vertex"<<vcl_endl;
  p=poly->vertex(0);
  assert(p->x()==0);
  assert(p->y()==0);

  p=poly->vertex(1);
  assert(p->x()==3);
  assert(p->y()==1);

  p=poly->vertex(2);
  assert(p->x()==5);
  assert(p->y()==3);

  p=poly->vertex(3);
  assert(p->x()==3);
  assert(p->y()==6);
  
  p=poly->vertex(4);
  assert(p->x()==1);
  assert(p->y()==4);

  vcl_cout<<"is_convex"<<vcl_endl;
  assert(poly->is_convex());

  vcl_cout<<"Copy constructor"<<vcl_endl;
  poly2=new vsol_polygon_2d(*poly);

  vcl_cout<<"=="<<vcl_endl;
  assert(*poly2==*poly);

  return result;
}
