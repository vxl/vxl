// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
//: Represents a homogeneous 2d line.


//--------------------------------------------------------------
//
// .NAME vgl_homg_line_2d
//
//--------------------------------------------------------------


#include <vgl/vgl_homg_line_2d.h> 
#include <vgl/vgl_homg_point_2d.h> 

// TODO
//  Type get_direction() const
//  {
//    // return the direction of the line
  
//    Type direction[3];
//    direction[0]=this->a();
//    direction[1]=this->b();
//    direction[2]=0;
//    return direction;
//  }
  

//  vcl_vector<Type> get_normal() const;
//  {
//    // todo

//    vcl_vector<Type,3> normal;
//    normal[0]= - this->b();
//    normal[1]=this->a();
//    normal[2]=0;
//    return normal;
//  }

//: get two points on the line.  These two points are normally the intersections
// with the Y axis and X axis, respectively.  When the line is parallel to one
// of these, the point with y=1 or x=1, resp. are taken.  When the line goes
// through the origin, the second point is (b, -a, 1).  Finally, when the line
// is the line at infinity, the returned points are (1,0,0) and (0,1,0).
// Thus, whenever possible, the returned points are not at infinity.
template <class Type>
void vgl_homg_line_2d<Type>::get_two_points(vgl_homg_point_2d<Type> &p1, vgl_homg_point_2d<Type> &p2)
{
  if (this->b() == 0) p1.set(-this->c(), this->a(), this->a());
  else                p1.set(0, -this->c(), this->b());
  if (this->a() == 0) p2.set(this->b(), -this->c(), this->b());
  else if ( c() == 0) p2.set(this->b(), -this->a(), 1);
  else                p2.set(-this->c(), 0, this->a());
}

// Note that the given points must be distinct!
template <class Type>
vgl_homg_line_2d<Type>::vgl_homg_line_2d (vgl_homg_point_2d<Type> const& l1,
                                          vgl_homg_point_2d<Type> const& l2)
{
  set(l1.y()*l2.w()-l1.w()*l2.y(),
      l1.w()*l2.x()-l1.x()*l2.w(),
      l1.x()*l2.y()-l1.y()*l2.x());
}

