// This is vxl/vgl/vgl_homg_point_3d.txx

#include <vgl/vgl_homg_point_3d.h>

//---------------------------------------------------------------------------
//: Default constructor with (0,0,0,1)
template <class Type>
vgl_homg_point_3d<Type>::vgl_homg_point_3d(void)
{
  data_[0]=0;
  data_[1]=0;
  data_[2]=0;
  data_[3]=1;
}


template <class Type>
bool vgl_homg_point_3d<Type>::operator==(const vgl_homg_point_3d<Type> &other) const
{
  bool result;

  result=this==&other;
  if(!result)
    result=((this->x()==other.x()) && (this->y()==other.y())
      && (this->z()==other.z()) &&  (this->w()==other.w()));
  return result;

}


#define VGL_HOMG_POINT_3D_INSTANTIATE(T) \
template class vgl_homg_point_3d<T>; \
template vcl_ostream& operator<<(vcl_ostream&, vgl_homg_point_3d<T>const&); \
template vcl_istream& operator>>(vcl_istream&, vgl_homg_point_3d<T>&)

