#ifndef rgtl_sqt_objects_h
#define rgtl_sqt_objects_h
//:
// \file
// \brief Store objects in a star-shaped region for spatial queries.
// \author Brad King
// \date April 2007
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <memory>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class rgtl_serialize_access;

class rgtl_child_index_tag;
template <typename Tag> class rgtl_tagged_index;
template <unsigned int D> class rgtl_object_array;
template <unsigned int D> class rgtl_sqt_cell_location;
template <unsigned int D> class rgtl_sqt_object_array;
template <unsigned int D> class rgtl_sqt_objects_internal;
template <unsigned int D> class rgtl_compact_tree_index;
template <class T, unsigned int n> class vnl_vector_fixed;

//: Store objects distributed in a star-shaped region for spatial queries.
//
// When a set of objects are mostly visible from a single point, they
// are said to be distributed over a star-shaped region.  An efficient
// spatial structure can be built using a quad-tree on the spherical
// parameter space.
template <unsigned int D>
class rgtl_sqt_objects
{
 public:
  //: Type representing a logical octree cell index.
  typedef rgtl_sqt_cell_location<D> cell_location_type;

  //: Type representing a physical octree cell index.
  typedef rgtl_compact_tree_index<D-1> cell_index_type;

  //: Type safe index of cell children.
  typedef rgtl_tagged_index<rgtl_child_index_tag> child_index_type;

  //: Type representing an object array.
  typedef rgtl_object_array<D> object_array_type;

  //: Type representing an object array specifically for building a SQT.
  typedef rgtl_sqt_object_array<D> sqt_object_array_type;

  //: Default constructor should be used only just before loading a previously serialized instance.
  rgtl_sqt_objects(object_array_type const& oa);

  //:
  //  Construct with a set of objects, the star-shape guard location,
  //  a maximum subdivision level, and a maximum number of objects per
  //  leaf.
  rgtl_sqt_objects(sqt_object_array_type const& soa,
                   double const origin[D], int ml, int mpl);

  //:
  //  Build with a set of objects, the star-shape guard location,
  //  a maximum subdivision level, and a maximum number of objects per
  //  leaf.
  void build(sqt_object_array_type const& soa,
             double const origin[D], int ml, int mpl);

  //: Destruct.
  ~rgtl_sqt_objects();

  //: Query a ray emanating from the SQT origin in the given direction.
  //  If the ray intersects an object, the location of the closest
  //  intersection point is computed and stored in "x", the scale
  //  between "d" and "x - origin" is stored in "s" (x=o+d*s), and true is
  //  returned.  Otherwise false is returned.
  bool query_ray(double const d[D], double x[D], double* s) const;

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

  //: Enable/Disable query_closest debug output if support is compiled in.
  void set_query_closest_debug(bool b);

  //: Get the origin at which the SQT is centered.
  double origin(unsigned int a) const;
  vnl_vector_fixed<double,D> const& origin() const;

  //: Get a bounding box around the entire volume represented by the SQT.
  void compute_bounds(double bounds[D][2]) const;

  //: Access the internal octree representation.
  bool has_children(unsigned int face, cell_index_type cell_index) const;
  cell_index_type get_child(unsigned int face, cell_index_type cell_index,
                            child_index_type child) const;
  bool get_depth_range(unsigned int face, cell_index_type cell_index,
                       double& depth_min, double& depth_max) const;

 private:
  // Internal implementation details.
  typedef rgtl_sqt_objects_internal<D> internal_type;
  std::unique_ptr<internal_type> internal_;

  friend class rgtl_serialize_access;
  template <class Serializer> void serialize(Serializer& sr);
};

#endif // rgtl_sqt_objects_h
