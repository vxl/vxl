/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_sqt_cell_geometry_h
#define rgtl_sqt_cell_geometry_h

//:
// \file
// \brief Represent the geometry of a spherical quad-tree cell.
// \author Brad King
// \date February 2007

#include "rgtl_sqt_cell_location.h"

template <unsigned int D, unsigned int Face> class rgtl_sqt_space;

//: Represent the geometry of a spherical quad-tree cell.
//
// This class efficiently maintains SQT cell geometry as the tree
// is traversed.
template <unsigned int D, unsigned int Face>
class rgtl_sqt_cell_geometry
{
public:
  //: Type representing a logical cell location.
  typedef rgtl_sqt_cell_location<D> cell_location_type;

  //: Construct the geometry of a given cell location.
  explicit rgtl_sqt_cell_geometry(cell_location_type const& cell);

  //: Get the logical cell location.
  cell_location_type const& location() const;

  //: Get the cell bounding cone axis and opening angle.
  void get_cone(float axis[D], float& angle) const;
  void get_cone(double axis[D], double& angle) const;
  double const* get_cone_axis() const;
  double get_cone_angle() const;

  //: Get the edge directions.
  void get_wedge_edge(unsigned int edge_index, double edge[D]) const;
  double const* get_wedge_edge(unsigned int edge_index) const;

  //: Compute the cell center plane normals.
  void get_center_planes(double center_normals[D-1][D]) const;

  //: Compute geometry of all children of a cell.
  class children_type
  {
  public:
    //: The type representing the parent and child cells.
    typedef rgtl_sqt_cell_geometry<D, Face> cell_geometry_type;

    //: Type-safe index of child.
    typedef typename cell_location_type::child_index_type child_index_type;

    //: Construct with the parent whose children are to be computed.
    children_type(cell_geometry_type const& parent)
      { parent.get_children(this->children); }

    //: Index the child geometries.
    cell_geometry_type& operator[](child_index_type c)
      { return this->children[c]; }
    cell_geometry_type const& operator[](child_index_type c) const
      { return this->children[c]; }
  private:
    children_type(); // not implemented

    // Store the child array.
    cell_geometry_type children[1<<(D-1)];
  };

private:
  // Support for children_type.
  rgtl_sqt_cell_geometry();
  void get_children(rgtl_sqt_cell_geometry children[(1<<(D-1))]) const;
  void get_children(rgtl_sqt_cell_geometry children[(1<<(D-1))],
                    double const center_normals[D-1][D]) const;
  friend class children_type;

  // Internal methods.
  void compute_cone();

  // The SQT spherical space parameterization for this face.
  typedef rgtl_sqt_space<D, Face> space;

  // Type-safe index to a child.
  typedef typename cell_location_type::child_index_type child_index_type;

  // The cell location.
  cell_location_type location_;

  // The cell geometry.
  double axis_[D];            // Bounding cone axis direction.
  double angle_;              // Bounding cone opening angle.
  double lower_[D-1][D];      // Lower bounding plane normals.
  double upper_[D-1][D];      // Upper bounding plane normals.
  double edges_[1<<(D-1)][D]; // Wedge edge ray directions.
};

#endif
