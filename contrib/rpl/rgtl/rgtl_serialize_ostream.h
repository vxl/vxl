/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt) */

#ifndef rgtl_serialize_ostream_h
#define rgtl_serialize_ostream_h
//:
// \file
// \brief Save rgtl objects to a stream.
// \author Brad King
// \date February 2008

#include <iosfwd>
#include <iostream>
#include <cstddef>
#include "rgtl_serialize_direction.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Serialization save object that writes to a stream.
//  See rgtl_serialize.txt for serialization interface details.
class rgtl_serialize_ostream
{

 public:
  //: Mark this class as a serialization save direction.
  typedef rgtl_serialize_direction_save serialize_direction;

  //: Construct with a stream to which to write data.
  rgtl_serialize_ostream(std::ostream& is): stream_(is) {}

  //: Safe conversion to a boolean type.
  explicit operator bool() const
  { return this->okay()? true : false; }

  //: Write data to the output stream.
  rgtl_serialize_ostream& write(void const* data, std::size_t length);

  //: Get the current position in the stream.
  std::size_t position();
 private:
  bool okay() const;
  std::ostream& stream_;
};

//: Provide "saver << object" syntax
template <typename T>
inline rgtl_serialize_ostream& operator<<(rgtl_serialize_ostream& rsr, T& x)
{
  return rsr & x;
}

//: Provide "serializer & object" syntax for arbitrary objects
template <typename T>
rgtl_serialize_ostream& operator&(rgtl_serialize_ostream& rsr, T& x)
{
  rgtl_serialize(rsr, x);
  return rsr;
}

//: Provide "serializer & array" syntax for array types
template <typename T, unsigned int N>
rgtl_serialize_ostream& operator&(rgtl_serialize_ostream& rsr, T(&x)[N])
{
  for (unsigned int i=0; i < N; ++i)
    {
    rsr & x[i];
    }
  return rsr;
}

//: Poison "serializer & pointer" syntax for pointer types
template <typename T>
rgtl_serialize_ostream& operator&(rgtl_serialize_ostream& rsr, T*& x)
{
  rgtl_serialize_ostream_does_not_support_pointers(x);
  return rsr;
}

//: Provide "serializer & object" syntax for primitive types
#define RGTL_SERIALIZE_OSTREAM_DECL(T)                              \
  rgtl_serialize_ostream& operator&(rgtl_serialize_ostream& rsr, T& x)
RGTL_SERIALIZE_OSTREAM_DECL(bool);
RGTL_SERIALIZE_OSTREAM_DECL(char);
RGTL_SERIALIZE_OSTREAM_DECL(signed char);
RGTL_SERIALIZE_OSTREAM_DECL(unsigned char);
RGTL_SERIALIZE_OSTREAM_DECL(short);
RGTL_SERIALIZE_OSTREAM_DECL(unsigned short);
RGTL_SERIALIZE_OSTREAM_DECL(int);
RGTL_SERIALIZE_OSTREAM_DECL(unsigned int);
RGTL_SERIALIZE_OSTREAM_DECL(long);
RGTL_SERIALIZE_OSTREAM_DECL(unsigned long);
RGTL_SERIALIZE_OSTREAM_DECL(long long);
RGTL_SERIALIZE_OSTREAM_DECL(unsigned long long);
RGTL_SERIALIZE_OSTREAM_DECL(float);
RGTL_SERIALIZE_OSTREAM_DECL(double);
#undef RGTL_SERIALIZE_OSTREAM_DECL

#endif
