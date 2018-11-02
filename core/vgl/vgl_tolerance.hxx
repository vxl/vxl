#ifndef vgl_tolerance_hxx_
#define vgl_tolerance_hxx_

#include <cmath>
#include <limits>
#include "vgl_tolerance.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//! \file


template <typename T>
const T vgl_tolerance<T>::point_3d_coplanarity = (T)std::sqrt(1.0f*std::numeric_limits<T>::epsilon());

template <typename T>
const T vgl_tolerance<T>::position = std::numeric_limits<T>::epsilon();




#undef VGL_TOLERANCE_INSTANTIATE
#define VGL_TOLERANCE_INSTANTIATE(T) \
template class vgl_tolerance<T >

#endif
