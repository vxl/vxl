// This is core/vbl/vbl_local_minima.h
#ifndef vbl_local_minima_h_
#define vbl_local_minima_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Find local minima in arrays
// \author J.L. Mundy
//  October 2, 2010
// \verbatim
//  Modificationss
//   <None>
// \endverbatim

#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>

//: Find the local minima in arrays
//  The result is an array with non-zero elements where local minima
//  exist. A local minimum must be smaller than *all* neigboring elements
//  by a threshold. The neighborhoods in 2-d and 3-d are 8-connected.
//  The result is an array with non-zero elements at minima.
//  The value of non-zero element indicates the smallest difference
//  between the minima and the neighboring elements.
template <class T>
bool local_minima(vbl_array_1d<T> const& in, vbl_array_1d<T>& minima,
                  T thresh = T(0));

template <class T>
bool local_minima(vbl_array_2d<T> const& in, vbl_array_2d<T>& minima,
                  T thresh = T(0));

template <class T>
bool local_minima(vbl_array_3d<T> const& in, vbl_array_3d<T>& minima,
                  T thresh = T(0));

#endif // vbl_local_minima_h_
