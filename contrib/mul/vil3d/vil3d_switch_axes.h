#ifndef vil3d_switch_axes_h_
#define vil3d_switch_axes_h_
//:
// \file
// \author Tim Cootes.

#include <vil3d/vil3d_image_view.h>

//: Change axes so that result(j,i,k)==im(i,j,k)
// \relatesalso vil3d_image_view
template<class T>
inline vil3d_image_view<T> vil3d_switch_axes_jik(const vil3d_image_view<T> &im)
{
  return vil3d_image_view<T>(im.memory_chunk(),im.origin_ptr(),
                             im.nj(),im.ni(),im.nk(),im.nplanes(),
                             im.jstep(),im.istep(),im.kstep(),im.planestep());
}

//: Change axes so that result(j,k,i)==im(i,j,k)
// \relatesalso vil3d_image_view
template<class T>
inline vil3d_image_view<T> vil3d_switch_axes_jki(const vil3d_image_view<T> &im)
{
  return vil3d_image_view<T>(im.memory_chunk(),im.origin_ptr(),
                             im.nj(),im.nk(),im.ni(),im.nplanes(),
                             im.jstep(),im.kstep(),im.istep(),im.planestep());
}

//: Change axes so that result(k,i,j)==im(i,j,k)
// \relatesalso vil3d_image_view
template<class T>
inline vil3d_image_view<T> vil3d_switch_axes_kij(const vil3d_image_view<T> &im)
{
  return vil3d_image_view<T>(im.memory_chunk(),im.origin_ptr(),
                             im.nk(),im.ni(),im.nj(),im.nplanes(),
                             im.kstep(),im.istep(),im.jstep(),im.planestep());
}

//: Change axes so that result(k,j,i)==im(i,j,k)
// \relatesalso vil3d_image_view
template<class T>
inline vil3d_image_view<T> vil3d_switch_axes_kji(const vil3d_image_view<T> &im)
{
  return vil3d_image_view<T>(im.memory_chunk(),im.origin_ptr(),
                             im.nk(),im.nj(),im.ni(),im.nplanes(),
                             im.kstep(),im.jstep(),im.istep(),im.planestep());
}

//: Change axes so that result(i,k,j)==im(i,j,k)
// \relatesalso vil3d_image_view
template<class T>
inline vil3d_image_view<T> vil3d_switch_axes_ikj(const vil3d_image_view<T> &im)
{
  return vil3d_image_view<T>(im.memory_chunk(),im.origin_ptr(),
                             im.ni(),im.nk(),im.nj(),im.nplanes(),
                             im.istep(),im.kstep(),im.jstep(),im.planestep());
}

#endif // vil3d_switch_axes_h_
