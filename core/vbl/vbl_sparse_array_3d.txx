//-*- c++ -*-------------------------------------------------------------------
//
// Class: vbl_sparse_array_3d
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 02 Oct 96
//
//-----------------------------------------------------------------------------

#include "vbl_sparse_array_3d.h"
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>

// Default ctor
template <class T>
vbl_sparse_array_3d<T>::vbl_sparse_array_3d(unsigned int n1, unsigned int n2, unsigned int n3):
  n1_(n1),
  n2_(n2),
  n3_(n3)
{
}

// Copy ctor
template <class T>
vbl_sparse_array_3d<T>::vbl_sparse_array_3d(vbl_sparse_array_3d<T> const & that)
{
  operator=(that);
}

// Assignment
template <class T>
vbl_sparse_array_3d<T>& vbl_sparse_array_3d<T>::operator=(vbl_sparse_array_3d<T> const & that)
{
  n1_ = that.n1_;
  n2_ = that.n2_;
  n3_ = that.n3_;
  storage_ = that.storage_;
  return *this;
}

// Destructor
template <class T>
vbl_sparse_array_3d<T>::~vbl_sparse_array_3d()
{
}

// locals
static inline unsigned encode(unsigned i, unsigned j, unsigned k)
{
  // A bit of an arbitrary restriction, for efficiency 
  // 2048 is 1 << 11, could be 1 << 21 for 64bit machines.
  // Use a map of tuples if you need bigger sparse arrays
  assert( (i < 1024) && (j < 2048) && (k < 2048) );
  return (i << 22) | (j << 11) | k;
}

static inline void decode(unsigned v, unsigned& i, unsigned& j, unsigned& k)
{
  unsigned lowmask = (1U << 11) - 1;
  k = v & lowmask;
  j = (v >> 11) & lowmask;
  i = (v >> 22) & lowmask;
}

template <class T>
T& vbl_sparse_array_3d<T>::operator () (unsigned i, unsigned j, unsigned k)
{
  //   cout << "{vbl_sparse_array_3d(" << i << "," << j << "," << k << ") - " << "storage[" << encode(i,j,k) << "] - " << storage_[encode(i,j,k)] << "}";
  
  return storage_[encode(i,j,k)];
}

template <class T>
const T& vbl_sparse_array_3d<T>::operator () (unsigned i, unsigned j, unsigned k) const
{
  Map::const_iterator p = storage_.find(encode(i,j,k));
  //cout << "{vbl_sparse_array_3d(" << i << "," << j << "," << k << ") - " << "storage[" << encode(i,j,k) << "] - " << storage_[encode(i,j,k)] << "}";

  assert(p != storage_.end());

  return (*p).second;
}

template <class T>
bool vbl_sparse_array_3d<T>::fullp(unsigned i, unsigned j, unsigned k) const
{
  // cout << "{vbl_sparse_array_3d::fullp(" << i << "," << j << "," << k << ") - " << (storage_.find(encode(i,j,k)) != storage_.end()) << "}";

  return (storage_.find(encode(i,j,k)) != storage_.end());
}

template <class T>
bool vbl_sparse_array_3d<T>::put(unsigned i, unsigned j, unsigned k, const T& t)
{
  unsigned int v = encode(i,j,k);
  vcl_pair<Map::iterator,bool> res = storage_.insert(Map::value_type(v,t));
  //  cout << "{vbl_sparse_array_3d::put(" << i << "," << j << "," << k << ") - " << res.second << "}";

  return res.second;
} 

template <class T>
ostream& vbl_sparse_array_3d<T>::print(ostream& out) const
{
  for(Map::const_iterator p = storage_.begin(); p != storage_.end(); ++p) {
    unsigned i,j,k;
    decode((*p).first, i, j, k);
    out << "(" << i << "," << j << "," << k << "): " << (*p).second << vcl_endl;
  }
  return out;
}

#undef VBL_SPARSE_ARRAY_3D_INSTANTIATE_base
#define VBL_SPARSE_ARRAY_3D_INSTANTIATE_base(T)\
template class vbl_sparse_array_3d<T>

#undef VBL_SPARSE_ARRAY_3D_INSTANTIATE 
#define VBL_SPARSE_ARRAY_3D_INSTANTIATE(T) \
VBL_SPARSE_ARRAY_3D_INSTANTIATE_base(T); \
VCL_INSTANTIATE_INLINE(ostream& operator << (ostream&, const vbl_sparse_array_3d<T> &))

#ifdef MAIN
int main()
{
  vbl_sparse_array_3d<double> x;

  x(1,2,3) = 1.23;
  x(100,200,3) = 100.2003;

  vcl_cout << "123 = " << x(1,2,3) << vcl_endl;
  vcl_cout << "222 = " << x(2,2,2) << vcl_endl;

  vcl_cout << "333 is full? " << x.fullp(3,3,3) << vcl_endl;
  
  vcl_cout << x;
}
#endif
