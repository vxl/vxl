// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_octree_objects_hxx
#define rgtl_octree_objects_hxx

#include <iostream>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <stack>
#include <vector>
#include "rgtl_octree_objects.h"

#include "rgtl_object_array.h"
#include "rgtl_object_once.h"
#include "rgtl_octree_cell_bounds.hxx"
#include "rgtl_octree_cell_geometry.hxx"
#include "rgtl_octree_cell_location.hxx"
#include "rgtl_octree_data_fixed.hxx"
#include "rgtl_serialize_access.h"
#include "rgtl_serialize_base.h"
#include "rgtl_serialize_istream.h"
#include "rgtl_serialize_ostream.h"

#include <vnl/vnl_math.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// TODO: During tree construction we should check the set of objects
// in the current node.  Those with no boundary inside the node should
// be left out of the count of objects remaining because no amount of
// subdivision will reduce the count.

//#define RGTL_OCTREE_OBJECTS_DEBUG_BUILD
//#define RGTL_OCTREE_OBJECTS_DEBUG_QUERY
//#define RGTL_OCTREE_OBJECTS_DEBUG_FRONT

#if defined(RGTL_OCTREE_OBJECTS_DEBUG_BUILD) || \
    defined(RGTL_OCTREE_OBJECTS_DEBUG_QUERY) || \
    defined(RGTL_OCTREE_OBJECTS_DEBUG_FRONT) || 0
# include <std::iostream.h>
#endif

// Compile-time option to store distances at node centers and using
// them during recursion to establish smaller query radius bounds.  It
// seems like this should help for query points outside the root cell
// but is slower for query points inside the root cell because the
// overhead of computing the bound for every node on the way down the
// tree is too great compared to just looking up the initial leaf bound.
//#define RGTL_OCTREE_OBJECTS_NODE_DT

//----------------------------------------------------------------------------
struct rgtl_octree_objects_cell_data
{
  // Distance from the cell center to the nearest object.  NOTE:
  // During tree construction and distance transform this could be set
  // to a tentative bound.  We could use the sign bit to indicate this
  // case.  Negative values would be tentative bounds and positive
  // values final distances.  The initial bound for an empty cell
  // would be -inf, and for a non-empty cell half the cell diagonal.
  double distance;

  // TODO: Instead of using rgtl_octree_data_fixed, setup a custom
  // node/leaf data policy so that a variable number of distance
  // values can be stored for each cell.  This will allow a kth-order
  // distance transform to be computed by the user so that it can be
  // used for initial bounds with k > 1 for query_closest.

  rgtl_octree_objects_cell_data() {}
  rgtl_octree_objects_cell_data(double d): distance(d) {}
 private:
  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr & distance;
  }
};

//----------------------------------------------------------------------------
struct rgtl_octree_objects_leaf_data: public rgtl_octree_objects_cell_data
{
  typedef rgtl_octree_objects_cell_data derived;
  typedef int index_type;

  // Objects in this leaf are stored contiguously in an array.  This
  // is the index of the beginning and (one past) the end of the block
  // of object ids.  If index_begin == index_end the cell has no
  // objects.
  index_type index_begin;
  index_type index_end;

  rgtl_octree_objects_leaf_data() {}
  rgtl_octree_objects_leaf_data(double d, index_type begin, index_type end):
    rgtl_octree_objects_cell_data(d), index_begin(begin), index_end(end) {}
 private:
  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr & rgtl_serialize_base<derived>(*this);
    sr & index_begin;
    sr & index_end;
  }
};

//----------------------------------------------------------------------------
#ifdef RGTL_OCTREE_OBJECTS_NODE_DT
struct rgtl_octree_objects_node_data: public rgtl_octree_objects_cell_data
{
  typedef rgtl_octree_objects_cell_data derived;

  // Currently we have no extra data for nodes.
  rgtl_octree_objects_node_data() {}
  rgtl_octree_objects_node_data(double d): rgtl_octree_objects_cell_data(d) {}
 private:
  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr & rgtl_serialize_base<derived>(*this);
  }
};
#else
typedef void rgtl_octree_objects_node_data;
#endif

//----------------------------------------------------------------------------
template <unsigned int D>
class rgtl_octree_objects_distance_transform
{
 public:
  typedef rgtl_octree_objects_leaf_data leaf_data_type;
  typedef rgtl_octree_objects_node_data node_data_type;
  typedef rgtl_octree_data_fixed<D, leaf_data_type, node_data_type> tree_type;
  typedef typename tree_type::cell_index_type cell_index_type;
  typedef typename tree_type::cell_location_type cell_location_type;
  typedef typename tree_type::child_index_type child_index_type;

  // An entry on the front of propagating distance bounds.
  struct entry_type
  {
    entry_type(cell_location_type const& idx,
               cell_index_type cidx, double bound):
      location(idx), cell_index(cidx), Bound(bound) {}

    // Logical index of the cell.
    cell_location_type location;

    // Integer index of the cell.
    cell_index_type cell_index;

    // Current bound.
    double Bound;
  };
  struct entry_compare_type
  {
    bool operator()(const entry_type& l, const entry_type& r) const
    {
      return l.Bound < r.Bound;
    }
  };
  struct index_compare_type
  {
    bool operator()(cell_index_type l, cell_index_type r) const
    {
      return l < r;
    }
  };

  // The front of propagating distance bounds.
  typedef std::multiset<entry_type, entry_compare_type> front_type;
  front_type front_;
  typedef std::map<cell_index_type,
                  typename front_type::iterator,
                  index_compare_type> front_index_map_type;
  front_index_map_type front_index_map_;

  typedef rgtl_octree_objects_internal<D> objects_type;
  objects_type& objects_;
  tree_type& tree_;
  int k_;
  std::vector<double> squared_distances_;

