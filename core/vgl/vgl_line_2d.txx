//*****************************************************************************
// File name: vgl_line_2d.h
// Description: Represents a euclidean 2d line
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/05| François BERTEL          | Several minor bugs fixed
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/02/15| Don HAMILTON, Peter TU   |Creation
//*****************************************************************************
#include <vgl/vgl_line_2d.h>

#include <vgl/vgl_point_2d.h>

//: find the distance of the line to the origin
template <class Type>
Type vgl_line_2d<Type>::distance_to_origin() const
{
  Type aa=a();
  Type bb=b();
  Type cc=c();
  Type norm=aa*aa+bb*bb;

  Type midpt_x=-aa*cc/norm; 
  Type midpt_y=-bb*cc/norm;
  
  return sqrt(midpt_x*midpt_x+midpt_y*midpt_y);
}

//: get two points on the line 
// find the closest point to the origin and then add 10 x direction and to y
// direction  
template <class Type>
void vgl_line_2d<Type>::get_two_points(vgl_point_2d<Type> &p1,
                                       vgl_point_2d<Type> &p2)
{
  Type aa=a();
  Type bb=b();
  Type cc=c();
  Type norm=aa*aa+bb*bb;
  
  Type midpt_x=-aa*cc/norm; 
  Type midpt_y=-bb*cc/norm;
  
  Type dx=dir_x();
  Type dy=dir_y();

  p1.set(midpt_x+10*dx,midpt_y+10*dy);
  p2.set(midpt_y-10*dx,midpt_y-10*dy);
}

