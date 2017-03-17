/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_octree_cell_geometry_h
#define rgtl_octree_cell_geometry_h

//:
// \file
// \brief Represent the geometry of an octree cell.
// \author Brad King
// \date February 2007

#include "rgtl_octree_cell_location.h"

template <unsigned int D> class rgtl_octree_cell_bounds;

//: Represent the geometry of an octree cell.
//
// This class efficiently maintains octree cell geometry as the tree
// is traversed.
template <unsigned int D>
class rgtl_octree_cell_geometry
{
public:
  //: Type representing a logical cell location.
  typedef rgtl_octree_cell_location<D> cell_location_type;

  //: Type representing the octree cell bounds.
  typedef rgtl_octree_cell_bounds<D> cell_bounds_type;

  //: Default constructor used only to create array.
  rgtl_octree_cell_geometry();

  //: Construct the geometry of a given cell location.
  explicit rgtl_octree_cell_geometry(cell_location_type const& cell,
                                     cell_bounds_type const& root_bounds);

  //: Get the logical cell location.
  cell_location_type const& location() const;

  //: Get the cell bounding sphere.
  void get_sphere(double center[D], double& radius) const;
  double const* get_sphere_center() const;
  double get_sphere_radius() const;

  //: Get the cell bounding box range on each axis.
  double const* get_lower() const;
  double const* get_upper() const;

  //: Get the cell bounding box corners.
  double const* get_corner(unsigned int i) const;
  double const (*get_corners() const)[D] { return this->corners_; }

  //: Compute the child cell geometries.
  void get_children(rgtl_octree_cell_geometry children[1<<D]) const;

private:

  void compute_corners();

  // Type-safe index to a child.
  typedef typename cell_location_type::child_index_type child_index_type;

  // The cell location.
  cell_location_type location_;

  // The cell geometry.
  double center_[D];        // Bounding sphere center.
  double radius_;           // Bounding sphere radius.
  double lower_[D];         // Lower bounding plane positions.
  double upper_[D];         // Upper bounding plane positions.
  double corners_[1<<D][D]; // Bounding box corners.
};

#endif