  rgtl_octree_objects_distance_transform(objects_type* intern, int k):
    objects_(*intern), tree_(intern->tree_), k_(k),
    squared_distances_(k) {}
  void initialize_front(cell_location_type const& cell, cell_index_type cidx);
  bool execute_transform();
  void propagate_front(cell_location_type const& cell, double const center[D],
                       double distance);
  void propagate_front(double const from_center[D], double distance,
                       unsigned int face, cell_location_type const& cell,
                       cell_index_type cidx);
#ifdef RGTL_OCTREE_OBJECTS_NODE_DT
  bool transform_nodes();
  bool transform_nodes(cell_location_type const& cell, cell_index_type cidx,
                       double half_diagonal, double& parent_bound);
#endif
};

//----------------------------------------------------------------------------
template <unsigned int D>
class rgtl_octree_objects_query_closest
{
 public:
  // Construct with a reference to the main representation.
  typedef rgtl_octree_objects_internal<D> internal_type;
  rgtl_octree_objects_query_closest(internal_type const& intern, int k);

  // Get useful types from the main representation.
  typedef typename internal_type::cell_location_type cell_location_type;
  typedef typename internal_type::cell_index_type cell_index_type;
  typedef typename internal_type::child_index_type child_index_type;
  typedef typename internal_type::leaf_data_type leaf_data_type;
  typedef typename internal_type::node_data_type node_data_type;

  // The query entry point.
  int query(double const p[D], int k, double bound,
            int* ids, double* squared_distances, double* points);

  // The recursive query implementation.
  void query_impl(cell_location_type const& cell, cell_index_type cell_index,
                  double const p[D], int k);

  // Entry in the list of closest objects.
  struct closest_object_entry
  {
    closest_object_entry(): id(-1), distance_squared(-1) {}
    closest_object_entry(int idx, double d2, double q[D])
    : id(idx), distance_squared(d2)
    {
      for (unsigned int a=0; a < D; ++a)
      {
        this->point[a] = q[a];
      }
    }
    int id;
    double distance_squared;
    double point[D];

    bool operator<(closest_object_entry const& that)
    {
      return this->distance_squared >= 0 &&
             that.distance_squared  >= 0 &&
             this->distance_squared < that.distance_squared;
    }
  };

  // The internal objects representation.
  internal_type const& internal_;

  // Keep a sorted list of the best k squared distances.  Use -1 to
  // indicate no object yet found.
  std::vector<closest_object_entry> best_;

  // Keep track of the current squared distance bound.
  double bound_;

#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
  // Keep track of the number of object queries made.
  int checked_count_;
#endif
};

//----------------------------------------------------------------------------
template <unsigned int D>
class rgtl_octree_objects_internal
{
 public:
  typedef rgtl_octree_objects_leaf_data leaf_data_type;
  typedef rgtl_octree_objects_node_data node_data_type;
  typedef rgtl_octree_data_fixed<D, leaf_data_type, node_data_type> tree_type;
  typedef rgtl_object_array<D> object_array_type;
  typedef rgtl_octree_cell_bounds<D> bounds_type;
  typedef typename tree_type::cell_index_type cell_index_type;
  typedef typename tree_type::cell_location_type cell_location_type;
  typedef typename tree_type::child_index_type child_index_type;
  typedef rgtl_octree_cell_geometry<D> cell_geometry_type;
  rgtl_octree_objects_internal(object_array_type const& objs);

  rgtl_octree_objects_internal(object_array_type const& objs,
                               bounds_type const& b, int ml);

  // Wrap around public object array API.
  int number_of_objects() const
  {
    return this->object_array_.number_of_objects();
  }
  bool object_intersects_object(int idA, int idB) const
  {
    return this->object_array_.object_intersects_object(idA, idB);
  }
  bool object_intersects_box(int id,
                             cell_geometry_type const& cell_geometry) const
  {
    return this->object_array_.object_intersects_box
      (id,
       cell_geometry.get_sphere_center(),
       cell_geometry.get_sphere_radius(),
       cell_geometry.get_lower(),
       cell_geometry.get_upper(),
       cell_geometry.get_corners());
  }
  bool object_closest_point(int id,
                            double const x[D],
                            double y[D],
                            double bound_squared) const
  {
    return this->object_array_.object_closest_point(id, x, y, bound_squared);
  }

  // Compute the squared distance between two points.
  static double compute_distance_squared(double const p[D],
                                         double const q[D])
  {
    double d = 0.0;
    for (unsigned int a=0; a < D; ++a)
    {
      double da = p[a]-q[a];
      d += da*da;
    }
    return d;
  }

  // Compute the center point of a cell.
  void compute_center(cell_location_type const& cell, double center[D]) const
  {
    rgtl_octree_cell_bounds<D> upper;
    child_index_type last = child_index_type((1<<D)-1);
    upper.compute_bounds(this->bounds_, cell.get_child(last));
    upper.origin().copy_out(center);
  }

  // Compute the squared distances to the nearest and farthest point
  // of a cell volume to the given point.
  void cell_distances(cell_location_type const& cell, double const x[D],
                      double& nearest_squared, double& farthest_squared) const;
  void cell_distances(bounds_type const& cell_bounds, double const x[D],
                      double& nearest_squared, double& farthest_squared) const;
  void cell_nearest(cell_location_type const& cell, double const x[D],
                    double& nearest_squared) const;
  void cell_nearest(bounds_type const& cell_bounds, double const x[D],
                    double& nearest_squared) const;

  // Build the spatial structure within the given cell.
  void build(cell_geometry_type const& cell_geometry, std::vector<int>& ids);

  // Query the closest k objects.
  int query_closest(double const p[D], int k, double bound_squared, int* ids,
                    double* squared_distances, double* points) const;
  int query_closest_impl(double const p[D], int k, double current_bound,
                         int* ids, double* squared_distances,
                         double* points) const;

  // Lookup an initial closest object distance bound.
  void lookup_leaf(cell_location_type const& cell,
                   cell_index_type cidx,
                   double const p[D],
                   cell_location_type& leaf, cell_index_type& lidx) const;

  // Compute a kth-order distance transform on the cell centers so
  // that each has a known distance to the kth-closest object.
  bool compute_distance_transform(int k);

