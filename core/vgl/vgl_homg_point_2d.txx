// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
//: Reperesents a homogeneous 2D point.


#include <vgl/vgl_homg_point_2d.h> 
#include <vgl/vgl_homg_line_2d.h> 

// Note that the given lines must be distinct!
template <class Type>
vgl_homg_point_2d<Type>::vgl_homg_point_2d (vgl_homg_line_2d<Type> const& l1,
                                            vgl_homg_line_2d<Type> const& l2)
{
  set(l1.b()*l2.c()-l1.c()*l2.b(),
      l1.c()*l2.a()-l1.a()*l2.c(),
      l1.a()*l2.b()-l1.b()*l2.a());
}

