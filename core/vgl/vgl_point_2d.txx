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

template <class Type>
bool vgl_point_2d<Type>::operator==(const vgl_point_2d &other) const
{
  bool result;	

  result=this==&other;
  if(!result)
    result=((this->x()==other.x()) && (this->y()==other.y()));
  return result;
		
}

template <class Type>
vgl_point_2d<Type> vgl_point_2d<Type>::operator+(const vgl_point_2d<Type>& that) const
{
   vgl_point_2d<Type> result((this->x()+that.x()),this->y()+that.y());
   return result;
}	

template <class Type>
vgl_point_2d<Type> vgl_point_2d<Type>::operator-(const vgl_point_2d<Type>& that) const
{
   vgl_point_2d<Type> result((this->x()-that.x()),this->y()-that.y());
   return result;
}	

template <class Type>
Type vgl_point_2d<Type>::distance(const vgl_point_2d<Type>& p) const
{	
  vgl_point_2d<Type> dif= (*this)-p;
  Type dist = sqrt(dif.x()*dif.x() + dif.y()*dif.y());
  return dist;
}	 