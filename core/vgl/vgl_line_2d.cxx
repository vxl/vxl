// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
//: Represents a euclidean 2d line.


//--------------------------------------------------------------
//
// Class : vgl_line_2d
//
//--------------------------------------------------------------


#include <vgl/vgl_line_2d.h> 


//: find the distance of the line to the origin
template <class Type>
Type vgl_line_2d<Type>::dist_orign() const
{
  Type a = a(), b = b(), c = c();
  Type norm = a*a + b*b;

  Type midpt_x = -a*c/norm; 
  Type midpt_y = -b*c/norm;
  
  return sqrt(midpt_x*midpt_x + midpt_y*midpt_y);
}


//: get two points on the line 
// find the closest point to the origin and then add 10 x direction and to y direction  
template <class Type>
void vgl_line_2d<Type>::get_two_points(vgl_point_2d<Type> &p1, vgl_point_2d<Type> &p2)
{
  Type a = a(), b = b(), c = c();
  Type norm = a*a + b*b;
  
  Type midpt_x = -a*c/norm; 
  Type midpt_y = -b*c/norm;
  
  Type dx = dirx();
  Type dy = diry();

  p1.set(midpt_x + 10*dx(),midpt_y + 10*dy);
  p2.set(midpt_y - 10*dx(),midpt_y - 10*dy);
}

