#ifndef rgtl_octree_objects_h
#define rgtl_octree_objects_h
//:
// \file
// \brief Store a set of objects in an octree for efficient spatial queries.
// \author Brad King
// \date March 2007
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class rgtl_serialize_access;

template <unsigned int D> class rgtl_octree_cell_bounds;
template <unsigned int D> class rgtl_object_array;

template <unsigned int D> class rgtl_octree_objects_internal;

//: Store a fixed set of objects in a spatial structure for efficient lookup.
//
// Given an array of objects with a few basic operations defined by
// rgtl_object_array, stores the objects in an efficient octree-based
// spatial structure.  The structure supports several spatial queries
// on the set of objects.  It features a distance transform that
// efficiently pre-computes distances from all octree leaf centers to
// their nearest object points.  This is used to significantly reduce
// the initial sphere radius for closest object queries.
template <unsigned int D>
class rgtl_octree_objects
{
 public:
  typedef rgtl_object_array<D> object_array_type;
  typedef rgtl_octree_cell_bounds<D> bounds_type;

  //: Construct with a set of objects, the region of interest, and a maximum subdivision level.
  rgtl_octree_objects(object_array_type const& objs,
                      bounds_type const& b, int ml);

  //: Default constructor should be used only just before loading a previously serialized instance.
  rgtl_octree_objects(object_array_type const& oa);

  //: Destruct.
  ~rgtl_octree_objects();

  //: Query the given hyper-sphere for objects its volume intersects.
  //  Returns the number of objects found.
  int query_sphere(double const center[D], double radius,
                   std::vector<int>& ids) const;

  //: Query the given object for other objects its volume intersects.
  //  Returns the number of objects found.  Note that the given id is
  //  treated opaquely and passed back to the object_intersects_box
  //  and object_intersects_object callbacks.  It may therefore be out
  //  of range which is useful in querying the objects with another
  //  object not in the set.
  int query_object(int id, std::vector<int>& ids) const;

  //: Query the k closest objects to the given point.
  //  Returns the number of objects found.  Any combination of the object ids,
  //  squared distances, and closest point locations may be obtained.
  //  Pass null pointers to for the results not desired.  If a
  //  non-negative value is given for bound_squared no objects outside
  //  the squared distance bound will be returned.  This optionally
  //  limits the search to a user-specified sphere.
  int query_closest(double const p[D], int k, int* ids,
                    double* squared_distances, double* points,
                    double bound_squared = -1) const;

  //: Compute the nth-order distance transform on the leaf cell centers.
  //  This speeds up query_closest for query points inside the
  //  bounds and k<=n by providing a smaller initial bound.
  bool compute_distance_transform(int n = 1) const;

  //: Enable/Disable query_closest debug output if support is compiled in.
  void set_query_closest_debug(bool b);

 private:
  // Internal implementation details.
  typedef rgtl_octree_objects_internal<D> internal_type;
  internal_type* internal_;

  friend class rgtl_serialize_access;
  template <class Serializer> void serialize(Serializer& sr);
};

#endif // rgtl_octree_objects_h
