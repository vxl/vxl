//--------------------------------------------------------------
//
// Class : vgl_point_2d
//
//--------------------------------------------------------------

#include <vgl/vgl_point_2d.h> 
#include <vgl/vgl_line_2d.h> 
#include <vgl/vgl_homg_point_2d.h> 
#include <vgl/vgl_homg_line_2d.h> 

template <class Type>
vgl_point_2d<Type>::vgl_point_2d(vgl_line_2d<Type> const& l1,
                                 vgl_line_2d<Type> const& l2)
{
  vgl_homg_line_2d<Type> h1(l1.a(), l1.b(), l1.c());
  vgl_homg_line_2d<Type> h2(l2.a(), l2.b(), l2.c());
  vgl_homg_point_2d<Type> p(h1, h2); // do homogeneous intersection
  set(p.x()/p.w(), p.y()/p.w()); // could be infinite!
}
