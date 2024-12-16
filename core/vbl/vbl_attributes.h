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

#include "vbl_array_1d.h"
#include "vbl_array_2d.h"
#include "vbl_array_3d.h"

//: minimum element value
template <class T>
T
minval(const vbl_array_1d<T> & in);
//: maximum element value
template <class T>
T
maxval(const vbl_array_1d<T> & in);
//: min max bounds
template <class T>
void
bounds(const vbl_array_1d<T> & in, T & min, T & max);
//: mean of element values
template <class T>
T
mean(const vbl_array_1d<T> & in);
//: median of element values
// Actually, the "left median" is returned:
// it's always a value from the array, which for even-sized arrays is a
// "left-biased" or better said, a "smaller-biased" middle entry from the array.
template <class T>
T
median(const vbl_array_1d<T> & in);

//: minimum element value
template <class T>
T
minval(const vbl_array_2d<T> & in);
//: maximum element value
template <class T>
T
maxval(const vbl_array_2d<T> & in);
//: min max bounds
template <class T>
void
bounds(const vbl_array_2d<T> & in, T & min, T & max);
//: mean of element values
template <class T>
T
mean(const vbl_array_2d<T> & in);
//: median of element values
// Actually, the "left median" is returned:
// it's always a value from the array, which for even-sized arrays is a
// "left-biased" or better said, a "smaller-biased" middle entry from the array.
template <class T>
T
median(const vbl_array_2d<T> & in);

//: minimum element value
template <class T>
T
minval(const vbl_array_3d<T> & in);
//: maximum element value
template <class T>
T
maxval(const vbl_array_3d<T> & in);
//: min max bounds
template <class T>
void
bounds(const vbl_array_3d<T> & in, T & min, T & max);
//: mean of element values
template <class T>
T
mean(const vbl_array_3d<T> & in);
//: median of element values
// Actually, the "left median" is returned:
// it's always a value from the array, which for even-sized arrays is a
// "left-biased" or better said, a "smaller-biased" middle entry from the array.
template <class T>
T
median(const vbl_array_3d<T> & in);

#endif // vbl_attributes_h_
