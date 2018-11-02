// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_object_array_triangles_3d_h
#define rgtl_object_array_triangles_3d_h
//:
// \file
// \brief Hold a set of triangles for storage in spatial data structures.
// \author Brad King
// \date March 2007

#include <iostream>
#include <vector>
#include "rgtl_object_array_points.h"

#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class rgtl_serialize_access;

//: Represent a set of triangles for storage in 3d spatial data structures.
//
// This uses the point array structure to hold the triangle vertices
// and provide an interface for storing the point locations.  It
// additionally stores a list of three point ids for each triangle.
class rgtl_object_array_triangles_3d: public rgtl_object_array_points<3>
{
 public:
  typedef rgtl_object_array_points<3> derived;

  //: Construct to hold a given number of points and triangles.
  rgtl_object_array_triangles_3d(int npts = 0, int ntris = 0);

  //: Destruct.
  ~rgtl_object_array_triangles_3d();

  //: Get the point ids of a triangle with the given id.
  void get_triangle(int id, int point_ids[3]) const;

  //: Set the point ids of a triangle with the given id.
  void set_triangle(int id, int const point_ids[3]);

  //: Add a triangle with the given three point ids.
  //  Returns the id of the new triangle.
  int add_triangle(int const point_ids[3]);

  //: Reserve space for adding up to a given number of triangles.
  void reserve_triangles(unsigned int n);

  //: Compute angle-weighted pseudo-normals on the entire mesh.
  void compute_normals();

  //:
  //  Get the angle-weighted pseudo-normal from last closest point
  //  found on the given triangle.  Safe only after all triangles
  //  have been inserted and compute_normals called.
  void get_closest_point_normal(int id, double n[3]) const;

 public:
  //: Get the number of objects in the array.
  virtual int number_of_objects() const;

  //: Check whether one object intersects another.
  virtual bool object_intersects_object(int idA, int idB) const;

  //: Check whether an object intersects an axis-aligned bounding box.
  virtual bool object_intersects_box(int id,
                                     double const center[3],
                                     double const radius,
                                     double const lower[3],
                                     double const upper[3],
                                     double const corners[8][3]) const;

  //: Compute the closest point on an object to the point given.
  virtual bool object_closest_point(int id,
                                    double const x[3],
                                    double y[3],
                                    double bound_squared) const;

  //: Return whether the given ray intersects the given triangle.
  //  Also gets the point of intersection if one exists.
  bool object_intersects_ray(int id,
                             double const origin[3],
                             double const direction[3],
                             double y[3], double* s) const;

 private:

  // Precompute separating axes for each triangle.
  void compute_axes(int id) const;

  // Store the triangle vertex ids.
  typedef vnl_vector_fixed<int, 3> triangle_type;
  std::vector<triangle_type> triangles_;

  // Internal implementation details.
  class pimpl;
  friend class pimpl;
  pimpl* pimpl_;

  friend class rgtl_serialize_access;
  template <class Serializer> void serialize(Serializer& sr);
  template <class Serializer> void serialize_save(Serializer& sr);
  template <class Serializer> void serialize_load(Serializer& sr);
};

#endif
