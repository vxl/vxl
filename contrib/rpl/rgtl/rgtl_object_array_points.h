/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_object_array_points_h
#define rgtl_object_array_points_h

//:
// \file
// \brief Hold a set of points for storage in spatial data structures.
// \author Brad King
// \date March 2007

#include <iostream>
#include <vector>
#include "rgtl_object_array.h"
#include "rgtl_serialize_access.h"
#include "rgtl_serialize_base.h"
#include "rgtl_serialize_stl_vector.h"

#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//----------------------------------------------------------------------------
template <unsigned int D>
class rgtl_object_array_points: public rgtl_object_array<D>
{
  typedef vnl_vector_fixed<double, D> point_type;
public:
  typedef rgtl_object_array<D> derived;

  //: Construct to hold a given number of points.
  rgtl_object_array_points(int n = 0);

  //: Construct to hold the points given.
  rgtl_object_array_points(std::vector<point_type> const& points);

  //: Set the number of points stored.
  void set_number_of_points(int n);

  //: Get the number of points stored.
  int get_number_of_points() const;

  //: Get a point location in space.
  void get_point(int id, double x[D]) const;

  //: Set a point location in space.
  void set_point(int id, double const x[D]);

  //: Add a point location in space.  Returns the id of the point.
  int add_point(double const x[D]);

  //: Get the number of objects in the array.
  virtual int number_of_objects() const;

  //: Check whether one object intersects another.
  virtual bool object_intersects_object(int idA, int idB) const;

  //: Check whether an object intersects an axis-aligned bounding box.
  virtual bool object_intersects_box(int id,
                                     double const center[D],
                                     double const radius,
                                     double const lower[D],
                                     double const upper[D],
                                     double const corners[1<<D][D]) const;

  //: Compute the closest point on an object to the point given.
  virtual bool object_closest_point(int id,
                                    double const x[D],
                                    double y[D],
                                    double bound_squared) const;

  //: Compute the intersection of an object with a ray.
  virtual bool object_intersects_ray(int id,
                                     double const origin[D],
                                     double const direction[D],
                                     double y[D], double* s) const;

  //: Compute an axis-aligned bounding box around the objects.
  virtual void compute_bounds(double bounds[D][2]) const;
private:
  std::vector<point_type> points_;

  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
    {
    sr & rgtl_serialize_base<derived>(*this);
    sr & points_;
    }
};


#endif
