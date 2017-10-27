#ifndef rgtl_octree_cell_bounds_h
#define rgtl_octree_cell_bounds_h
//:
// \file
// \brief Represent the bounding box for an octree cell in D dimensions.
// \author Brad King
// \date February 2007
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <vnl/vnl_vector_fixed.h>

#include "rgtl_serialize_access.h"
#include "rgtl_serialize_vnl_vector_fixed.h"

template <unsigned int D> class rgtl_octree_cell_location;

//: Represent an axis-aligned bounding hypercube for a cell.
template <unsigned int D>
class rgtl_octree_cell_bounds
{
 public:
  //: Type used to represent the hypercube origin.
  typedef vnl_vector_fixed<double, D> point_type;

  //: Construct with a unit hypercube.
  rgtl_octree_cell_bounds();

  //: Construct with the origin and size (edge length) of the hypercube.
  rgtl_octree_cell_bounds(double const o[D], double s);
  rgtl_octree_cell_bounds(point_type const& o, double s);

  //: Set the origin and size (edge length) of the hypercube.
  void set_bounds(double const o[D], double s);
  void set_bounds(point_type const& o, double s);

  //:
  //  Compute a bounding hypercube enclosing the given axis-aligned
  //  bounding box with the longest axis scaled by the given factor.
  //  The resulting cube will be centered around the original bounding
  //  box along each axis.
  void compute_bounds(double const (&bds)[D][2], double factor = 1);
  void compute_bounds(double const bds[D*2], double factor = 1);

  //: Compute the bounding hypercube for the given cell in a tree with the given root cell bounds.
  void compute_bounds(rgtl_octree_cell_bounds<D> const& root_bounds,
                      rgtl_octree_cell_location<D> const& cell);

  //: Determine whether the bounding hypercube contains the given point.
  bool contains(double const p[D]) const;
  bool contains(point_type const& p) const;

  //: Set the edge length of the hypercube.
  void size(double s) { this->size_ = s; }

  //: Set the lower point of the hypercube.
  void origin(point_type const& o) { this->origin_ = o; }

  //: Get the edge length of the hypercube.
  double size() const { return this->size_; }

  //: Get the lower point of the hypercube.
  point_type const& origin() const { return this->origin_; }
  double origin(unsigned int i) const { return this->origin_[i]; }

 private:
  point_type origin_;
  double size_;

  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr& origin_;
    sr& size_;
  }
};

#endif // rgtl_octree_cell_bounds_h
