#ifndef vgl_tolerance_txx_
#define vgl_tolerance_txx_

#include "vgl_tolerance.h"

#include <vcl_cmath.h>
#include <vcl_limits.h>

//! \file


template <typename T>
const T vgl_tolerance<T>::point_3d_coplanarity = (T)vcl_sqrt(vcl_numeric_limits<T>::epsilon());

template <typename T>
const T vgl_tolerance<T>::position = vcl_numeric_limits<T>::epsilon();




#undef VGL_TOLERANCE_INSTANTIATE
#define VGL_TOLERANCE_INSTANTIATE(T) \
template class vgl_tolerance<T >

#endif
