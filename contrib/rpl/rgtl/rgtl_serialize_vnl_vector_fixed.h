/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_serialize_vnl_vector_fixed_h
#define rgtl_serialize_vnl_vector_fixed_h

//:
// \file
// \brief Implement serialization for vnl_vector_fixed.
// \author Brad King
// \date February 2008

#include <vnl/vnl_vector_fixed.h>

template <class Serializer, typename T, unsigned int N>
void rgtl_serialize(Serializer& sr, vnl_vector_fixed<T,N>& self)
{
  for(unsigned int i=0; i < N; ++i)
    {
    sr & self(i);
    }
}

#endif
