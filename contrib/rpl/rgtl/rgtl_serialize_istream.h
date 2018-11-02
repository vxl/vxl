/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt) */

#ifndef rgtl_serialize_istream_h
#define rgtl_serialize_istream_h
//:
// \file
// \brief Load rgtl objects from a stream.
// \author Brad King
// \date February 2008

#include <iosfwd>
#include <iostream>
#include <cstddef>
#include "rgtl_serialize_direction.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Serialization load object that reads from a stream.
//  See rgtl_serialize.txt for serialization interface details.
class rgtl_serialize_istream
{

 public:
  //: Mark this class as a serialization load direction.
  typedef rgtl_serialize_direction_load serialize_direction;

  //: Construct with a stream from which to read data.
  rgtl_serialize_istream(std::istream& is);

  //: Safe conversion to a boolean type.
  explicit operator bool() const
  { return this->okay()? true : false; }

  //: Read data from the input stream.
  rgtl_serialize_istream& read(void* data, std::size_t length);

  //: Get the current position in the stream.
  std::size_t position();
 private:
  bool okay() const;
  std::istream& stream_;
  std::size_t buffer_left_;
  std::size_t buffer_used_;
  static unsigned int const buffer_size = 1024;
  char buffer_[buffer_size];
};

//: Provide "loader >> object" syntax
template <typename T>
inline rgtl_serialize_istream& operator>>(rgtl_serialize_istream& rsr, T& x)
{
  return rsr & x;
}

//: Provide "serializer & object" syntax for arbitrary objects
template <typename T>
rgtl_serialize_istream& operator&(rgtl_serialize_istream& rsr, T& x)
{
  rgtl_serialize(rsr, x);
  return rsr;
}

//: Provide "serializer & array" syntax for array types
template <typename T, unsigned int N>
rgtl_serialize_istream& operator&(rgtl_serialize_istream& rsr, T(&x)[N])
{
  for (unsigned int i=0; i < N; ++i)
    {
    rsr & x[i];
    }
  return rsr;
}

//: Poison "serializer & pointer" syntax for pointer types
template <typename T>
rgtl_serialize_istream& operator&(rgtl_serialize_istream& rsr, T*& x)
{
  rgtl_serialize_istream_does_not_support_pointers(x);
  return rsr;
}

//: Provide "serializer & object" syntax for primitive types
#define RGTL_SERIALIZE_ISTREAM_DECL(T)                              \
  rgtl_serialize_istream& operator&(rgtl_serialize_istream& rsr, T& x)
RGTL_SERIALIZE_ISTREAM_DECL(bool);
RGTL_SERIALIZE_ISTREAM_DECL(char);
RGTL_SERIALIZE_ISTREAM_DECL(signed char);
RGTL_SERIALIZE_ISTREAM_DECL(unsigned char);
RGTL_SERIALIZE_ISTREAM_DECL(short);
RGTL_SERIALIZE_ISTREAM_DECL(unsigned short);
RGTL_SERIALIZE_ISTREAM_DECL(int);
RGTL_SERIALIZE_ISTREAM_DECL(unsigned int);
RGTL_SERIALIZE_ISTREAM_DECL(long);
RGTL_SERIALIZE_ISTREAM_DECL(unsigned long);
RGTL_SERIALIZE_ISTREAM_DECL(long long);
RGTL_SERIALIZE_ISTREAM_DECL(unsigned long long);
RGTL_SERIALIZE_ISTREAM_DECL(float);
RGTL_SERIALIZE_ISTREAM_DECL(double);
#undef RGTL_SERIALIZE_ISTREAM_DECL

#endif
