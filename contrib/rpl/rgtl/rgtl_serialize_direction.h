/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_serialize_direction_h
#define rgtl_serialize_direction_h

//:
// \file
// \brief Serialization helper class to lookup direction of serialization.
// \author Brad King
// \date February 2008

//: Tags indicating the serialization direction.
class rgtl_serialize_direction_save;
class rgtl_serialize_direction_load;

//: Template to lookup the direction of a given serializer type.
//  This may be specialized by some serializer types.
template <class Serializer>
struct rgtl_serialize_direction
{
  typedef typename Serializer::serialize_direction type;
};

#endif
