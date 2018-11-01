// This is core/vbl/vbl_attributes.h
#ifndef vbl_attributes_h_
#define vbl_attributes_h_
//:
// \file
// \brief Find summary attributes of an array
// \author J.L. Mundy
//  October 5, 2010
// \verbatim
//  Modifications
//   Oct 7, 2010 - Peter Vanroose - Added the median
// \endverbatim

#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>

//: minimum element value
template <class T>
T minval(vbl_array_1d<T> const& in);
//: maximum element value
template <class T>
T maxval(vbl_array_1d<T> const& in);
//: min max bounds
template <class T>
void bounds(vbl_array_1d<T> const& in, T& min, T& max);
//: mean of element values
template <class T>
T mean(vbl_array_1d<T> const& in);
//: median of element values
// Actually, the "left median" is returned:
// it's always a value from the array, which for even-sized arrays is a
// "left-biased" or better said, a "smaller-biased" middle entry from the array.
template <class T>
T median(vbl_array_1d<T> const& in);

//: minimum element value
template <class T>
T minval(vbl_array_2d<T> const& in);
//: maximum element value
template <class T>
T maxval(vbl_array_2d<T> const& in);
//: min max bounds
template <class T>
void bounds(vbl_array_2d<T> const& in, T& min, T& max);
//: mean of element values
template <class T>
T mean(vbl_array_2d<T> const& in);
//: median of element values
// Actually, the "left median" is returned:
// it's always a value from the array, which for even-sized arrays is a
// "left-biased" or better said, a "smaller-biased" middle entry from the array.
template <class T>
T median(vbl_array_2d<T> const& in);

//: minimum element value
template <class T>
T minval(vbl_array_3d<T> const& in);
//: maximum element value
template <class T>
T maxval(vbl_array_3d<T> const& in);
//: min max bounds
template <class T>
void bounds(vbl_array_3d<T> const& in, T& min, T& max);
//: mean of element values
template <class T>
T mean(vbl_array_3d<T> const& in);
//: median of element values
// Actually, the "left median" is returned:
// it's always a value from the array, which for even-sized arrays is a
// "left-biased" or better said, a "smaller-biased" middle entry from the array.
template <class T>
T median(vbl_array_3d<T> const& in);

#endif // vbl_attributes_h_
