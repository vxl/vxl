#ifndef rgtl_tagged_index_h
#define rgtl_tagged_index_h
//:
// \file
// \brief Unsigned integer wrapper for type-safe indexing
// \author Brad King
// \date December 2006
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <cstddef>
#include "rgtl_serialize_access.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Wraps an unsigned integer for use as an index.
//  The index is compatible only with other indexes using the same tag.
//  Implicit conversions are disallowed.  This provides type-safe indexing.
template <typename Tag>
class rgtl_tagged_index
{
 public:
  //: The underlying index type.
  typedef std::size_t index_type;

  //: Default construct to a zero index.
  rgtl_tagged_index(): value_(0) {}

  //: Explicitly construct from an integer index.
  explicit rgtl_tagged_index(index_type v): value_(v) {}

  //: Implicitly retrieve the integer index value.
  operator index_type() const { return value_; }

  //: Wrap some standard integer operators.
  rgtl_tagged_index& operator++() { ++value_; return *this; }
  rgtl_tagged_index& operator*=(index_type r)
  {
    value_ *= r;
    return *this;
  }
  rgtl_tagged_index& operator|=(index_type r)
  {
    value_ |= r;
    return *this;
  }
  friend rgtl_tagged_index operator-(rgtl_tagged_index const& l, int r)
  {
    return rgtl_tagged_index(l.value_ - r);
  }

 private:
  index_type value_;

  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr& value_;
  }
};

#endif // rgtl_tagged_index_h
