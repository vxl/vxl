/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt) */

#ifndef rgtl_serialize_ostream_hxx
#define rgtl_serialize_ostream_hxx
//:
// \file
// \brief Save rgtl objects to a stream.
// \author Brad King
// \date February 2008

#include "rgtl_serialize_direction.hxx"

#include <vcl_compiler.h>
#include <vcl_iosfwd.h>
#include <vcl_cstddef.h>

//: Serialization save object that writes to a stream.
//  See rgtl_serialize.txt for serialization interface details.
class rgtl_serialize_ostream
{
  VCL_SAFE_BOOL_DEFINE;
 public:
  //: Mark this class as a serialization save direction.
  typedef rgtl_serialize_direction_save serialize_direction;

  //: Construct with a stream to which to write data.
  rgtl_serialize_ostream(vcl_ostream& is): stream_(is) {}

  //: Safe conversion to a boolean type.
  operator safe_bool() const
  { return this->okay()? VCL_SAFE_BOOL_TRUE : 0; }

  //: Write data to the output stream.
  rgtl_serialize_ostream& write(void const* data, vcl_size_t length);

  //: Get the current position in the stream.
  vcl_size_t position();
 private:
  bool okay() const;
  vcl_ostream& stream_;
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
