/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_compact_tree_link_hxx
#define rgtl_compact_tree_link_hxx

//:
// \file
// \brief Store a flagged index for rgtl_compact_tree.
// \author Brad King
// \date December 2006

#include <vcl_compiler.h>
#include "rgtl_serialize_access.hxx"
#include <vcl_cstddef.h>

//: Store a flagged index for rgtl_compact_tree.
//
// Several parts of the implementation of rgtl_compact_tree require an
// index with a bit flag.  This class represents the pair efficiently
// using bit field storage in a single integer.
class rgtl_compact_tree_link
{
  VCL_SAFE_BOOL_DEFINE;
public:
  typedef vcl_size_t index_type;

  //: Default-construct with a zero index and false flag.
  rgtl_compact_tree_link(): i_(0), f_(0) {}

  //: Construct given an index and flag value explicitly.
  rgtl_compact_tree_link(index_type i, bool f): i_(i), f_(f) {}

  //: Returns the index portion of the link.
  index_type index() const { return i_; }

  //: Returns the boolean flag portion of the link.
  operator safe_bool () const { return f_? VCL_SAFE_BOOL_TRUE : 0; }

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
