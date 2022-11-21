// This is core/vnl/vnl_vector_ref.hxx
#ifndef vnl_vector_ref_hxx_
#define vnl_vector_ref_hxx_
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include "vnl_vector_ref.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vnl_math.h"  // for vnl_math::isfinite

#define VNL_VECTOR_REF_INSTANTIATE(T) \
template class vnl_vector_ref<T >

#endif // vnl_vector_ref_hxx_
