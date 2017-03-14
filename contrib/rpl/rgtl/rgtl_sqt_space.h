#ifndef rgtl_sqt_space_h
#define rgtl_sqt_space_h
//:
// \file
// \brief Spherical space parameterization.
// \author Brad King
// \date April 2007
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "rgtl_config.h"
#include "rgtl_static_assert.h"

//: Implement methods not specific to a particular face.
template <unsigned int D>
class rgtl_sqt_space_base
{
 public:
  //: Get the canonical cube face containing the given direction.
  static unsigned int direction_to_face(double const d[D]);
};

//: Parameterize a D-dimensional hypersphere with an axis-aligned hypercube.
template <unsigned int D, unsigned int Face>
class rgtl_sqt_space: public rgtl_sqt_space_base<D>
{
 public:
  //: The embedding dimension of the hypersphere.
  RGTL_STATIC_CONST(unsigned int, dimension = D);

  //: The index of the face.
  RGTL_STATIC_CONST(unsigned int, face = Face);

  //: The index of the axis normal to the face.
  RGTL_STATIC_CONST(unsigned int, face_axis = Face>>1);

  //: The side of the origin on which the face lies along the axis.
  //  The negative side is 0 and the positive side is 1.
  RGTL_STATIC_CONST(unsigned int, face_side = Face&1);

  //: Convert from parameters on the current face to a full direction.
  static void parameters_to_direction(double const u[D-1], double d[D]);

  //: Convert from a full direction to parameters on the current face.
  //  The component of the input direction corresponding to the
  //  face_axis must be the largest magnitude component, and its sign
  //  must be consistent with face_side.
  static void direction_to_parameters(double const d[D], double u[D-1]);

  //: Compute a normal to the isoplane on which the j-th parameter is constant at the given value.
  //  The normal points in the direction of increasing parameter value.
  static void isoplane_normal(unsigned int j, double u, double n[D]);

 private:
  // Make sure the face index is in range.
  RGTL_STATIC_ASSERT(Face < (D<<1));
};

#endif // rgtl_sqt_space_h
