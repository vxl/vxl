#include <vgl/vgl_point_3d.h>

#include <vcl/vcl_cmath.h>

template <class Type>
bool vgl_point_3d<Type>::operator==(const vgl_point_3d &other) const
{
  bool result;	

  result=this==&other;
  if(!result)
    result=((this->x()==other.x()) && (this->y()==other.y()) && (this->z()==other.z()));
  return result;
		
}



template <class Type>
vgl_point_3d<Type> vgl_point_3d<Type>::operator+(const vgl_point_3d<Type>& that) const
{
   vgl_point_3d<Type> result((this->x()+that.x()),this->y()+that.y(),
		this->z()+that.z());
   return result;
}	

template <class Type>
vgl_point_3d<Type> vgl_point_3d<Type>::operator-(const vgl_point_3d<Type>& that) const
{
   vgl_point_3d<Type> result((this->x()-that.x()),this->y()-that.y(),this->z()-that.z());
   return result;
}	

template <class Type>
Type vgl_point_3d<Type>::distance(const vgl_point_3d<Type>& p) const
{	
  vgl_point_3d<Type> dif= (*this)-p;
  Type dist = /*vcl_*/sqrt(dif.x()*dif.x() + dif.y()*dif.y() + dif.z()*dif.z());
  return dist;
}
