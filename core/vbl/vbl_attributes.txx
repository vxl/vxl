#ifndef VBL_ATTRIBUTES_TXX_
#define VBL_ATTRIBUTES_TXX_
#include <vbl/vbl_attributes.h>
#include <vcl_limits.h>
#include <vcl_cassert.h>
// ------------ 1d array ----------
//: minimum element value
template <class T>
T minval(vbl_array_1d<T> const& in)
{
  T mval = vcl_numeric_limits<T>::max();
  vcl_size_t n = in.size();
  assert(n>0);
  for(vbl_array_1d<T>::const_iterator it = in.begin();
      it != in.end(); ++it){
    T v = *it;
    if(v<mval) mval = v;
  }
  return mval;
}
//: maximum element value
template <class T>
T maxval(vbl_array_1d<T> const& in)
{
  T mval = vcl_numeric_limits<T>::min();
  vcl_size_t n = in.size();
  assert(n>0);
  for(vbl_array_1d<T>::const_iterator it = in.begin();
      it != in.end(); ++it){
    T v = *it;
    if(v>mval) mval = v;
  }
  return mval;
}
//: min max bounds
template <class T>
void bounds(vbl_array_1d<T> const& in, T& min, T& max)
{
  max = vcl_numeric_limits<T>::min(); min = vcl_numeric_limits<T>::max();
  vcl_size_t n = in.size();
  assert(n>0);
  for(vbl_array_1d<T>::const_iterator it = in.begin();
      it != in.end(); ++it){
    T v = *it;
    if(v>max) max = v;
    if(v<min) min = v;
  }
}
//: mean of element values
template <class T>
T mean(vbl_array_1d<T> const& in)
{
  T sum = T(0);
  vcl_size_t n = in.size();
  assert(n>0);
  for(vbl_array_1d<T>::const_iterator it = in.begin();
      it != in.end(); ++it) sum += *it;
  return sum/static_cast<T>(n);
}
// ----------------- 2d array ------------
//: minimum element value
template <class T>
T minval(vbl_array_2d<T> const& in)
{
  T mval = vcl_numeric_limits<T>::max();
  vcl_size_t n = in.size();
  assert(n>0);
  for(vbl_array_2d<T>::const_iterator it = in.begin();
      it != in.end(); ++it){
    T v = *it;
    if(v<mval) mval = v;
  }
  return mval;
}
//: maximum element value
template <class T>
T maxval(vbl_array_2d<T> const& in)
{
  T mval = vcl_numeric_limits<T>::min();
  vcl_size_t n = in.size();
  assert(n>0);
  for(vbl_array_2d<T>::const_iterator it = in.begin();
      it != in.end(); ++it){
    T v = *it;
    if(v>mval) mval = v;
  }
  return mval;
}
//: min max bounds
template <class T>
void bounds(vbl_array_2d<T> const& in, T& min, T& max)
{
  max = vcl_numeric_limits<T>::min(); min = vcl_numeric_limits<T>::max();
  vcl_size_t n = in.size();
  assert(n>0);
  for(vbl_array_2d<T>::const_iterator it = in.begin();
      it != in.end(); ++it){
    T v = *it;
    if(v>max) max = v;
    if(v<min) min = v;
  }
}
//: mean of element values
template <class T>
T mean(vbl_array_2d<T> const& in)
{
  T sum = T(0);
  vcl_size_t n = in.size();
  assert(n>0);
  for(vbl_array_2d<T>::const_iterator it = in.begin();
      it != in.end(); ++it) sum += *it;
  return sum/static_cast<T>(n);
}
// ----------------- 3d array ------------
//: minimum element value
template <class T>
T minval(vbl_array_3d<T> const& in)
{
  T mval = vcl_numeric_limits<T>::max();
  vcl_size_t n = in.size();
  assert(n>0);
  for(vbl_array_3d<T>::const_iterator it = in.begin();
      it != in.end(); ++it){
    T v = *it;
    if(v<mval) mval = v;
  }
  return mval;
}
//: maximum element value
template <class T>
T maxval(vbl_array_3d<T> const& in)
{
  T mval = vcl_numeric_limits<T>::min();
  vcl_size_t n = in.size();
  assert(n>0);
  for(vbl_array_3d<T>::const_iterator it = in.begin();
      it != in.end(); ++it){
    T v = *it;
    if(v>mval) mval = v;
  }
  return mval;
}
//: min max bounds
template <class T>
void bounds(vbl_array_3d<T> const& in, T& min, T& max)
{
  max = vcl_numeric_limits<T>::min(); min = vcl_numeric_limits<T>::max();
  vcl_size_t n = in.size();
  assert(n>0);
  for(vbl_array_3d<T>::const_iterator it = in.begin();
      it != in.end(); ++it){
    T v = *it;
    if(v>max) max = v;
    if(v<min) min = v;
  }
}
//: mean of element values
template <class T>
T mean(vbl_array_3d<T> const& in)
{
  T sum = T(0);
  vcl_size_t n = in.size();
  assert(n>0);
  for(vbl_array_3d<T>::const_iterator it = in.begin();
      it != in.end(); ++it) sum += *it;
  return sum/static_cast<T>(n);
}

#define VBL_ATTRIBUTES_INSTANTIATE(T) \
template T minval(vbl_array_1d<T >const&); \
template T maxval(vbl_array_1d<T >const&); \
template void bounds(vbl_array_1d<T >const&, T&, T&); \
template T mean(vbl_array_1d<T >const&);   \
template T minval(vbl_array_2d<T >const&); \
template T maxval(vbl_array_2d<T >const&); \
template void bounds(vbl_array_2d<T >const&, T&, T&); \
template T mean(vbl_array_2d<T >const&);   \
template T minval(vbl_array_3d<T >const&); \
template T maxval(vbl_array_3d<T >const&); \
template void bounds(vbl_array_3d<T >const&, T&, T&); \
template T mean(vbl_array_3d<T >const&);  

#endif // VBL_ATTRIBUTES_TXX_
