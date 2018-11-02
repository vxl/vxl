/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_compact_tree_link_h
#define rgtl_compact_tree_link_h

//:
// \file
// \brief Store a flagged index for rgtl_compact_tree.
// \author Brad King
// \date December 2006

#include <iostream>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "rgtl_serialize_access.h"

//: Store a flagged index for rgtl_compact_tree.
//
// Several parts of the implementation of rgtl_compact_tree require an
// index with a bit flag.  This class represents the pair efficiently
// using bit field storage in a single integer.
class rgtl_compact_tree_link
{

public:
  typedef std::size_t index_type;

  //: Default-construct with a zero index and false flag.
  rgtl_compact_tree_link(): i_(0), f_(0) {}

  //: Construct given an index and flag value explicitly.
  rgtl_compact_tree_link(index_type i, bool f): i_(i), f_(f) {}

  //: Returns the index portion of the link.
  index_type index() const { return i_; }

  //: Returns the boolean flag portion of the link.
  /* The old 'safe_bool' did implicit conversions, best practice would be to use explicit operator bool */
  operator bool () const { return f_? true : false; }

  //: Returns the inverse of the boolean flag portion of the link.
  bool operator!() const { return !f_; }

private:
  index_type i_: sizeof(index_type)*8 - 1;
  index_type f_: 1;

  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
    {
    // TODO: Convert the members to a union with a struct for the
    // bitfields.
    index_type& data = reinterpret_cast<index_type&>(*this);
    sr & data;
    }
};

#endif
