// This is vxl/vgl/vgl_homg_plane_3d.txx
#ifndef vgl_homg_plane_3d_txx_
#define vgl_homg_plane_3d_txx_

#include <vcl_iostream.h>
#include "vgl_homg_plane_3d.h"
#include <vgl/vgl_homg_point_3d.h>

//: Construct from Normal and a point
template <class Type>
vgl_homg_plane_3d<Type>::vgl_homg_plane_3d(const Type normal[3],
                                           const vgl_homg_point_3d<Type> &p)
{
  // find d given then x*nx + y*ny + z*nz + d*w = 0

  Type w=p.w();

  // TODO - use tolerance
  if(w != 0.0)
    {
      Type val=normal[0]*p.x()+normal[1]*p.y()+normal[2]*p.z();
      Type d=val/w;
      set(normal[0],normal[1],normal[2],d);
    }
  else
    // the point is at infinity so set the plane to infinity TODO
    set(0,0,0,1);
}

template <class Type>
bool vgl_homg_plane_3d<Type>::operator==(vgl_homg_plane_3d<Type> const & other) const
{
  return (this==&other) ||
         (   (this->nx()==other.nx()) && (this->ny()==other.ny())
          && (this->nz()==other.nz()) && (this->d() ==other.d()));
}

template <class Type>
inline vcl_ostream&  operator<<(vcl_ostream& s, const vgl_homg_plane_3d<Type>& p) {
  return s << " <vgl_homg_plane_3d "
           << p.data_[0] << " x + "
           << p.data_[1] << " y + "
           << p.data_[2] << " z + "
           << p.data_[3] << " = 0 >";
}

template <class Type>
vcl_istream&  operator>>(vcl_istream& is, vgl_homg_plane_3d<Type>& p) {
  return is >> p.data_[0] >> p.data_[1] >> p.data_[2] >> p.data_[3];
}

#endif // vgl_homg_plane_3d_txx_
