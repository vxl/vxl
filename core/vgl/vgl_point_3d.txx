#include <vgl/vgl_point_3d.h>

#include <vcl_cmath.h>

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

#define VGL_POINT_3D_INSTANTIATE(T) \
template class vgl_point_3d<T>; \
template ostream& operator<<(ostream&, const vgl_point_3d<T>&); \
template istream& operator>>(istream&, vgl_point_3d<T>&)

