#include "rgtl_serialize_istream.hxx"
//:
// \file
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <vcl_iostream.h>
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>

//----------------------------------------------------------------------------
rgtl_serialize_istream
::rgtl_serialize_istream(vcl_istream& is):
  stream_(is), buffer_left_(0), buffer_used_(0)
{
}

//----------------------------------------------------------------------------
bool rgtl_serialize_istream::okay() const
{
  return this->stream_?true:false;
}

//----------------------------------------------------------------------------
rgtl_serialize_istream&
rgtl_serialize_istream::read(void* vdata, vcl_size_t length)
{
  char* data = static_cast<char*>(vdata);
  while (length > 0)
  {
    // If our buffer is empty, read more data from the stream.
    if (this->buffer_left_ == 0)
    {
      if (!this->stream_.read(this->buffer_, buffer_size))
      {
        if (this->stream_.gcount() > 0)
        {
          this->stream_.clear(this->stream_.rdstate() & ~vcl_ios::failbit);
        }
        else
        {
          vcl_cerr << "Error reading from stream (1)!\n";
          vcl_abort();
        }
      }
      this->buffer_used_ = 0;
      this->buffer_left_ = this->stream_.gcount();
    }

    // Copy data from the buffer.
    vcl_size_t s = this->buffer_left_ < length ? this->buffer_left_ : length;
    vcl_memcpy(data, this->buffer_+this->buffer_used_, s);
    this->buffer_used_ += s;
    this->buffer_left_ -= s;
    data += s;
    length -= s;
  }
  return *this;
}

//----------------------------------------------------------------------------
vcl_size_t rgtl_serialize_istream::position()
{
  return this->stream_.tellg();
}

#define RGTL_SERIALIZE_ISTREAM_DEFN(T)                              \
  rgtl_serialize_istream& operator&(rgtl_serialize_istream& rsr, T& x) \
  { return rsr.read(&x, sizeof(x)); }
RGTL_SERIALIZE_ISTREAM_DEFN(bool)
RGTL_SERIALIZE_ISTREAM_DEFN(char)
RGTL_SERIALIZE_ISTREAM_DEFN(signed char)
RGTL_SERIALIZE_ISTREAM_DEFN(unsigned char)
RGTL_SERIALIZE_ISTREAM_DEFN(short)
RGTL_SERIALIZE_ISTREAM_DEFN(unsigned short)
RGTL_SERIALIZE_ISTREAM_DEFN(int)
RGTL_SERIALIZE_ISTREAM_DEFN(unsigned int)
RGTL_SERIALIZE_ISTREAM_DEFN(long)
RGTL_SERIALIZE_ISTREAM_DEFN(unsigned long)
RGTL_SERIALIZE_ISTREAM_DEFN(long long)
RGTL_SERIALIZE_ISTREAM_DEFN(unsigned long long)
RGTL_SERIALIZE_ISTREAM_DEFN(float)
RGTL_SERIALIZE_ISTREAM_DEFN(double)
#undef RGTL_SERIALIZE_ISTREAM_DEFN
