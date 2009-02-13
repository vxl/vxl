/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_serialize_stl_vector_hxx
#define rgtl_serialize_stl_vector_hxx

//:
// \file
// \brief Implement serialization for stl vector.
// \author Brad King
// \date February 2008

#include "rgtl_serialize_split.hxx"

#include <vcl_vector.h>

#include <vcl_iostream.h>

template <class Serializer, typename T>
void rgtl_serialize_save(Serializer& sr, vcl_vector<T>& self)
{
  typename vcl_vector<T>::size_type sz = self.size();
  sr << sz;
  for(typename vcl_vector<T>::iterator i = self.begin(); i != self.end(); ++i)
    {
    sr << *i;
    }
}

template <class Serializer, typename T>
void rgtl_serialize_load(Serializer& sr, vcl_vector<T>& self)
{
  typename vcl_vector<T>::size_type sz;
  sr >> sz;
  self.resize(sz);
  for(typename vcl_vector<T>::iterator i = self.begin(); i != self.end(); ++i)
    {
    sr >> *i;
    }
}

template <class Serializer, typename T>
void rgtl_serialize(Serializer& sr, vcl_vector<T>& self)
{
  rgtl_serialize_split(sr, self);
}

#endif
