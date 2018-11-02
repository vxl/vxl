// This is core/vgl/algo/vgl_orient_box_3d_operators.hxx
#ifndef vgl_orient_box_3d_operators_hxx_
#define vgl_orient_box_3d_operators_hxx_
//:
// \file

#include "vgl_orient_box_3d_operators.h"
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_vector_fixed.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
vgl_orient_box_3d<T>
vgl_orient_box_3d_operators<T>::minimal_box(std::vector<vgl_point_3d<T> > const& plist)
{
  if (plist.size() == 0)
    return vgl_box_3d<T>(); // an empty box
  else if (plist.size() == 1)
    return vgl_orient_box_3d_operators<T>::minimal_box(plist.front());
  else if (plist.size() == 2)
  {
    vgl_box_3d<T> bb; bb.add(plist.front());
    vgl_vector_3d<T> dir = plist.back() - plist.front();
    vgl_vector_3d<T> rot_dir = vgl_vector_3d<T>(T(dir.length()), T(0), T(0));
    bb.add(plist.front() + rot_dir);
    vnl_quaternion<double> orient(vnl_vector_fixed<double,3>(dir.x(), dir.y(), dir.z()), 0.0);
    return vgl_orient_box_3d<T>(bb, orient);
  }
  else
    assert(!"Not yet implemented minimal_box() for more than 2 points");
  return vgl_box_3d<T>(); // return an empty box in case of error
}

#undef VGL_ORIENT_BOX_3D_OPERATORS_INSTANTIATE
#define VGL_ORIENT_BOX_3D_OPERATORS_INSTANTIATE(T) \
template class vgl_orient_box_3d_operators<T >

#endif // vgl_orient_box_3d_operators_hxx_
