// This is vxl/vgl/vgl_homg_point_3d.txx
#ifndef vgl_homg_point_3d_txx_
#define vgl_homg_point_3d_txx_

#include "vgl_homg_point_3d.h"
#include <vgl/vgl_homg_plane_3d.h>
#include <vcl_iostream.h>

// Note that the given planes must be distinct and not have a line in common!
template <class Type>
vgl_homg_point_3d<Type>::vgl_homg_point_3d (vgl_homg_plane_3d<Type> const& l1,
                                            vgl_homg_plane_3d<Type> const& l2,
                                            vgl_homg_plane_3d<Type> const& l3)
{
  set( l1.ny()*l2.nz()*l3.d()+l2.ny()*l3.nz()*l1.d()+l3.ny()*l1.nz()*l2.d()
      -l1.ny()*l3.nz()*l2.d()-l2.ny()*l1.nz()*l3.d()-l3.ny()*l2.nz()*l1.d(),
       l1.nz()*l2.d()*l3.nx()+l2.nz()*l3.d()*l1.nx()+l3.nz()*l1.d()*l2.nx()
      -l1.nz()*l3.d()*l2.nx()-l2.nz()*l1.d()*l3.nx()-l3.nz()*l2.d()*l1.nx(),
       l1.d()*l2.nx()*l3.ny()+l2.d()*l3.nx()*l1.ny()+l3.d()*l1.nx()*l2.ny()
      -l1.d()*l3.nx()*l2.ny()-l2.d()*l1.nx()*l3.ny()-l3.d()*l2.nx()*l1.ny(),
       l1.nx()*l2.ny()*l3.nz()+l2.nx()*l3.ny()*l1.nz()+l3.nx()*l1.ny()*l2.nz()
      -l1.nx()*l3.ny()*l2.nz()-l2.nx()*l1.ny()*l3.nz()-l3.nx()*l2.ny()*l1.nz());
}

template <class Type>
bool vgl_homg_point_3d<Type>::operator==(vgl_homg_point_3d<Type> const& other) const
{
  return (this==&other) ||
         (   this->x()*other.w() == this->w()*other.x()
          && this->y()*other.w() == this->w()*other.y()
          && this->z()*other.w() == this->w()*other.z());
}

template <class Type>
vcl_ostream& operator<<(vcl_ostream& s, vgl_homg_point_3d<Type> const& p)
{
  return s << " <vgl_homg_point_3d ("
           << p.x() << "," << p.y() << ","
           << p.z() << "," << p.w() << ") >";
}

template <class Type>
vcl_istream& operator>>(vcl_istream& s, vgl_homg_point_3d<Type>& p)
{
  Type x, y, z, w;
  s >> x >> y >> z >> w;
  p.set(x,y,z,w);
  return s;
}

#define VGL_HOMG_POINT_3D_INSTANTIATE(T) \
template class vgl_homg_point_3d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, vgl_homg_point_3d<T >const&); \
template vcl_istream& operator>>(vcl_istream&, vgl_homg_point_3d<T >&)

#endif // vgl_homg_point_3d_txx_
