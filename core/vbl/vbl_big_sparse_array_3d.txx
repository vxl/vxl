// This is core/vbl/vbl_big_sparse_array_3d.txx
#ifndef vbl_big_sparse_array_3d_txx_
#define vbl_big_sparse_array_3d_txx_

#include "vbl_big_sparse_array_3d.h"
#include <vcl_cassert.h>
#include <vcl_utility.h> // for vcl_pair<T,bool>
#include <vcl_iostream.h>

// locals
inline ulonglong bigencode(unsigned i, unsigned j, unsigned k)
{
  // Use a map of tuples if you need bigger sparse arrays
#if VXL_HAS_INT_64
  assert( i <= 0x3fffff && j <= 0x1fffff && k <= 0x1fffff );
  return (((ulonglong)i) << 42) |
         (((ulonglong)j) << 21) |
         ( (ulonglong)k);
#else
  assert( i <= 0x3ff && j <= 0x7ff && k <= 0x7ff );
  return (((ulonglong)i) << 22) |
         (((ulonglong)j) << 11) |
         ( (ulonglong)k);
#endif
}

inline void bigdecode(ulonglong v, unsigned& i, unsigned& j, unsigned& k)
{
#if VXL_HAS_INT_64
  k = (unsigned)(v & 0x1fffff); // 21 lowest bits
  j = (unsigned)((v >> 21) & 0x1fffff); // "middle" 21 bits
  i = (unsigned)((v >> 42) & 0x3fffff); // 22 highest bits
#else
  k = (unsigned)(v & 0x7ff); // 11 lowest bits
  j = (unsigned)((v >> 11) & 0x7ff); // "middle" 11 bits
  i = (unsigned)((v >> 22) & 0x3ff); // 10 highest bits
#endif
}

template <class T>
T& vbl_big_sparse_array_3d<T>::operator() (unsigned i, unsigned j, unsigned k)
{
#ifdef DEBUG
  vcl_cout << "{vbl_big_sparse_array_3d(" << i << ',' << j << ',' << k
           << ") - storage[" << bigencode(i,j,k) << "] - "
           << storage_[bigencode(i,j,k)] << "}\n";
#endif
  return storage_[bigencode(i,j,k)];
}

template <class T>
T const& vbl_big_sparse_array_3d<T>::operator() (unsigned i, unsigned j, unsigned k) const
{
  typename Map::const_iterator p = storage_.find(bigencode(i,j,k));
#ifdef DEBUG
  vcl_cout << "{vbl_big_sparse_array_3d(" << i << ',' << j << ',' << k
           << ") - storage[" << bigencode(i,j,k) << "] - "
           << storage_[bigencode(i,j,k)] << "}\n";
#endif
  assert(p != storage_.end());
  return (*p).second;
}

template <class T>
bool vbl_big_sparse_array_3d<T>::fullp(unsigned i, unsigned j, unsigned k) const
{
#ifdef DEBUG
  vcl_cout << "{vbl_big_sparse_array_3d::fullp(" << i << ',' << j << ',' << k << ") - "
           << (storage_.find(bigencode(i,j,k)) != storage_.end()) << "}\n";
#endif
  return (storage_.find(bigencode(i,j,k)) != storage_.end());
}

template <class T>
bool vbl_big_sparse_array_3d<T>::put(unsigned i, unsigned j, unsigned k, T const& t)
{
  typedef typename Map::iterator iter;
  typedef typename Map::value_type value_type;
  ulonglong v = bigencode(i,j,k);
  vcl_pair<iter,bool> res = storage_.insert(value_type(v,t));
#ifdef DEBUG
  vcl_cout << "{vbl_big_sparse_array_3d::put(" << i << ',' << j << ',' << k << ") - "
           << res.second << "}\n";
#endif
  return res.second;
}

template <class T>
vcl_ostream& vbl_big_sparse_array_3d<T>::print(vcl_ostream& out) const
{
  for (typename Map::const_iterator p = storage_.begin(); p != storage_.end(); ++p) {
    unsigned i,j,k;
    bigdecode((*p).first, i, j, k);
    out << '(' << i << ',' << j << ',' << k << "): " << (*p).second << vcl_endl;
  }
  return out;
}

#define VBL_BIG_SPARSE_ARRAY_3D_INSTANTIATE_base(T) \
template class vbl_big_sparse_array_3d<T >

#undef VBL_BIG_SPARSE_ARRAY_3D_INSTANTIATE
#define VBL_BIG_SPARSE_ARRAY_3D_INSTANTIATE(T) \
VBL_BIG_SPARSE_ARRAY_3D_INSTANTIATE_base(T); \
VCL_INSTANTIATE_INLINE(vcl_ostream& operator << (vcl_ostream&, vbl_big_sparse_array_3d<T > const&))

#endif // vbl_big_sparse_array_3d_txx_
