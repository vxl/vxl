#ifndef rgrl_set_of_txx_
#define rgrl_set_of_txx_
//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include <iostream>
#include <algorithm>
#include "rgrl_set_of.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template<typename T>
rgrl_set_of<T>::
rgrl_set_of() = default;


template<typename T>
void
rgrl_set_of<T>::
push_back( T const& item )
{
  data_.push_back( item );
}


template<typename T>
std::size_t
rgrl_set_of<T>::
size() const
{
  return data_.size();
}

template<typename T>
bool
rgrl_set_of<T>::
empty() const
{
  return data_.empty();
}

template<typename T>
T const&
rgrl_set_of<T>::
operator[]( unsigned i ) const
{
  return data_[i];
}


template<typename T>
T&
rgrl_set_of<T>::
operator[]( unsigned i )
{
  return data_[i];
}


template<typename T>
void
rgrl_set_of<T>::
clear()
{
  data_.clear();
}

template<typename T>
void
rgrl_set_of<T>::
resize( unsigned n )
{
  data_.resize( n );
}

template<typename T>
void
rgrl_set_of<T>::
fill( T const& item )
{
  std::fill( data_.begin(), data_.end(), item );
}

#endif // rgrl_set_of_txx_