  // Query the objects intersecting a sphere.
  int query_sphere(double const center[D], double radius,
                   std::vector<int>& ids) const;
  void query_sphere(cell_location_type const& cell,
                    cell_index_type cidx,
                    double const p[D],
                    double radius_squared,
                    std::vector<int>& ids) const;

  // Extract objects from a cell.
  void extract_objects(cell_index_type cidx,
                       double const* center, double radius_squared,
                       std::vector<int>& ids) const;

  // Query the objects intersecting an object.
  int query_object(int id, std::vector<int>& ids) const;
  void query_object(cell_location_type const& cell,
                    cell_index_type cidx,
                    int idA, std::vector<int>& ids) const;

#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
  void set_query_closest_debug(bool b) { this->query_closest_debug_ = b; }
#else
  void set_query_closest_debug(bool) {}
#endif

 private:
  object_array_type const& object_array_;
  bounds_type bounds_;

  // The maximum subdivision level.
  int max_level_;

  // The maximum number of objects per leaf.
  int max_per_leaf_;

  // The order of the computed distance transform.
  int distance_transform_order_;

  // Keep track of objects visited on a per-query basis.
  rgtl_object_once object_once_;

  // The actual octree structure.
  tree_type tree_;

  // Leaf data stores index ranges into this array which maps to the
  // object ids in each leaf.
  std::vector<int> object_ids_;

#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
  // Enable closest point query output only after the distance
  // transform has finished.
  bool query_closest_debug_;
#endif

