#ifndef rgtl_sqt_object_array_triangles_3d_h
#define rgtl_sqt_object_array_triangles_3d_h
//:
// \file
// \brief Represent triangles in 3d during SQT construction.
// \author Brad King
// \date April 2007
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "rgtl_sqt_object_array.h"

class rgtl_object_array_triangles_3d;

//: Represent the triangles from a rgtl_object_array_triangles_3d during SQT construction.
//
// During construction of a SQT triangles are split into polygons by
// clipping them against SQT cell boundaries.  This class efficiently
// stores them.
class rgtl_sqt_object_array_triangles_3d: public rgtl_sqt_object_array<3>
{
 public:
  //: The type from which this class derives.
  typedef rgtl_sqt_object_array<3> derived;

  //: The original object array type.
  typedef rgtl_object_array_triangles_3d object_array_type;

  //: Construct from an original array of triangles.
  rgtl_sqt_object_array_triangles_3d(object_array_type const& oa);

  //: Get the set of triangles in the given SQT face.
  virtual sqt_object_set_ptr new_set(double const origin[3],
                                     unsigned int face) const;

  //: Get the original array of triangles.
  virtual derived::object_array_type const& original() const;

 private:
  //: Reference the original array of triangles.
  object_array_type const& original_;
};

#endif // rgtl_sqt_object_array_triangles_3d_h
