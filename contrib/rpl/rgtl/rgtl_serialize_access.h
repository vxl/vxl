/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_serialize_access_h
#define rgtl_serialize_access_h

//:
// \file
// \brief Serialization helper class to access private methods.
// \author Brad King
// \date February 2008

//: Helper class to call private serialization members.
//  See rgtl_serialize.txt for serialization interface details.
class rgtl_serialize_access
{
public:
  template <typename Serializer, typename T>
  static void serialize(Serializer& rsr, T& x)
    {
    x.serialize(rsr);
    }
  template <typename Serializer, typename T>
  static void serialize_save(Serializer& rsr, T& x)
    {
    x.serialize_save(rsr);
    }
  template <typename Serializer, typename T>
  static void serialize_load(Serializer& rsr, T& x)
    {
    x.serialize_load(rsr);
    }
};

//: Helper function to call private serialize() members.
template <typename Serializer, typename T>
inline void rgtl_serialize(Serializer& rsr, T& x)
{
  rgtl_serialize_access::serialize(rsr, x);
}

//: Helper function to call private serialize_save() members.
template <typename Serializer, typename T>
inline void rgtl_serialize_save(Serializer& rsr, T& x)
{
  rgtl_serialize_access::serialize_save(rsr, x);
}

//: Helper function to call private serialize_load() members.
template <typename Serializer, typename T>
inline void rgtl_serialize_load(Serializer& rsr, T& x)
{
  rgtl_serialize_access::serialize_load(rsr, x);
}

#endif
