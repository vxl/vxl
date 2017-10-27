/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt) */

#ifndef rgtl_object_array_h
#define rgtl_object_array_h
//:
// \file
// \brief Abstract interface for an array of objects for spatial structures.
// \author Brad King
// \date March 2007

#include "rgtl_serialize_access.h"

//: Abstract interface for an array of objects in D dimensions.
//
// This interface is meant for use in defining a set of objects for
// storage in a spatial structure.  A few simple operations are
// required.  The objects may be of heterogeneous type so long as the
// operations are implemented properly for each object type.
template <unsigned int D>
class rgtl_object_array
{
 public:
  //: Virtual destructor.
  virtual ~rgtl_object_array() {}

  //: Get the number of objects in the array.
  virtual int number_of_objects() const = 0;

  //: Check whether one object intersects another.
  virtual bool object_intersects_object(int idA, int idB) const = 0;

  //: Check whether an object intersects an axis-aligned bounding box.
  virtual bool object_intersects_box(int id,
                                     double const center[D],
                                     double const radius,
                                     double const lower[D],
                                     double const upper[D],
                                     double const corners[1<<D][D]) const = 0;

  //: Compute the closest point on an object to the point given.
  //  Returns whether a closest point was computed and stored.
  //  If the closest point is farther than the given squared distance
  //  bound from the query point an implementation may optionally
  //  choose to skip computation of the exact closest point and return
  //  false.  A negative squared distance bound is equivalent to an
  //  infinite bound.
  virtual bool object_closest_point(int id,
                                    double const x[D],
                                    double y[D],
                                    double bound_squared) const = 0;

  //: Compute the intersection of an object with a ray.
  //  If an intersection exists true is returned, "y" is set to the
  //  intersection point, and "s" is set to the scale between the
  //  vectors "direction" and "y - origin".  Otherwise false is
  //  returned.
  virtual bool object_intersects_ray(int id,
                                     double const origin[D],
                                     double const direction[D],
                                     double y[D], double* s) const = 0;

  //: Compute an axis-aligned bounding box around the objects.
  virtual void compute_bounds(double bounds[D][2]) const = 0;
 private:
  friend class rgtl_serialize_access;
  template <class Serializer> void serialize(Serializer&) {}
};

#endif