  friend class rgtl_octree_objects_distance_transform<D>;
  friend class rgtl_octree_objects_query_closest<D>;

  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr & bounds_;
    sr & max_level_;
    sr & max_per_leaf_;
    sr & distance_transform_order_;
    sr & object_once_;
    sr & tree_;
    sr & object_ids_;
  }
};

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_objects_internal<D>
::rgtl_octree_objects_internal(object_array_type const& objs):
  object_array_(objs), bounds_(), max_level_(0), max_per_leaf_(0),
  distance_transform_order_(0), object_once_()
{
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
  this->query_closest_debug_ = false;
#endif
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_objects_internal<D>
::rgtl_octree_objects_internal(object_array_type const& objs,
                               bounds_type const& b, int ml):
  object_array_(objs), bounds_(b), max_level_(ml), max_per_leaf_(10),
  distance_transform_order_(0), object_once_(objs.number_of_objects())
{
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
  this->query_closest_debug_ = false;
#endif
  int n = this->number_of_objects();
  std::vector<int> ids(n);
  for (int i=0; i < n; ++i)
  {
    ids[i] = i;
  }
  cell_geometry_type root_geometry(cell_location_type(), this->bounds_);
  this->build(root_geometry, ids);
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_objects_internal<D>
::cell_distances(cell_location_type const& cell, double const x[D],
                 double& nearest_squared, double& farthest_squared) const
{
  // Get the bounds of the cell.
  bounds_type cell_bounds;
  cell_bounds.compute_bounds(this->bounds_, cell);

  // Compute the distances for these bounds.
  this->cell_distances(cell_bounds, x, nearest_squared, farthest_squared);
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_objects_internal<D>
::cell_distances(bounds_type const& cell_bounds, double const x[D],
                 double& nearest_squared, double& farthest_squared) const
{
  // Compute the squared magnitude of vectors pointing from the given
  // point the nearest and farthest points in the volume of the cell.
  nearest_squared = 0;
  farthest_squared = 0;
  for (unsigned int a=0; a < D; ++a)
  {
    double l = cell_bounds.origin(a)-x[a];
    double u = cell_bounds.origin(a)+cell_bounds.size()-x[a];
    double l2 = l*l;
    double u2 = u*u;
    if (l > 0) { nearest_squared += l2; }
    else if (u < 0) { nearest_squared += u2; }
    if (l2 > u2) { farthest_squared += l2; }
    else { farthest_squared += u2; }
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_objects_internal<D>
::cell_nearest(cell_location_type const& cell, double const x[D],
               double& nearest_squared) const
{
  // Get the bounds of the cell.
  bounds_type cell_bounds;
  cell_bounds.compute_bounds(this->bounds_, cell);

  // Compute the distances for these bounds.
  this->cell_nearest(cell_bounds, x, nearest_squared);
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_objects_internal<D>
::cell_nearest(bounds_type const& cell_bounds, double const x[D],
               double& nearest_squared) const
{
  // Compute the squared magnitude of vectors pointing from the given
  // point the nearest point in the volume of the cell.
  nearest_squared = 0;
  for (unsigned int a=0; a < D; ++a)
  {
    double l = cell_bounds.origin(a)-x[a];
    double u = cell_bounds.origin(a)+cell_bounds.size()-x[a];
    double l2 = l*l;
    double u2 = u*u;
    if (l > 0) { nearest_squared += l2; }
    else if (u < 0) { nearest_squared += u2; }
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_objects_internal<D>
::build(cell_geometry_type const& cell_geometry, std::vector<int>& ids)
{
  cell_location_type const& cell = cell_geometry.location();
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_BUILD
  std::cout << "Considering " << ids.size()
           << " objects in cell " << cell << std::endl;
#endif
  bool tooDeep = cell.level() >= this->max_level_;
  bool tooMany = (static_cast<int>(ids.size()) > this->max_per_leaf_);
  if (tooMany && !tooDeep)
  {
    // We need to divide this cell.
    // Compute the child cell geometries.
    cell_geometry_type child_geometry[1<<D];
    cell_geometry.get_children(child_geometry);

    // Distribute objects into children.
    std::vector<int> child_ids[1<<D];
    for (unsigned int i=0; i < (1<<D); ++i)
    {
      // Build a list of objects intersecting this child.
      for (std::vector<int>::const_iterator pi = ids.begin();
           pi != ids.end(); ++pi)
      {
        int id = *pi;
        if (this->object_intersects_box(id, child_geometry[i]))
        {
          child_ids[i].push_back(id);
        }
      }
    }

    // Erase memory used by id list for this cell.
    ids.clear();

    // Build the children recursively.
    for (unsigned int i=0; i < (1<<D); ++i)
    {
      this->build(child_geometry[i], child_ids[i]);
    }
  }
  else
  {
    // We will not divide this cell further.
    // Store the objects for this cell.
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_BUILD
    if (!ids.empty())
    {
      std::cout << "Storing " << ids.size() << " objects in cell "
               << cell << std::endl;
    }
#endif
    typedef typename leaf_data_type::index_type index_type;
    index_type index_begin = index_type(this->object_ids_.size());
    for (std::vector<int>::const_iterator pi = ids.begin();
         pi != ids.end(); ++pi)
    {
      this->object_ids_.push_back(*pi);
    }
    index_type index_end = index_type(this->object_ids_.size());
    leaf_data_type leaf_data(-std::numeric_limits<double>::infinity(),
                             index_begin, index_end);

    // Store the data in the leaf.
    this->tree_.set_leaf_data(cell, &leaf_data);
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
int
rgtl_octree_objects_internal<D>
::query_closest(double const p[D], int k, double bound_squared,
                int* ids, double* squared_distances, double* points) const
{
  // Establish an initial upper-bound on the distance to an object.
  double nearest_squared;
  double farthest_squared;
  this->cell_distances(this->bounds_, p, nearest_squared, farthest_squared);

  // Check for a smaller bound given by the caller.
  if (bound_squared >= 0 && bound_squared < farthest_squared)
  {
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
    if (this->query_closest_debug_)
    {
      std::cout << "User initial bound "
               << bound_squared << " < " << farthest_squared << std::endl;
    }
#endif
    farthest_squared = bound_squared;
  }

#ifndef RGTL_OCTREE_OBJECTS_NODE_DT
  // Check for a smaller bound given by the distance transform.
  if (nearest_squared <= 0 && k <= this->distance_transform_order_)
  {
    // The query point is inside the root cell and the number of
    // objects desired is not larger than the order of the distance
    // transform.  An initial bound may be available in the leaf
    // containing the query point.
    cell_location_type leaf;
    cell_index_type lidx;
    this->lookup_leaf(cell_location_type(), cell_index_type(), p, leaf, lidx);
    if (leaf_data_type const* leaf_data = this->tree_.get_leaf_data(lidx))
    {
      if (vnl_math::isfinite(leaf_data->distance))
      {
        // Compute a bound on the distance by adding the distance from
        // the query point to the cell center and the distance from
        // the cell center to the nearest object.
        double bound = std::fabs(leaf_data->distance);
        double center[D];
        this->compute_center(leaf, center);
        double distance_squared = this->compute_distance_squared(center, p);
        bound += std::sqrt(distance_squared);

        // Shrink the squared distance bound if possible.
        // Increase the computed potential bound by a small fraction
        // in order to avoid missing an object altogether due to
        // rounding problems.
        double squared_bound = bound*bound*1.00001;
        if (squared_bound < farthest_squared)
        {
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
          if (this->query_closest_debug_)
          {
            std::cout << "Reduced initial bound "
                     << squared_bound << " < " << farthest_squared
                     << " using leaf " << leaf << std::endl;
          }
#endif
          farthest_squared = squared_bound;
        }
      }
    }
  }
#endif

  // Perform the real query.
  return this->query_closest_impl(p, k, farthest_squared,
                                  ids, squared_distances, points);
}

//----------------------------------------------------------------------------
template <unsigned int D>
int
rgtl_octree_objects_internal<D>
::query_closest_impl(double const p[D], int k, double current_bound, int* ids,
                     double* squared_distances, double* points) const
{
  rgtl_octree_objects_query_closest<D> qc(*this, k);
  return qc.query(p, k, current_bound, ids, squared_distances, points);
}

//----------------------------------------------------------------------------
template <unsigned int D>
void rgtl_octree_objects_internal<D>
::lookup_leaf(cell_location_type const& cell,
              cell_index_type cidx,
              double const p[D],
              cell_location_type& leaf, cell_index_type& lidx) const
{
  if (this->tree_.has_children(cidx))
  {
    // Compute the index of the child containing the point.
    double center[D];
    this->compute_center(cell, center);
    child_index_type child_index(0);
    for (unsigned int a=0; a < D; ++a)
    {
      child_index |= (p[a] < center[a])? 0 : (1<<a);
    }

    // Recursively explore the child.
    this->lookup_leaf(cell.get_child(child_index),
                      this->tree_.get_child(cidx, child_index),
                      p, leaf, lidx);
  }
  else
  {
    // Return this leaf.
    leaf = cell;
    lidx = cidx;
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool rgtl_octree_objects_internal<D>::compute_distance_transform(int k)
{
  // Make sure we do not compute more than one distance transform.
  if (this->distance_transform_order_ > 0)
  {
    return this->distance_transform_order_ == k;
  }

  // Setup the kth-order transform.
  rgtl_octree_objects_distance_transform<D> dt(this, k);

  // Initialize the transform front.
  dt.initialize_front(cell_location_type(), cell_index_type());

  // Execute the entire transform.  Disable debugging output for
  // closest object computation during the transform.
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
  bool old = this->query_closest_debug_;
  this->query_closest_debug_ = false;
#endif
  bool success = dt.execute_transform();
#ifdef RGTL_OCTREE_OBJECTS_NODE_DT
  success = success && dt.transform_nodes();
#endif
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
  this->query_closest_debug_ = old;
#endif

  // If the transform succeeded store its order to enable use during
  // closest object queries.
  if (success)
  {
    this->distance_transform_order_ = k;
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_objects_distance_transform<D>
::initialize_front(cell_location_type const& cell, cell_index_type cidx)
{
  if (this->tree_.has_children(cidx))
  {
    // Recursively bound the children.
    for (child_index_type i(0); i < (1<<D); ++i)
    {
      this->initialize_front(cell.get_child(i),
                             this->tree_.get_child(cidx, i));
    }
  }
  else if (leaf_data_type const* leaf_data =
           this->tree_.get_leaf_data(cidx))
  {
    // The front is initialized with non-empty leaves.
    if (leaf_data->index_begin != leaf_data->index_end)
    {
      double bound;
      if (leaf_data->index_end - leaf_data->index_begin >= k_)
      {
        // The initial bound is the half-diagonal length because the
        // leaf is known to contain at least k objects in its volume.
        rgtl_octree_cell_bounds<D> cell_bounds;
        cell_bounds.compute_bounds(this->objects_.bounds_, cell);
        double half = cell_bounds.size() / 2;
        bound = std::sqrt(half*half*D);
      }
      else
      {
        // The initial bound is infinity because the leaf does not
        // contain at least k objects.
        bound = std::numeric_limits<double>::infinity();
      }

      // Update the leaf data with this bound.
      leaf_data_type new_leaf(-bound,
                              leaf_data->index_begin, leaf_data->index_end);
      this->tree_.set_leaf_data(cidx, &new_leaf);

      // Create the front entry.
      entry_type entry(cell, cidx, bound);
      typename front_type::iterator ei = this->front_.insert(entry);
      this->front_index_map_[cidx] = ei;
    }
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool
rgtl_octree_objects_distance_transform<D>
::execute_transform()
{
  // Loop until the front is empty.
  while (!this->front_.empty())
  {
    // Get the next entry in the front.
    entry_type entry = *this->front_.begin();

    // Erase the entry from the front.
    this->front_.erase(this->front_.begin());
    this->front_index_map_.erase(entry.cell_index);

    // Compute the true distance for this entry's cell center.
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_FRONT
    std::cout << "Checking front entry cell " << entry.location
             << " with bound " << entry.Bound << std::endl;
#endif
    double squared_bound = entry.Bound*entry.Bound;
    double* squared_distances = &this->squared_distances_[0];
    double center[D];
    this->objects_.compute_center(entry.location, center);
    int n = this->objects_.query_closest_impl(center, this->k_, squared_bound,
                                              0, squared_distances, 0);
    if (n != this->k_)
    {
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_FRONT
      std::cout << "Could not find " << this->k_
               << " closest object(s)." << std::endl;
#endif
      return false;
    }
    double distance = std::sqrt(squared_distances[n-1]);
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_FRONT
    std::cout << "Found distance " << distance << std::endl;
#endif

    // Update the leaf data with this distance.
    leaf_data_type const* old_leaf =
      this->tree_.get_leaf_data(entry.cell_index);
    leaf_data_type new_leaf(distance,
                            old_leaf->index_begin, old_leaf->index_end);
    this->tree_.set_leaf_data(entry.cell_index, &new_leaf);

    // Propagate bounds to adjacent cells based on this distance.
    this->propagate_front(entry.location, center, distance);
  }

  return true;
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_objects_distance_transform<D>
::propagate_front(cell_location_type const& cell,
                  double const center[D],
                  double distance)
{
  // Propagate a distance bound through each face of the cell.
  for (unsigned int face = 0; face < 2*D; ++face)
  {
    // Get the neighbor through this face.
    cell_location_type neighbor;
    cell_index_type nidx;
    if (this->tree_.get_neighbor(cell, face, neighbor, nidx))
    {
      this->propagate_front(center, distance, face, neighbor, nidx);
    }
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_objects_distance_transform<D>
::propagate_front(double const from_center[D],
                  double distance,
                  unsigned int face,
                  cell_location_type const& cell,
                  cell_index_type cidx)
{
  if (this->tree_.has_children(cidx))
  {
    // Recurse to the children that are the real face neighbors of the
    // original cell.
    unsigned int axis = face >> 1;
    unsigned int side = (face & 1) ^ 1;

    // The bit corresponding to the face axis is fixed to one side.
    // The other bits come from i.
    unsigned int lower_mask = ((1<<axis)-1);
    unsigned int middle_bit = (side << axis);
    unsigned int upper_mask = ((1<<(D-1))-1) ^ lower_mask;
    for (unsigned int i=0; i < (1<<(D-1)); ++i)
    {
      child_index_type child_index =
        child_index_type(((upper_mask & i) << 1) |
                         middle_bit |
                         (lower_mask & i));
      this->propagate_front(from_center, distance, face,
                            cell.get_child(child_index),
                            this->tree_.get_child(cidx, child_index));
    }
  }
  else if (leaf_data_type const* old_leaf = this->tree_.get_leaf_data(cidx))
  {
    // This is a leaf to which we may propagate a bound and update the
    // front.
    if (old_leaf->distance >= 0)
    {
      // This leaf already has the true distance.
      return;
    }

    // Compute the propagated bound.
    double to_center[D];
    this->objects_.compute_center(cell, to_center);
    double distance_squared =
      this->objects_.compute_distance_squared(to_center, from_center);
    double bound = distance + std::sqrt(distance_squared);

    // Propagate the bound if it is smaller.  Note that tentative
    // bounds are stored in the leaves as negative distances.
    if (bound < -old_leaf->distance)
    {
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_FRONT
      std::cout << "Updating bound of leaf " << cell
               << " with " << bound << " < " << (-old_leaf->distance)
               << std::endl;
#endif
      // Update the leaf data with the new bound.
      leaf_data_type new_leaf(-bound,
                              old_leaf->index_begin, old_leaf->index_end);
      this->tree_.set_leaf_data(cidx, &new_leaf);

      // Update the front entry for the leaf.  Note that we must
      // remove the old entry and insert a new entry to be sure the
      // ordering of the front is updated properly.
      entry_type entry(cell, cidx, bound);
      typename front_type::iterator ei = this->front_.insert(entry);
      typename front_index_map_type::iterator mi =
        this->front_index_map_.find(cidx);
      if (mi == this->front_index_map_.end())
      {
        // The leaf was not already in the front.  Create a front
        // index for the new entry.
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_FRONT
        std::cout << "Creating new front entry." << std::endl;
#endif
        this->front_index_map_[cidx] = ei;
      }
      else
      {
        // The leaf was already in the front.  Remove the old entry
        // and update the front index for the new entry.
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_FRONT
        std::cout << "Replacing old front entry." << std::endl;
#endif
        this->front_.erase(mi->second);
        mi->second = ei;
      }
    }
  }
}

#ifdef RGTL_OCTREE_OBJECTS_NODE_DT
//----------------------------------------------------------------------------
template <unsigned int D>
bool
rgtl_octree_objects_distance_transform<D>
::transform_nodes()
{
  double half = this->objects_.bounds_.size()/2;
  double half_diagonal = std::sqrt(D*half*half);
  double bound = 0;
  return this->transform_nodes(cell_location_type(), cell_index_type(),
                               half_diagonal, bound);
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool
rgtl_octree_objects_distance_transform<D>
::transform_nodes(cell_location_type const& cell, cell_index_type cidx,
                  double half_diagonal, double& parent_bound)
{
  double distance;
  if (this->tree_.has_children(cidx))
  {
    // Recursively transform the children.
    double bound = std::numeric_limits<double>::infinity();
    for (child_index_type i(0); i < (1<<D); ++i)
    {
      if (!this->transform_nodes(cell.get_child(i),
                                 this->tree_.get_child(cidx, i),
                                 half_diagonal/2, bound))
      {
        return false;
      }
    }

    // We now have a bound provided by our children.  Use it to
    // compute this node center distance.
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_FRONT
    std::cout << "Checking node " << cell
             << " with bound " << bound << std::endl;
#endif
    double squared_bound = bound*bound;
    double squared_distance = -1;
    double* squared_distances = &this->squared_distances_[0];
    double center[D];
    this->objects_.compute_center(cell, center);
    int n = this->objects_.query_closest_impl(center, this->k_, squared_bound,
                                              0, squared_distances, 0);
    if (n != this->k_)
    {
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_FRONT
      std::cout << "Could not find " << this->k_
               << " closest object(s)." << std::endl;
#endif
      return false;
    }
    distance = std::sqrt(squared_distances[n-1]);
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_FRONT
    std::cout << "Found distance " << distance << std::endl;
#endif

    // Store this distance in the node.
    node_data_type new_node(distance);
    this->tree_.set_node_data(cidx, &new_node);
  }
  else if (leaf_data_type const* leaf_data =
           this->tree_.get_leaf_data(cidx))
  {
    // Check this leaf.
    if (leaf_data->distance < 0)
    {
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_FRONT
      std::cout << "No distance for leaf " << cell << std::endl;
#endif
      return false;
    }
    else
    {
      // Use the distance provided by this leaf.
      distance = leaf_data->distance;
    }
  }
  else
  {
    // There is no leaf!
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_FRONT
    std::cout << "No data for leaf " << cell << std::endl;
#endif
    return false;
  }

  // Bound the distance for our parent cell center using our distance.
  double bound = distance + half_diagonal;
  if (bound < parent_bound)
  {
    parent_bound = bound;
  }

  return true;
}
#endif

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_objects_query_closest<D>
::rgtl_octree_objects_query_closest(internal_type const& intern, int k):
  internal_(intern), best_(k), bound_(0)
{
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
  this->checked_count_ = 0;
#endif
}

//----------------------------------------------------------------------------
template <unsigned int D>
int
rgtl_octree_objects_query_closest<D>
::query(double const p[D], int k, double bound,
        int* ids, double* squared_distances, double* points)
{
  // Store the initial squared distance bound.
  this->bound_ = bound;

#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
  if (this->internal_.query_closest_debug_)
  {
    std::cout << "Querying point (";
    const char* sep = "";
    for (unsigned int a=0; a < D; ++a)
    {
      std::cout << sep << p[a];
      sep = ", ";
    }
    std::cout << ") with initial bound " << this->bound_ << std::endl;
  }
#endif

  // Keep track objects already tested to avoid duplicating tests.
  this->internal_.object_once_.reset();

  // Recursively visit the tree starting at the root.
  this->query_impl(cell_location_type(), cell_index_type(), p, k);

#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
  if (this->internal_.query_closest_debug_)
  {
    std::cout << " Checked " << this->checked_count_
             << " of " << this->internal_.number_of_objects()
             << " objects." << std::endl;
  }
#endif

  // Copy the final object id list to the output.
  std::vector<closest_object_entry>& best = this->best_;
  for (int i=0; i < k; ++i)
  {
    if (best[i].distance_squared >= 0)
    {
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
      if (this->internal_.query_closest_debug_)
      {
        std::cout << i
                 << ": id " << best[i].id
                 << ", d  " << std::sqrt(best[i].distance_squared) << std::endl;
      }
#endif
      if (ids)
      {
        ids[i] = best[i].id;
      }
      if (squared_distances)
      {
        squared_distances[i] = best[i].distance_squared;
      }
      if (points)
      {
        for (unsigned int a=0; a < D; ++a)
        {
          points[i*D+a] = best[i].point[a];
        }
      }
    }
    else
    {
      return i;
    }
  }
  return k;
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_objects_query_closest<D>
::query_impl(cell_location_type const& cell, cell_index_type cell_index,
             double const p[D], int k)
{
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
  if (this->internal_.query_closest_debug_)
  {
    std::cout << "Considering cell " << cell << std::endl;
  }
#endif

  // Make sure the cell intersects the current bounding sphere.
  double nearest_squared;
  this->internal_.cell_nearest(cell, p, nearest_squared);
  if (nearest_squared > this->bound_)
  {
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
    if (this->internal_.query_closest_debug_)
    {
      std::cout << " cell is out of range: " << nearest_squared
               << " > " << this->bound_ << std::endl;
    }
#endif
    return;
  }

#ifdef RGTL_OCTREE_OBJECTS_NODE_DT
  // Compute the cell center.
  double center[D];
  this->internal_.compute_center(cell, center);
  double center_distance = std::numeric_limits<double>::infinity();
#endif

  // Check whether this is a node or leaf.
  bool is_node = this->internal_.tree_.has_children(cell_index);
  leaf_data_type const* ld = 0;
  if (is_node)
  {
#ifdef RGTL_OCTREE_OBJECTS_NODE_DT
    // Check if this node center provides a distance.
    if (node_data_type const* nd =
        this->internal_.tree_.get_node_data(cell_index))
    {
      center_distance = nd->distance;
    }
#endif
  }
  else
  {
    // This is a leaf.  Lookup the data.
    ld = this->internal_.tree_.get_leaf_data(cell_index);

#ifdef RGTL_OCTREE_OBJECTS_NODE_DT
    // Check if this leaf center provides a distance.
    if (ld)
    {
      center_distance = ld->distance;
    }
#endif
  }

#ifdef RGTL_OCTREE_OBJECTS_NODE_DT
  // Try to reduce the current bound using the distance transform
  // result for this cell.
  if (k <= this->internal_.distance_transform_order_ &&
      (center_distance*center_distance) < this->bound_)
  {
    double distance_squared =
      this->internal_.compute_distance_squared(center, p);
    if (distance_squared < this->bound_)
    {
      // Compute a bound on the distance by adding the distance from
      // the query point to the cell center and the distance from
      // the cell center to its nearest object.
      double bound = std::fabs(center_distance) + std::sqrt(distance_squared);

      // Shrink the squared distance bound if possible.
      // Increase the computed potential bound by a small fraction
      // in order to avoid missing an object altogether due to
      // rounding problems.
      double squared_bound = bound*bound*1.00001;
      if (squared_bound < this->bound_)
      {
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
        if (this->internal_.query_closest_debug_)
        {
          std::cout << "Reduced current bound "
                   << squared_bound << " < " << this->bound_
                   << " using cell " << cell << std::endl;
        }
#endif
        this->bound_ = squared_bound;
      }
    }
  }
#endif

  if (is_node)
  {
    // Visit the children closer to the query point first.
#ifndef RGTL_OCTREE_OBJECTS_NODE_DT
    double center[D];
    this->internal_.compute_center(cell, center);
#endif
    unsigned int child_xor = 0;
    for (unsigned int a=0; a < D; ++a)
    {
      if (p[a] >= center[a])
      {
        child_xor |= (1<<a);
      }
    }

    // The cell is divided, so visit the children.
    for (unsigned int l = 0; l < (1<<D); ++l)
    {
      child_index_type i(l^child_xor);
      this->query_impl(cell.get_child(i),
                       this->internal_.tree_.get_child(cell_index, i), p, k);
    }
  }
  else if (ld)
  {
    // This is a leaf.  Look for objects.
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
    if (this->internal_.query_closest_debug_)
    {
      if (ld->index_begin < ld->index_end)
      {
        std::cout << " checking objects" << std::endl;
      }
      else
      {
        std::cout << " no objects" << std::endl;
      }
    }
#endif
    // Loop over the objects in this cell.  When the objects are not
    // points it is possible that an object will provide a point
    // that is closer to the query than any point in the cell.  If
    // this occurs we can stop testing objects in the cell.
    typedef typename leaf_data_type::index_type index_type;
    for (index_type i=ld->index_begin;
         i < ld->index_end && nearest_squared <= this->bound_; ++i)
    {
      // Test each object at most once.
      int id = this->internal_.object_ids_[i];
      if (this->internal_.object_once_.visit(id))
      {
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
        ++checked_count_;
#endif

        // Get the squared distance to this object and use it only
        // if it is smaller than the current distance bound.
        double q[D];
        double distance_squared;
        if (this->internal_.object_closest_point(id, p, q, this->bound_) &&
            (distance_squared = this->internal_.compute_distance_squared(p, q),
             distance_squared <= this->bound_))
        {
          // Insert this object into our sorted list of k objects.
          std::vector<closest_object_entry>& best = this->best_;
          closest_object_entry obj(id, distance_squared, q);
          for (int j=0; j < k && obj.distance_squared >= 0; ++j)
          {
            if (best[j].distance_squared < 0 || obj < best[j])
            {
              std::swap(obj, best[j]);
            }
          }

          // Update the current bounding sphere radius.
          if (best[k-1].distance_squared >= 0 &&
              best[k-1].distance_squared < this->bound_)
          {
            this->bound_ = best[k-1].distance_squared;
#ifdef RGTL_OCTREE_OBJECTS_DEBUG_QUERY
            if (this->internal_.query_closest_debug_)
            {
              std::cout << " Reduced bound to " << this->bound_ << std::endl;
              if (nearest_squared > this->bound_ && i < ld->index_end-1)
              {
                std::cout << " Terminating cell early!" << std::endl;
              }
            }
#endif
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
int
rgtl_octree_objects_internal<D>
::query_sphere(double const center[D], double radius,
               std::vector<int>& ids) const
{
  // Reset the object visitation marks.
  this->object_once_.reset();

  // Query the tree recursively.
  this->query_sphere(cell_location_type(), cell_index_type(),
                     center, radius*radius, ids);

  // Return the number of objects found.
  return static_cast<int>(ids.size());
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_objects_internal<D>
::query_sphere(cell_location_type const& cell,
               cell_index_type cidx,
               double const p[D],
               double radius_squared,
               std::vector<int>& ids) const
{
  // Compute the squared magnitude of vectors pointing from the sphere
  // center to the nearest and farthest points in the volume of the
  // cell.
  double nearest_squared;
  double farthest_squared;
  this->cell_distances(cell, p, nearest_squared, farthest_squared);

  // If the radius of the sphere is less than the nearest point on the
  // cell then the entire cell is outside the sphere.
  if (radius_squared < nearest_squared)
  {
    // The sphere contains no part of the cell.  Do nothing.
  }
  // If the radius of the sphere is greater than the farthest point on
  // the cell then the entire cell is contained within the sphere.
  else if (radius_squared > farthest_squared)
  {
    // The sphere completely contains the cell.  Extract objects in
    // the cell and its children.  By not passing the center or radius
    // to this method it will not bother testing the objects since
    // they are known to intersect the cell, which is inside the
    // sphere.
    this->extract_objects(cidx, 0, 0, ids);
  }
  // If the radius of the sphere is between the length of the nearest
  // and farthest vectors then the surface of the sphere must pass
  // through the cell.
  else
  {
    if (this->tree_.has_children(cidx))
    {
      // The cell is divided, so evaluate the children recursively.
      // Some of them may be completely inside or outside the sphere.
      for (child_index_type i(0); i < (1<<D); ++i)
      {
        this->query_sphere(cell.get_child(i),
                           this->tree_.get_child(cidx, i),
                           p, radius_squared, ids);
      }
    }
    else
    {
      // The cell is not divided.  Extract the objects within the sphere.
      this->extract_objects(cidx, p, radius_squared, ids);
    }
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_objects_internal<D>
::extract_objects(cell_index_type cidx,
                  double const* center, double radius_squared,
                  std::vector<int>& ids) const
{
  if (this->tree_.has_children(cidx))
  {
    // Extract objects from all children.
    for (child_index_type i(0); i < (1<<D); ++i)
    {
      this->extract_objects(this->tree_.get_child(cidx, i), center,
                            radius_squared, ids);
    }
  }
  else if (leaf_data_type const* ld = this->tree_.get_leaf_data(cidx))
  {
    // Extract objects from this leaf.
    typedef typename leaf_data_type::index_type index_type;
    for (index_type i=ld->index_begin; i < ld->index_end; ++i)
    {
      int id = this->object_ids_[i];
      if (this->object_once_.visit(id))
      {
        // If a sphere is given test the object against it.
        if (center)
        {
          double q[D];
          if (this->object_closest_point(id, center, q, radius_squared))
          {
            double distance_squared =
              this->compute_distance_squared(center, q);
            if (distance_squared <= radius_squared)
            {
              ids.push_back(id);
            }
          }
        }
        else
        {
          ids.push_back(id);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
int
rgtl_octree_objects_internal<D>
::query_object(int id, std::vector<int>& ids) const
{
  // Reset the object visitation marks.
  this->object_once_.reset();

  // Query the tree recursively.
  this->query_object(cell_location_type(), cell_index_type(), id, ids);

  // Return the number of objects found.
  return static_cast<int>(ids.size());
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_objects_internal<D>
::query_object(cell_location_type const& cell, cell_index_type cidx,
               int idA, std::vector<int>& ids) const
{
  // Get the bounding box of this cell.
  cell_geometry_type cell_geometry(cell, this->bounds_);
  if (!this->object_intersects_box(idA, cell_geometry))
  {
    return;
  }

  // Search this cell for objects.
  if (this->tree_.has_children(cidx))
  {
    // Recursively search the children.
    for (child_index_type i(0); i < (1<<D); ++i)
    {
      this->query_object(cell.get_child(i),
                         this->tree_.get_child(cidx, i), idA, ids);
    }
  }
  else if (leaf_data_type const* ld = this->tree_.get_leaf_data(cidx))
  {
    // Objects in this leaf are candidates.
    typedef typename leaf_data_type::index_type index_type;
    for (index_type i=ld->index_begin; i < ld->index_end; ++i)
    {
      int idB = this->object_ids_[i];
      if (this->object_once_.visit(idB))
      {
        // Check this object for intersection with the query object.
        // Skip the query object itself.
        if (idA != idB &&
            this->object_intersects_object(idA, idB))
        {
          ids.push_back(idB);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_objects<D>::rgtl_octree_objects(object_array_type const& oa):
  internal_(new internal_type(oa))
{
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_objects<D>::rgtl_octree_objects(object_array_type const& objs,
                                            bounds_type const& b, int ml)
{
  this->internal_ = new internal_type(objs, b, ml);
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_octree_objects<D>::~rgtl_octree_objects()
{
  delete this->internal_;
}

//----------------------------------------------------------------------------
template <unsigned int D>
int
rgtl_octree_objects<D>
::query_sphere(double const center[D], double radius,
               std::vector<int>& ids) const
{
  return this->internal_->query_sphere(center, radius, ids);
}

//----------------------------------------------------------------------------
template <unsigned int D>
int
rgtl_octree_objects<D>
::query_object(int id, std::vector<int>& ids) const
{
  return this->internal_->query_object(id, ids);
}

//----------------------------------------------------------------------------
template <unsigned int D>
int
rgtl_octree_objects<D>
::query_closest(double const p[D], int k, int* ids, double* squared_distances,
                double* points, double bound_squared) const
{
  return this->internal_->query_closest(p, k, bound_squared,
                                        ids, squared_distances, points);
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool
rgtl_octree_objects<D>
::compute_distance_transform(int n) const
{
  return this->internal_->compute_distance_transform(n);
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_octree_objects<D>
::set_query_closest_debug(bool b)
{
  this->internal_->set_query_closest_debug(b);
}

//----------------------------------------------------------------------------
template <unsigned int D>
template <class Serializer>
void rgtl_octree_objects<D>::serialize(Serializer& sr)
{
  sr & *(this->internal_);
}

//----------------------------------------------------------------------------
#undef RGTL_OCTREE_OBJECTS_INSTANTIATE
#define RGTL_OCTREE_OBJECTS_INSTANTIATE( D ) \
  template class rgtl_octree_data_fixed< D , rgtl_octree_objects_leaf_data>; \
  template class rgtl_octree_objects_distance_transform< D >; \
  template class rgtl_octree_objects_internal< D >; \
  template class rgtl_octree_objects< D >; \
  template void rgtl_octree_objects< D > \
    ::serialize<rgtl_serialize_ostream>(rgtl_serialize_ostream&); \
  template void rgtl_octree_objects< D > \
    ::serialize<rgtl_serialize_istream>(rgtl_serialize_istream&)

#endif
