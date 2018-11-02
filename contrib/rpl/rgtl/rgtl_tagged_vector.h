#ifndef rgtl_tagged_vector_h
#define rgtl_tagged_vector_h
//:
// \file
// \brief STL vector wrapper with type-safe indexing
// \author Brad King
// \date December 2006
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <vector>
#include "rgtl_tagged_index.h"
#include "rgtl_serialize_access.h"
#include "rgtl_serialize_base.h"
#include "rgtl_serialize_split.h"
#include "rgtl_serialize_stl_vector.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Wrap an STL vector and replace the indexing operators with tagged-indexing equivalents.
template <typename Tag, typename T>
class rgtl_tagged_vector: protected std::vector<T>
{
  // TODO: Use a mmap-ed file to allow very large vectors.
  typedef std::vector<T> derived;
 public:
  //: Allow access to standard vector types.
  typedef typename derived::size_type size_type;
  typedef typename derived::value_type value_type;
  typedef typename derived::iterator iterator;
  typedef typename derived::const_iterator const_iterator;
  typedef typename derived::reference reference;
  typedef typename derived::const_reference const_reference;

  //: Define the indexing type.
  typedef rgtl_tagged_index<Tag> index_type;

  //: Provide standard vector constructors.
  rgtl_tagged_vector(): derived() {}
  rgtl_tagged_vector(unsigned int length,
                     T const& v = T()): derived(length, v) {}

  //: Indexing operators require the tagged index type.
  reference operator[](index_type i)
  { return derived::operator[](i); }
  const_reference operator[](index_type i) const
  { return derived::operator[](i); }

  //: Allow access to standard vector operations.
  void push_back(T const& v) { derived::push_back(v); }
  size_type size() const { return derived::size(); }
  void resize(index_type s) { return derived::resize(size_type(s)); }
  iterator begin() { return derived::begin(); }
  iterator end() { return derived::end(); }
  const_iterator begin() const { return derived::begin(); }
  const_iterator end() const { return derived::end(); }
  void erase(iterator first, iterator last) { derived::erase(first, last); }
  void clear() { derived::clear(); }
 private:
  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr& rgtl_serialize_base<derived>(*this);
  }
};

#endif // rgtl_tagged_vector_h
