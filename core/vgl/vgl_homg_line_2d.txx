// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
//: Represents a homogeneous 2d line.


//--------------------------------------------------------------
//
// Class : vgl_homg_line_2d
//
//--------------------------------------------------------------


#include <vgl/vgl_homg_line_2d.h> 

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

 // find the distance of the line to the origin
template <class Type>
Type vgl_homg_line_2d<Type>::dist_orign() const
{
  // TODO
  return 0;
}


// get two points on the line 
template <class Type>
void vgl_homg_line_2d<Type>::get_two_points(vgl_homg_point_2d<Type> &p1, vgl_homg_point_2d<Type> &p2)
{
  // todo 
  // needs svd ie:
  // IUE_matrix<double> M(get_vector().data_block(), 1, 3);
  // SVD svd(o);
  // p1->set(svd.V(0,1), svd.V(1,1), svd.V(2,1));
  // p2->set(svd.V(0,2), svd.V(1,2), svd.V(2,2));
}

