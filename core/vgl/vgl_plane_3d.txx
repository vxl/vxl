// This is vxl/vgl/vgl_plane_3d.txx
#ifndef vgl_plane_3d_txx_
#define vgl_plane_3d_txx_

//:
// \file
// \author Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
// \brief Represents a euclidean 3D plane.

#include <vcl_iostream.h>
#include "vgl_plane_3d.h"

template <class Type>
vcl_ostream& operator<<(vcl_ostream& s, const vgl_plane_3d<Type>& p) {
  return s << " <vgl_plane_3d "
           << p.data_[0] << " x + "
           << p.data_[1] << " y + "
           << p.data_[2] << " z + "
           << p.data_[3] << " = 0 >";
}

template <class Type>
vcl_istream&  operator>>(vcl_istream& is, vgl_plane_3d<Type>& p) {
  return is >> p.data_[0] >> p.data_[1] >> p.data_[2] >> p.data_[3];
}

#endif // vgl_plane_3d_txx_
