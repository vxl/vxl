//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_set_of.h"

template<typename T>
rgrl_set_of<T>::
rgrl_set_of()
{
}


template<typename T>
void 
rgrl_set_of<T>::
push_back( T const& item )
{
  data_.push_back( item );
}


template<typename T>
unsigned
rgrl_set_of<T>::
size() const
{
  return data_.size();
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
