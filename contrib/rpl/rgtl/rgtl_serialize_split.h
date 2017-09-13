#ifndef rgtl_serialize_split_h
#define rgtl_serialize_split_h
//:
// \file
// \brief Serialization helper class to split save and load calls.
// \author Brad King
// \date February 2008
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "rgtl_serialize_direction.h"

//: Implement serialization by calling rgtl_serialize_save.
struct rgtl_serialize_split_save
{
  template <class Serializer, typename T>
  static void serialize(Serializer& sr, T& x)
  {
    rgtl_serialize_save(sr, x);
  }
};

//: Implement serialization by calling rgtl_serialize_load.
struct rgtl_serialize_split_load
{
  template <class Serializer, typename T>
  static void serialize(Serializer& sr, T& x)
  {
    rgtl_serialize_load(sr, x);
  }
};

//: Lookup table for our implementation of each direction.
template <typename SerializeDirection> struct rgtl_serialize_split_lookup;

template <> struct rgtl_serialize_split_lookup<rgtl_serialize_direction_save>
{
  typedef rgtl_serialize_split_save type;
};

template <> struct rgtl_serialize_split_lookup<rgtl_serialize_direction_load>
{
  typedef rgtl_serialize_split_load type;
};

//: Dispatch a call to rgtl_serialize_save or rgtl_serialize_load based on the direction of the given serializer.
template <class Serializer, typename T>
void rgtl_serialize_split(Serializer& sr, T& x)
{
  // Lookup the direction of this serializer.
  typedef typename rgtl_serialize_direction<Serializer>::type
    serialize_direction;

  // Lookup our implementation to serialize in the proper direction.
  typedef typename rgtl_serialize_split_lookup<serialize_direction>::type
    serialize_impl;

  // Invoke serialization of the proper direction.
  serialize_impl::serialize(sr, x);
}

#endif // rgtl_serialize_split_h
