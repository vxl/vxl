#ifndef vbl_attributes_txx_
#define vbl_attributes_txx_
#include "vbl_attributes.h"
//:
// \file

#include <vcl_algorithm.h> // for std::sort()
#include <vcl_cassert.h>

// ------------ 1d array ----------

//: minimum element value
template <class T>
T minval(vbl_array_1d<T> const& in)
{
  assert(in.size()>0);
  T mval = *(in.begin());
  for (typename vbl_array_1d<T>::const_iterator it = in.begin();
       it != in.end(); ++it) {
    T v = *it;
    if (v<mval) mval = v;
  }
  return mval;
}

//: maximum element value
template <class T>
T maxval(vbl_array_1d<T> const& in)
{
  assert(in.size()>0);
  T mval = *(in.begin());
  for (typename vbl_array_1d<T>::const_iterator it = in.begin();
       it != in.end(); ++it) {
    T v = *it;
    if (v>mval) mval = v;
  }
  return mval;
}

//: min max bounds
template <class T>
void bounds(vbl_array_1d<T> const& in, T& min, T& max)
{
  assert(in.size()>0);
  max = min = *(in.begin());
  for (typename vbl_array_1d<T>::const_iterator it = in.begin();
       it != in.end(); ++it) {
    T v = *it;
    if (v>max) max = v;
    if (v<min) min = v;
  }
}

//: mean of element values
template <class T>
T mean(vbl_array_1d<T> const& in)
{
  T sum = T(0);
  vcl_size_t n = in.size();
  assert(n>0);
  for (typename vbl_array_1d<T>::const_iterator it = in.begin();
       it != in.end(); ++it)
    sum += *it;
  return sum/static_cast<T>(n);
}

// ----------------- 2d array ------------

//: minimum element value
template <class T>
T minval(vbl_array_2d<T> const& in)
{
  assert(in.size()>0);
  T mval = *(in.begin());
  for (typename vbl_array_2d<T>::const_iterator it = in.begin();
       it != in.end(); ++it) {
    T v = *it;
    if (v<mval) mval = v;
  }
  return mval;
}

//: maximum element value
template <class T>
T maxval(vbl_array_2d<T> const& in)
{
  assert(in.size()>0);
  T mval = *(in.begin());
  for (typename vbl_array_2d<T>::const_iterator it = in.begin();
       it != in.end(); ++it) {
    T v = *it;
    if (v>mval) mval = v;
  }
  return mval;
}

//: min max bounds
template <class T>
void bounds(vbl_array_2d<T> const& in, T& min, T& max)
{
  assert(in.size()>0);
  max = min = *(in.begin());
  for (typename vbl_array_2d<T>::const_iterator it = in.begin();
       it != in.end(); ++it) {
    T v = *it;
    if (v>max) max = v;
    if (v<min) min = v;
  }
}

//: mean of element values
template <class T>
T mean(vbl_array_2d<T> const& in)
{
  T sum = T(0);
  vcl_size_t n = in.size();
  assert(n>0);
  for (typename vbl_array_2d<T>::const_iterator it = in.begin();
       it != in.end(); ++it)
    sum += *it;
  return sum/static_cast<T>(n);
}

// ----------------- 3d array ------------

//: minimum element value
template <class T>
T minval(vbl_array_3d<T> const& in)
{
  assert(in.size()>0);
  T mval = *(in.begin());
  for (typename vbl_array_3d<T>::const_iterator it = in.begin();
       it != in.end(); ++it) {
    T v = *it;
    if (v<mval) mval = v;
  }
  return mval;
}

//: maximum element value
template <class T>
T maxval(vbl_array_3d<T> const& in)
{
  assert(in.size()>0);
  T mval = *(in.begin());
  for (typename vbl_array_3d<T>::const_iterator it = in.begin();
       it != in.end(); ++it) {
    T v = *it;
    if (v>mval) mval = v;
  }
  return mval;
}

//: min max bounds
template <class T>
void bounds(vbl_array_3d<T> const& in, T& min, T& max)
{
  assert(in.size()>0);
  max = min = *(in.begin());
  for (typename vbl_array_3d<T>::const_iterator it = in.begin();
       it != in.end(); ++it) {
    T v = *it;
    if (v>max) max = v;
    if (v<min) min = v;
  }
}

//: mean of element values
template <class T>
T mean(vbl_array_3d<T> const& in)
{
  T sum = T(0);
  vcl_size_t n = in.size();
  assert(n>0);
  for (typename vbl_array_3d<T>::const_iterator it = in.begin();
       it != in.end(); ++it)
    sum += *it;
  return sum/static_cast<T>(n);
}

//: median (actually: left-median) of element values
template <class T>
T median(vbl_array_1d<T> const& in)
{
  assert(in.size()>0);
  vbl_array_1d<T> t = in;
  vcl_sort(t.begin(), t.end());
  return *(t.begin() + (in.size()/2));
}

//: median (actually: left-median) of element values
template <class T>
T median(vbl_array_2d<T> const& in)
{
  assert(in.size()>0);
  vbl_array_2d<T> t = in;
  vcl_sort(t.begin(), t.end());
  return *(t.begin() + (in.size()/2));
}

//: median (actually: left-median) of element values
template <class T>
T median(vbl_array_3d<T> const& in)
{
  assert(in.size()>0);
  vbl_array_3d<T> t = in;
  vcl_sort(t.begin(), t.end());
  return *(t.begin() + (in.size()/2));
}

#define VBL_ATTRIBUTES_INSTANTIATE(T) \
template T minval(vbl_array_1d<T >const&); \
template T maxval(vbl_array_1d<T >const&); \
template void bounds(vbl_array_1d<T >const&, T&, T&); \
template T mean(vbl_array_1d<T >const&);   \
template T median(vbl_array_1d<T >const&); \
template T minval(vbl_array_2d<T >const&); \
template T maxval(vbl_array_2d<T >const&); \
template void bounds(vbl_array_2d<T >const&, T&, T&); \
template T mean(vbl_array_2d<T >const&);   \
template T median(vbl_array_2d<T >const&); \
template T minval(vbl_array_3d<T >const&); \
template T maxval(vbl_array_3d<T >const&); \
template void bounds(vbl_array_3d<T >const&, T&, T&); \
template T mean(vbl_array_3d<T >const&); \
template T median(vbl_array_3d<T >const&)

#endif // vbl_attributes_txx_
