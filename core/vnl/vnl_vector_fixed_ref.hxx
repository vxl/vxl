// This is core/vnl/vnl_vector_fixed_ref.hxx
#ifndef vnl_vector_fixed_ref_hxx_
#define vnl_vector_fixed_ref_hxx_
// Author: Paul P. Smyth, Vicon Motion Systems Ltd.
// Created: 02 May 2001
//
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include "vnl_vector_fixed_ref.h"
#include <vcl_cassert.h>
#include <vcl_compiler.h>
#include <vnl/vnl_math.h>  // for vnl_math::isfinite


//------------------------------------------------------------

template<class T, vxl::indexsize_t n>
vnl_vector_fixed<T,n>
vnl_vector_fixed_ref_const<T,n>::apply( T (*f)(T) ) const
{
  vnl_vector_fixed<T,n> ret;
  for ( size_type i = 0; i < n; ++i )
    ret[i] = f( data_block()[i] );
  return ret;
}

template<class T, vxl::indexsize_t n>
vnl_vector_fixed<T,n>
vnl_vector_fixed_ref_const<T,n>::apply( T (*f)(const T&) ) const
{
  vnl_vector_fixed<T,n> ret;
  for ( size_type i = 0; i < n; ++i )
    ret[i] = f( data_block()[i] );
  return ret;
}


template<class T, vxl::indexsize_t n>
vnl_vector<T>
vnl_vector_fixed_ref_const<T,n>::extract( vxl::indexsize_t len, vxl::indexsize_t start ) const
{
  assert( start < n && start + len <= n );
  return vnl_vector<T>( data_block() + start, len );
}

template<class T, vxl::indexsize_t n>
vnl_vector_fixed_ref<T,n> const&
vnl_vector_fixed_ref<T,n>::update( const vnl_vector<T>& v, vxl::indexsize_t start ) const
{
  size_type stop = start + v.size();
  assert( stop <= n );
  for (size_type i = start; i < stop; i++)
    this->data_block()[i] = v[i-start];
  return *this;
}

template <class T, vxl::indexsize_t n>
vnl_vector_fixed_ref<T,n> const&
vnl_vector_fixed_ref<T,n>::flip() const
{
  for ( vxl::indexsize_t i=0; 2*i+1 < n; ++i )
    std::swap( data_block()[i], data_block()[n-1-i] );
  return *this;
}

template <class T, vxl::indexsize_t n>
bool
vnl_vector_fixed_ref_const<T,n>::is_finite() const
{
  for ( size_type i = 0; i < this->size(); ++i )
    if ( !vnl_math::isfinite( (*this)[i] ) )
      return false;

  return true;
}


template <class T, vxl::indexsize_t n>
bool
vnl_vector_fixed_ref_const<T,n>::is_zero() const
{
  T const zero(0);
  for ( size_type i = 0; i < this->size(); ++i )
    if ( !( (*this)[i] == zero) )
      return false;

  return true;
}


template <class T, vxl::indexsize_t n>
bool
vnl_vector_fixed_ref<T,n>::read_ascii(std::istream& s) const
{
  for (vxl::indexsize_t i = 0; i < this->size(); ++i)
    s >> (*this)(i);

  return s.good() || s.eof();
}

template <class T, vxl::indexsize_t n>
void
vnl_vector_fixed_ref_const<T,n>::assert_finite_internal() const
{
  if (this->is_finite())
    return;

  std::cerr << __FILE__ ": *** NAN FEVER **\n" << *this;
  std::abort();
}

template <class T, vxl::indexsize_t n>
void
vnl_vector_fixed_ref_const<T,n>::print( std::ostream& s ) const
{
  if ( this->size() > 0 )
    s << (*this)[0];
  for ( size_type i = 1; i < this->size(); ++i )
    s << ' ' << (*this)[i];
}

// instantiation macros for vnl_vector_fixed_ref<T,vxl::indexsize_t> :

#define VNL_VECTOR_FIXED_REF_INSTANTIATE(T,n) \
template class VNL_EXPORT vnl_vector_fixed_ref<T, n >; \
template class VNL_EXPORT vnl_vector_fixed_ref_const<T, n >

#endif // vnl_vector_fixed_ref_hxx_
