// This is core/vbl/vbl_sparse_array_base.hxx
#ifndef vbl_sparse_array_base_hxx_
#define vbl_sparse_array_base_hxx_
//:
// \file
// \brief Contains a base class for sparse arrays.
// \author Ian Scott

#include <utility>
#include "vbl_sparse_array_base.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Empty the sparse matrix.
template <class T, class Index>
void vbl_sparse_array_base<T, Index>::clear()
{
  storage_.clear();
}

//: Return contents of (i).  Assertion failure if not yet filled.
template <class T, class Index>
T const & vbl_sparse_array_base<T, Index>::operator () (Index i) const
{
  typename Map::const_iterator p = storage_.find(i);

  assert(p != storage_.end());

  return (*p).second;
}

//: Erase element at location (i). Assertion failure if not yet filled.
template <class T, class Index>
void vbl_sparse_array_base<T, Index>::erase (Index i)
{
  typename Map::iterator p = storage_.find(i);

  assert(p != storage_.end());

  storage_.erase(p);
}

//: Return the memory address of location (i).  0 if not yet filled.
template <class T, class Index>
T* vbl_sparse_array_base<T, Index>::get_addr(Index i)
{
  typename Map::iterator p = storage_.find(i);

  if (p == storage_.end())
    return nullptr;

  return &(*p).second;
}

//: Return true if location (i) has been filled.
template <class T, class Index>
bool vbl_sparse_array_base<T, Index>::fullp(Index i) const
{
  return storage_.find(i) != storage_.end();
}

//: Put a value into location (i).
template <class T, class Index>
bool vbl_sparse_array_base<T, Index>::put(Index i, const T& t)
{
  typedef typename Map::iterator iter;
  typedef typename Map::value_type value_type;
  std::pair<iter,bool> res = storage_.insert(value_type(i,t));

  return res.second;
}

#undef VBL_SPARSE_ARRAY_BASE_INSTANTIATE
#define VBL_SPARSE_ARRAY_BASE_INSTANTIATE(T, I) \
template class vbl_sparse_array_base<T , I >

#endif // vbl_sparse_array_base_hxx_
