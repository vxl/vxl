/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_tokens_hxx
#define rtvl_tokens_hxx

#include "rtvl_tensor.hxx"

#include <rgtl/rgtl_object_array_points.hxx>

#include <vcl_vector.h>

#include <rgtl/rgtl_serialize_access.hxx>
#include <rgtl/rgtl_serialize_base.hxx>
#include <rgtl/rgtl_serialize_stl_vector.hxx>

template <unsigned int N>
class rtvl_tokens
{
public:
  /** Scale at which analysis is performed.  */
  double scale;

  /** Input token locations.  */
  rgtl_object_array_points<N> points;

  /** Refined tokens ready for dense analysis at this scale.  */
  vcl_vector< rtvl_tensor<N> > tokens;

private:
  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
    {
    sr & scale;
    sr & points;
    sr & tokens;
    }
};

#endif
