#include <iostream>
#include <cstddef>
#include "rgtl_object_once.h"
//:
// \file
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "rgtl_serialize_access.h"
#include "rgtl_serialize_split.h"
#include "rgtl_serialize_istream.h"
#include "rgtl_serialize_ostream.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//----------------------------------------------------------------------------
rgtl_object_once::rgtl_object_once(): marks_(), mark_(0)
{
}

//----------------------------------------------------------------------------
rgtl_object_once::rgtl_object_once(size_type n): marks_(n, 0), mark_(0)
{
}

//----------------------------------------------------------------------------
void rgtl_object_once::set_number_of_objects(size_type n)
{
  this->mark_ = 0;
  this->marks_.resize(0);
  this->marks_.resize(n, 0);
}

//----------------------------------------------------------------------------
void rgtl_object_once::reset() const
{
  // Rollover the counter if necessary.
  if (this->mark_ == 0xFFFFFFFF)
  {
    std::size_t s = this->marks_.size();
    this->mark_ = 0;
    this->marks_.resize(0);
    this->marks_.resize(s, 0);
  }

  // Increment the counter.
  ++this->mark_;
}

//----------------------------------------------------------------------------
bool rgtl_object_once::visit(size_type i) const
{
  if (this->marks_[i] != this->mark_)
  {
    this->marks_[i] = this->mark_;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
bool rgtl_object_once::visited(size_type i) const
{
  return this->marks_[i] == this->mark_;
}

//----------------------------------------------------------------------------
template <class Serializer>
void rgtl_object_once::serialize(Serializer& sr)
{
  rgtl_serialize_split(sr, *this);
}

//----------------------------------------------------------------------------
template <class Serializer>
void rgtl_object_once::serialize_load(Serializer& sr)
{
  size_type sz;
  sr >> sz;
  this->set_number_of_objects(sz);
}

//----------------------------------------------------------------------------
template <class Serializer>
void rgtl_object_once::serialize_save(Serializer& sr)
{
  size_type sz = this->marks_.size();
  sr << sz;
}

#define RGTL_OBJECT_ONCE_SERIALIZE_INSTANTIATE(T) \
  template void rgtl_object_once::serialize<T >(T&)
RGTL_OBJECT_ONCE_SERIALIZE_INSTANTIATE(rgtl_serialize_istream);
RGTL_OBJECT_ONCE_SERIALIZE_INSTANTIATE(rgtl_serialize_ostream);
