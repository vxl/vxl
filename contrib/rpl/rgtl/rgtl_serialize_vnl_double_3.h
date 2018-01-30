/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_serialize_vnl_double_3_h
#define rgtl_serialize_vnl_double_3_h

//:
// \file
// \brief Implement serialization for vnl_double_3.
// \author Brad King
// \date February 2008

#include "rgtl_serialize_vnl_vector_fixed.h"
#include "rgtl_serialize_base.h"

#include <vnl/vnl_double_3.h>

template <class Serializer>
void rgtl_serialize(Serializer& sr, vnl_double_3& self)
{
  sr & rgtl_serialize_base< vnl_vector_fixed<double,3> >(self);
}

#endif
