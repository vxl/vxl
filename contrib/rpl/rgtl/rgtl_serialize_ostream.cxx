#include "rgtl_serialize_ostream.hxx"
//:
// \file
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

//----------------------------------------------------------------------------
bool rgtl_serialize_ostream::okay() const
{
  return this->stream_?true:false;
}

//----------------------------------------------------------------------------
rgtl_serialize_ostream&
rgtl_serialize_ostream::write(void const* data, vcl_size_t length)
{
  this->stream_.write(static_cast<char const*>(data), length);
  if (!this->stream_)
  {
    vcl_cerr << "Error writing to stream!\n";
    vcl_abort();
  }
  return *this;
}

//----------------------------------------------------------------------------
vcl_size_t rgtl_serialize_ostream::position()
{
  return this->stream_.tellp();
}

#define RGTL_SERIALIZE_OSTREAM_DEFN(T)                              \
  rgtl_serialize_ostream& operator&(rgtl_serialize_ostream& rsr, T& x) \
  { return rsr.write(&x, sizeof(x)); }
RGTL_SERIALIZE_OSTREAM_DEFN(bool)
RGTL_SERIALIZE_OSTREAM_DEFN(char)
RGTL_SERIALIZE_OSTREAM_DEFN(signed char)
RGTL_SERIALIZE_OSTREAM_DEFN(unsigned char)
RGTL_SERIALIZE_OSTREAM_DEFN(short)
RGTL_SERIALIZE_OSTREAM_DEFN(unsigned short)
RGTL_SERIALIZE_OSTREAM_DEFN(int)
RGTL_SERIALIZE_OSTREAM_DEFN(unsigned int)
RGTL_SERIALIZE_OSTREAM_DEFN(long)
RGTL_SERIALIZE_OSTREAM_DEFN(unsigned long)
RGTL_SERIALIZE_OSTREAM_DEFN(long long)
RGTL_SERIALIZE_OSTREAM_DEFN(unsigned long long)
RGTL_SERIALIZE_OSTREAM_DEFN(float)
RGTL_SERIALIZE_OSTREAM_DEFN(double)
#undef RGTL_SERIALIZE_OSTREAM_DEFN
