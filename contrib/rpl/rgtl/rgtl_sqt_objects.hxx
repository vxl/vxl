// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_sqt_objects_hxx
#define rgtl_sqt_objects_hxx

#include <limits>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "rgtl_sqt_objects.h"

#include "rgtl_config.h"
#include "rgtl_object_array.h"
#include "rgtl_object_once.h"
#include "rgtl_octree_data_fixed.hxx"
#include "rgtl_sqt_cell_bounds.h"
#include "rgtl_sqt_cell_geometry.h"
#include "rgtl_sqt_cell_location.h"
#include "rgtl_sqt_object_array.h"
#include "rgtl_sqt_space.hxx"
#include "rgtl_serialize_access.h"
#include "rgtl_serialize_base.h"
#include "rgtl_serialize_vnl_vector_fixed.h"
#include "rgtl_serialize_istream.h"
#include "rgtl_serialize_ostream.h"

#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>

#include <cassert>

// TODO: During tree construction we should check the set of objects
// in the current node.  Those with no boundary inside the node should
// be left out of the count of objects remaining because no amount of
// subdivision will reduce the count.

//#define RGTL_SQT_OBJECTS_DEBUG_BUILD
//#define RGTL_SQT_OBJECTS_DEBUG_BUILD2
//#define RGTL_SQT_OBJECTS_DEBUG_QUERY

#if defined(RGTL_SQT_OBJECTS_DEBUG_BUILD) || \
    defined(RGTL_SQT_OBJECTS_DEBUG_BUILD2) || \
    defined(RGTL_SQT_OBJECTS_DEBUG_QUERY) || 0
# include <std::iostream.h>
# include <std::exception.h>
#endif

//----------------------------------------------------------------------------
// All cells store bounding information.
template <unsigned int D>
struct rgtl_sqt_objects_cell_data
{
  typedef float cone_bounds_type;
  cone_bounds_type depth_min;
  cone_bounds_type depth_max;
  cone_bounds_type cone_axis[D];
  cone_bounds_type cone_angle;
 private:
  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr & depth_min;
    sr & depth_max;
    sr & cone_axis;
    sr & cone_angle;
  }
};

// Leaves hold per-cell information and also store objects.
template <unsigned int D>
struct rgtl_sqt_objects_leaf_data: public rgtl_sqt_objects_cell_data<D>
{
  typedef rgtl_sqt_objects_cell_data<D> derived;
  typedef int index_type;

  // Store the begin and end index into a list of object ids
  // associated with the tree in which this leaf is stored.
  index_type index_begin;
  index_type index_end;

  rgtl_sqt_objects_leaf_data() {}
  rgtl_sqt_objects_leaf_data(index_type begin, index_type end):
    index_begin(begin), index_end(end) {}
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

// Nodes hold per-cell information.
template <unsigned int D>
struct rgtl_sqt_objects_node_data: public rgtl_sqt_objects_cell_data<D>
{
  typedef rgtl_sqt_objects_cell_data<D> derived;
 private:
  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr & rgtl_serialize_base<derived>(*this);
  }
};

//----------------------------------------------------------------------------
// Method helper object forward declarations.
template <unsigned int D> class rgtl_sqt_objects_query_closest;
template <unsigned int D, unsigned int Face>
class rgtl_sqt_objects_query_closest_face;

//----------------------------------------------------------------------------
// Superclass for typed per-face implementation of spatial structure.
template <unsigned int D>
class rgtl_sqt_objects_face_base
{
 public:
  // Type stored in tree leaves.
  typedef rgtl_sqt_objects_leaf_data<D> leaf_data_type;

  // Type stored in tree nodes.
  typedef rgtl_sqt_objects_node_data<D> node_data_type;

  // The type of the quad-tree itself.
  typedef rgtl_octree_data_fixed<D-1,
                                 leaf_data_type, node_data_type> tree_type;

  // Type-safe cell index in quad-tree representation.
  typedef typename tree_type::cell_index_type cell_index_type;

  // Type-safe child index in quad-tree representation.
  typedef typename tree_type::child_index_type child_index_type;

  // Type of logical SQT cell location.
  typedef rgtl_sqt_cell_location<D> cell_location_type;

  // Type representing SQT cell parameter bounds.
  typedef rgtl_sqt_cell_bounds<D> cell_bounds_type;

  // Full spatial structure internal representation type.
  typedef rgtl_sqt_objects_internal<D> internal_type;

  // Type representing the original array of objects stored.
  typedef typename internal_type::object_array_type object_array_type;

  // Type of pointer to set of objects during construction.
  typedef std::unique_ptr< rgtl_sqt_object_set<D> > sqt_object_set_ptr;

  // Construct with reference to main internal representation.
  rgtl_sqt_objects_face_base(internal_type& intern): internal_(intern) {}

  // Build the spatial structure in this face.
  virtual void build(sqt_object_set_ptr&) = 0;

  // Convert a direction in this face to parameters.
  virtual void direction_to_parameters(double const d[D],
                                       double u[D-1]) const = 0;

  // Query a ray contained in this face.
  virtual bool query_ray(double const d[D], double x[D], double* s) const = 0;

  virtual void query_closest(double const p[D], int k, double const u[D-1],
                             rgtl_sqt_objects_query_closest<D>& qc) const = 0;

  // Access the tree representation for this face.
  tree_type& tree() { return this->tree_; }
  tree_type const& tree() const { return this->tree_; }

 protected:
  // Reference the full spatial structure internal implementation that
  // owns this face.
  internal_type& internal_;

  // The actual quad-tree structure for this face.
  tree_type tree_;

  // Leaf data stores index ranges into this array which maps to the
  // object ids in each leaf.
  std::vector<int> object_ids_;

 private:
  friend class rgtl_serialize_access;
  template <class Serializer> void serialize(Serializer& sr)
  {
    sr & tree_;
    sr & object_ids_;
  }
};

// Implementation of spatial structure for a single face.
template <unsigned int D, unsigned int Face>
class rgtl_sqt_objects_face: public rgtl_sqt_objects_face_base<D>
{
 public:
  typedef rgtl_sqt_objects_face_base<D> derived;

  // Promote type names from superclass.
  typedef typename derived::leaf_data_type leaf_data_type;
  typedef typename derived::node_data_type node_data_type;
  typedef typename derived::tree_type tree_type;
  typedef typename derived::cell_index_type cell_index_type;
  typedef typename derived::child_index_type child_index_type;
  typedef typename derived::cell_location_type cell_location_type;
  typedef typename derived::cell_bounds_type cell_bounds_type;
  typedef typename derived::internal_type internal_type;
  typedef typename derived::object_array_type object_array_type;

  // Type maintaining SQT cell geometry as the tree is traversed.
  typedef rgtl_sqt_cell_geometry<D, Face> cell_geometry_type;

  // Construct empty tree with reference to full spatial structure
  // instance holding this per-face instance.
  rgtl_sqt_objects_face(internal_type& intern): derived(intern) {}

  // Pointer type for object set base class.
  typedef std::unique_ptr< rgtl_sqt_object_set<D> > sqt_object_set_ptr;

  // Pointer type for object set representation in this face.
  typedef std::unique_ptr< rgtl_sqt_object_set_face<D, Face> >
  sqt_object_set_face_ptr;

  // Spatial parameterization for this face.
  typedef rgtl_sqt_space<D, Face> space;

  // Convert a direction in this face to parameters.
  virtual void direction_to_parameters(double const d[D],
                                       double u[D-1]) const
  {
    space::direction_to_parameters(d, u);
  }

  // Build the spatial structure in this face.
  virtual void build(sqt_object_set_ptr& oa);

  // Query a ray contained in this face.
  virtual bool query_ray(double const d[D], double x[D], double* s) const;

  virtual void query_closest(double const p[D], int k, double const u[D-1],
                             rgtl_sqt_objects_query_closest<D>& qc) const;

#ifdef RGTL_SQT_OBJECTS_DEBUG_BUILD2
  class exception_stack
  {
   public:
    exception_stack(cell_location_type const& cell):
      cell_(cell), msg_(""), sz1_(0) {}
    void set_message(const char* msg) { this->msg_ = msg; }
    void set_size1(unsigned int sz1) { this->sz1_ = sz1; }
    ~exception_stack()
    {
      if (std::uncaught_exception())
      {
        std::cerr << "  " << cell_ << " (" << this->sz1_
                 << ") " << this->msg_ << '\n';
      }
    }
   private:
    cell_location_type const& cell_;
    const char* msg_;
    unsigned int sz1_;
  };
#endif

 private:
  // Recursive method to actually build the spatial structure in this face.
  void build(cell_geometry_type const& cell_geometry,
             cell_index_type cell_index,
             sqt_object_set_face_ptr& oa);

  // Recursive implementation of ray query.
  bool query_ray(cell_location_type const& cell, cell_index_type cell_index,
                 double const u[D-1], double const d[D], double x[D],
                 double* s) const;
  bool query_ray(int id, double const d[D], double x[D], double* s) const;

  // Access methods for internal representation.
  object_array_type const& object_array() const
  { return this->internal_.object_array(); }
  vnl_vector_fixed<double,D> const& origin() const
  { return this->internal_.origin(); }

  friend class rgtl_sqt_objects_query_closest_face<D, Face>;

  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr & rgtl_serialize_base<derived>(*this);
  }
};

//----------------------------------------------------------------------------
// Template container to hold a typed object for each face.
template <unsigned int D, unsigned int Face> class rgtl_sqt_objects_faces;

// Base class for face container.  Does not hold any faces but is used
// to terminate recursive instantiation.
template <unsigned int D>
class rgtl_sqt_objects_faces_base
{
 public:
  rgtl_sqt_objects_faces_base(rgtl_sqt_objects_internal<D>&,
                              rgtl_sqt_objects_face_base<D>* [(D<<1)]) {}
 private:
  friend class rgtl_serialize_access;
  template <class Serializer> void serialize(Serializer&) {}
};

// Template to lookup the next holding type for the next face.
template <unsigned int Face> struct rgtl_sqt_objects_face_next
{
  template <unsigned int D>
  struct get { typedef rgtl_sqt_objects_faces<D, Face-1> type; };
};

template <> struct rgtl_sqt_objects_face_next<0>
{
  template <unsigned int D>
  struct get { typedef rgtl_sqt_objects_faces_base<D> type; };
};

template <unsigned int D, unsigned int Face>
class rgtl_sqt_objects_faces
: public rgtl_sqt_objects_face_next<Face>::template get<D>::type
{
 public:
  // Superclass holds additional faces.
  typedef typename
  rgtl_sqt_objects_face_next<Face>::template get<D>::type derived;

  // Construct a face and provide a pointer to it.
  rgtl_sqt_objects_faces(rgtl_sqt_objects_internal<D>& intern,
                         rgtl_sqt_objects_face_base<D>* p[(D<<1)])
  : derived(intern, p), face_(intern)
  {
    p[Face] = &face_;
  }
 private:
  // Hold a face instance.
  rgtl_sqt_objects_face<D, Face> face_;

  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr & rgtl_serialize_base<derived>(*this);
    sr & face_;
  }
};

//----------------------------------------------------------------------------
template <unsigned int D>
class rgtl_sqt_objects_internal
{
 public:
  // Type holding original array of objects stored.
  typedef rgtl_object_array<D> object_array_type;

  // Type holding SQT-specific representation of object array.
  typedef rgtl_sqt_object_array<D> sqt_object_array_type;
  typedef std::unique_ptr<sqt_object_array_type> sqt_object_array_ptr;

  // Type holding SQT-specific representation of object set during
  // cell construction.
  typedef rgtl_sqt_object_set<D> sqt_object_set_type;
  typedef std::unique_ptr<sqt_object_set_type> sqt_object_set_ptr;

  // Default constructor.
  rgtl_sqt_objects_internal(object_array_type const& original_objs);

  // Constructor for SQT-specific representation of input object array.
  void build(sqt_object_array_type const& objs,
             double const origin[D], int ml, int mpl);

  // Get the implementation object for a given face index.
  rgtl_sqt_objects_face_base<D>& face(unsigned int i)
  {
    return *faces_[i];
  }
  rgtl_sqt_objects_face_base<D> const& face(unsigned int i) const
  {
    return *faces_[i];
  }

  // Get tree building parameters.
  int max_level() const { return this->max_level_; }
  int max_per_leaf() const { return this->max_per_leaf_; }

  // Get the original object array.
  object_array_type const& object_array() const
  { return this->object_array_; }

  // Get the SQT guard position.  This is also the origin of all sight rays.
  vnl_vector_fixed<double,D> const& origin() const { return this->origin_; }
  double origin(unsigned int a) const { return this->origin_[a]; }

  // External interface method implementations.
  bool query_ray(double const d[D], double x[D], double* s) const;
  int query_closest(double const p[D], int k, int* ids,
                    double* squared_distances, double* points,
                    double bound_squared) const;

  int number_of_objects() const
  {
    return this->object_array_.number_of_objects();
  }

  bool object_closest_point(int id,
                            double const x[D],
                            double y[D],
                            double bound_squared) const
  {
    return this->object_array_.object_closest_point(id, x, y, bound_squared);
  }

  bool visit_once(int id) const
  {
    return this->object_once_.visit(id);
  }

#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
  void set_query_closest_debug(bool b) { this->query_closest_debug_ = b; }
#else
  void set_query_closest_debug(bool) {}
#endif

 private:
  // Container holding typed face instances.
  rgtl_sqt_objects_faces<D, (D<<1)-1> faces_container_;

  // Array of face instance pointers to allow random access to face
  // implementations.
  rgtl_sqt_objects_face_base<D>* faces_[(D<<1)];

  // Reference to set of objects held in spatial structure.
  object_array_type const& object_array_;

  // Keep track of objects visited on a per-query basis.
  rgtl_object_once object_once_;

  // The maximum subdivision level.
  int max_level_;

  // The maximum number of objects per leaf.
  int max_per_leaf_;

  // The guard location of the star-shaped region.
  vnl_vector_fixed<double,D> origin_;

  friend class rgtl_sqt_objects_query_closest<D>;

#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
 public:
  // Enable closest point query output only after the distance
  // transform has finished.
  bool query_closest_debug_;
#endif

  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr & faces_container_;
    sr & object_once_;
    sr & max_level_;
    sr & max_per_leaf_;
    sr & origin_;
  }
};

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_sqt_objects_internal<D>
::rgtl_sqt_objects_internal(object_array_type const& original_objs):
  faces_container_(*this, faces_),
  object_array_(original_objs),
  object_once_(),
  max_level_(0), max_per_leaf_(0)
{
}

//----------------------------------------------------------------------------
template <unsigned int D>
void rgtl_sqt_objects_internal<D>::build(sqt_object_array_type const& objs,
                                         double const origin[D],
                                         int ml, int mpl)
{
  assert(&this->object_array_ == &objs.original());
  this->object_once_.set_number_of_objects(
    this->object_array_.number_of_objects());
  this->max_level_ = ml;
  this->max_per_leaf_ = mpl;

#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
  this->query_closest_debug_ = false;
#endif

  for (unsigned int a=0; a < D; ++a)
  {
    this->origin_[a] = origin[a];
  }

  int n = this->number_of_objects();
  std::vector<int> ids(n);
  for (int i=0; i < n; ++i)
  {
    ids[i] = i;
  }

  for (unsigned int i=0; i < (D<<1); ++i)
  {
    sqt_object_set_ptr soa(objs.new_set(this->origin_.data_block(), i));
    this->face(i).build(soa);
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool
rgtl_sqt_objects_internal<D>::query_ray(double const d[D],
                                        double x[D], double* s) const
{
  unsigned int f = rgtl_sqt_space_base<D>::direction_to_face(d);
  return this->face(f).query_ray(d, x, s);
}

//----------------------------------------------------------------------------
template <unsigned int D>
class rgtl_sqt_objects_query_closest
{
 public:
  typedef rgtl_sqt_objects_internal<D> internal_type;
  rgtl_sqt_objects_query_closest(internal_type const& intern,
                                 double const p[D], int k,
                                 double bound_squared)
  : internal_(intern), best_(k)
  {
#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
    this->checked_count_ = 0;
#endif
    this->center_depth_squared_ = 0;
    for (unsigned int a=0; a < D; ++a)
    {
      this->center_direction_[a] = p[a] - this->internal_.origin(a);
      this->center_depth_squared_ +=
        this->center_direction_[a]*this->center_direction_[a];
    }
    this->center_face_ =
      rgtl_sqt_space_base<D>::direction_to_face(this->center_direction_);
    this->internal_.face(this->center_face_).direction_to_parameters(
      this->center_direction_, this->center_parameters_);
    this->center_depth_ = std::sqrt(this->center_depth_squared_);
    this->set_bound(bound_squared);
#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
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
  }

  void check_object(double const p[D], int k, int id);
  int get_result(int k, int* ids, double* squared_distances, double* points);

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

  void update_bound(double bound);
  void set_bound(double bound);

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

  // Compute the distance between two points.
  static double compute_distance(double const p[D],
                                 double const q[D])
  {
    return std::sqrt(compute_distance_squared(p, q));
  }

  // The internal sqt objects representation.
  internal_type const& internal_;

  // Keep a sorted list of the best k squared distances.  Use -1 to
  // indicate no object yet found.
  std::vector<closest_object_entry> best_;

  // Keep track of the current squared distance bound.
  double bound_;

  // The parameters of the query point direction.
  unsigned int center_face_;
  double center_parameters_[D-1];

  // Keep track of the current bounding sphere.
  double center_depth_;
  double center_depth_squared_;
  double center_direction_[D];
  double radius_;

#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
  // Keep track of the number of object queries made.
  int checked_count_;
#endif
};

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_sqt_objects_query_closest<D>
::check_object(double const p[D], int k, int id)
{
#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
  ++this->checked_count_;
  if (this->internal_.query_closest_debug_)
  {
    std::cout << "  checking object id " << id << std::endl;
  }
#endif
  // Get the squared distance to this object and use it only
  // if it is smaller than the current distance bound.
  double q[D];
  double distance_squared;
  if (this->internal_.object_closest_point(id, p, q, this->bound_) &&
      (distance_squared = this->compute_distance_squared(p, q),
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
    this->update_bound(best[k-1].distance_squared);
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
int
rgtl_sqt_objects_query_closest<D>
::get_result(int k, int* ids, double* squared_distances, double* points)
{
#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
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
#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
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
rgtl_sqt_objects_query_closest<D>
::set_bound(double bound)
{
  this->bound_ = bound;
  this->radius_ = std::sqrt(bound);
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_sqt_objects_query_closest<D>
::update_bound(double bound)
{
  if (bound >= 0 && bound < this->bound_)
  {
    this->set_bound(bound);
#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
    if (this->internal_.query_closest_debug_)
    {
      std::cout << " Reduced bound to " << this->bound_ << std::endl;
    }
#endif
  }
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
class rgtl_sqt_objects_query_closest_face
{
 public:
  typedef rgtl_sqt_objects_face<D, Face> face_type;
  typedef rgtl_sqt_objects_internal<D> internal_type;
  typedef rgtl_sqt_objects_query_closest<D> qc_type;
  typedef typename face_type::cell_bounds_type cell_bounds_type;
  typedef typename face_type::cell_location_type cell_location_type;
  typedef typename face_type::cell_index_type cell_index_type;
  typedef typename face_type::child_index_type child_index_type;
  typedef typename face_type::leaf_data_type leaf_data_type;
  typedef typename face_type::node_data_type node_data_type;
  typedef rgtl_sqt_objects_cell_data<D> cell_data_type;
  typedef typename leaf_data_type::cone_bounds_type cone_bounds_type;

  rgtl_sqt_objects_query_closest_face(face_type const& face, qc_type& qc):
    internal_(qc.internal_), face_(face), qc_(qc) {}

  void query(double const p[D], int k, double const u[D-1])
  {
    // Recursively visit the tree starting at the root.
    this->query_impl(cell_location_type(Face), cell_index_type(), p, k, u);
  }

  void query_impl(cell_location_type const& cell,
                  cell_index_type cell_index,
                  double const p[D], int k, double const u[D-1]);

  double compute_disc_nearest(double query_distance,
                              double query_angle, double query_depth,
                              double cone_angle, double clip_depth) const;
  bool disjoint(cell_data_type const* cell_data,
                double& nearest_squared) const;

  static double dot(cone_bounds_type const u[D], double const v[D])
  {
    double d = 0;
    for (unsigned int a=0; a < D; ++a)
    {
      d += u[a]*v[a];
    }
    return d;
  }

 private:
  internal_type const& internal_;
  face_type const& face_;
  qc_type& qc_;
};

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_objects_query_closest_face<D, Face>
::query_impl(cell_location_type const& cell, cell_index_type cell_index,
             double const p[D], int k, double const u[D-1])
{
#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
  if (this->internal_.query_closest_debug_)
  {
    std::cout << "Considering cell " << cell << std::endl;
  }
#endif

  if (this->face_.tree().has_children(cell_index))
  {
    // Perform a fast-reject test if the current bounding sphere does
    // not intersect the cell bounding cone.
    double nearest_squared;
    if (node_data_type const* nd =
        this->face_.tree().get_node_data(cell_index))
    {
      if (this->disjoint(nd, nearest_squared))
      {
#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
        if (this->internal_.query_closest_debug_)
        {
          std::cout << " node is disjoint" << std::endl;
        }
#endif
        return;
      }
    }
    else
    {
      std::abort();
    }

    // This is a node.  Compute the child containing the ray, if any.
    // Use it to order child visitation to visit cells closer to the
    // query point first.
    unsigned int child_xor = 0;
    if (u)
    {
      cell_bounds_type upper(cell.get_child(child_index_type((1<<(D-1))-1)));
      for (unsigned int j=0; j < D-1; ++j)
      {
        if (u[j] >= upper.origin(j))
        {
          child_xor |= (1<<j);
        }
      }
    }

    // Recursively query each child.
    for (unsigned int i = 0;
         i < (1<<(D-1)) && this->qc_.bound_ >= nearest_squared; ++i)
    {
      child_index_type child_index(i^child_xor);
      this->query_impl(cell.get_child(child_index),
                       this->face_.tree().get_child(cell_index,
                                                    child_index),
                       p, k, u);
    }
  }
  else if (leaf_data_type const* ld =
           this->face_.tree().get_leaf_data(cell_index))
  {
    // Perform a fast-reject test if the current bounding sphere does
    // not intersect the cell bounding cone.
    double nearest_squared;
    if (this->disjoint(ld, nearest_squared))
    {
#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
      if (this->internal_.query_closest_debug_)
      {
        std::cout << " leaf is disjoint" << std::endl;
      }
#endif
      return;
    }

#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
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

    // Test the objects in this leaf.
    typedef typename leaf_data_type::index_type index_type;
    for (index_type i=ld->index_begin;
         i != ld->index_end && this->qc_.bound_ >= nearest_squared; ++i)
    {
      int id = this->face_.object_ids_[i];
      if (this->internal_.visit_once(id))
      {
        this->qc_.check_object(p, k, id);
      }
    }
  }
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
double
rgtl_sqt_objects_query_closest_face<D, Face>
::compute_disc_nearest(double query_distance,
                       double query_angle, double query_depth,
                       double cone_angle, double clip_depth) const
{
  // The disc can be formed by intersecting a cylinder and the clip
  // plane.  If the query point is outside the cylinder it is closest
  // to the disc boundary.  Otherwise it is closest to the disc
  // interior.
  double query_r = std::sin(query_angle)*query_distance;
  double cylinder_r = std::tan(cone_angle)*clip_depth;
  double distance_to_plane = query_depth - clip_depth;
  double distance_squared = distance_to_plane*distance_to_plane;
  if (query_r > cylinder_r)
  {
    // Query point is closest to the disc boundary.
    double distance_to_cylinder = query_r - cylinder_r;
    distance_squared += distance_to_cylinder*distance_to_cylinder;
  }
  else
  {
    // Query point is closest to the disc interior.
  }
  return distance_squared;
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
bool
rgtl_sqt_objects_query_closest_face<D, Face>
::disjoint(cell_data_type const* cell_data, double& nearest_squared) const
{
  // Test the current bounding sphere against the cell depth range.
  cone_bounds_type const* a = cell_data->cone_axis;
  double const* c = this->qc_.center_direction_;
  double a_dot_c = dot(a, c);
  if (a_dot_c+this->qc_.radius_ < cell_data->depth_min)
  {
    // The bounding sphere is completely in front of the cell.
    return true;
  }
  if (a_dot_c-this->qc_.radius_ > cell_data->depth_max)
  {
    // The bounding sphere is completely behind the cell.
    return true;
  }

  // Test the current bounding sphere against the cell bounding cone.
  double const beta = cell_data->cone_angle;
  double const theta = std::acos(a_dot_c / this->qc_.center_depth_);
  if (theta <= beta)
  {
    // The sphere center is inside the cone.
    if (a_dot_c < cell_data->depth_min)
    {
      // Query point is closest to near disc interior.
      double distance_to_near_plane = cell_data->depth_min - a_dot_c;
      nearest_squared = distance_to_near_plane*distance_to_near_plane;
    }
    else if (a_dot_c > cell_data->depth_max)
    {
      // Query point is closest to the far disc.
      nearest_squared = this->compute_disc_nearest(this->qc_.center_depth_,
                                                   theta, a_dot_c, beta,
                                                   cell_data->depth_max);
    }
    else
    {
      // Query point is inside the clipped cone.
      nearest_squared = 0;
    }

    // If the distance to the nearest is at least as far as the
    // current bound then the sphere is disjoint.
    return nearest_squared >= this->qc_.bound_;
  }

  double const phi = beta + vnl_math::pi / 2;
  if (theta >= phi)
  {
    // The sphere center is closest to the cone tip.
    if (this->qc_.center_depth_squared_ > this->qc_.bound_)
    {
      // The sphere is outside the cone.
      return true;
    }
    else
    {
      // Query point is closest to the near disc.
      nearest_squared = this->compute_disc_nearest(this->qc_.center_depth_,
                                                   theta, a_dot_c, beta,
                                                   cell_data->depth_min);

      // If the distance to the nearest is at least as far as the
      // current bound then the sphere is disjoint.
      return nearest_squared >= this->qc_.bound_;
    }
  }

  // The sphere center is closest to the cone surface.
  double c_dot_c = this->qc_.center_depth_squared_;
  double distance_to_cone_surface =
    (std::cos(phi)*a_dot_c + std::sin(phi)*std::sqrt(c_dot_c - a_dot_c*a_dot_c));
  if (this->qc_.radius_ < distance_to_cone_surface)
  {
    // The sphere is outside the cone.
    return true;
  }
  else
  {
    // The sphere center is outside the cone.
    // Test against the far normal cone.
    double const alpha = vnl_math::pi/2 - beta;
    double cos_beta = std::cos(beta);
    double cos_beta_2 = cos_beta * cos_beta;
    double scale_far = cell_data->depth_max / cos_beta_2;
    double cos_gamma_far =
      ((scale_far - a_dot_c) /
       std::sqrt(c_dot_c - 2*scale_far*a_dot_c + scale_far*scale_far));
    double gamma_far = std::acos(cos_gamma_far);
    if (gamma_far > alpha)
    {
      // Query point is behind the far normal cone and outside the
      // primal cone.  It is closest to the far disc boundary.
      double query_r = std::sin(theta)*this->qc_.center_depth_;
      double cylinder_r = std::tan(beta)*cell_data->depth_max;
      double distance_to_far_plane = a_dot_c - cell_data->depth_max;
      double distance_to_far_cylinder = query_r - cylinder_r;
      nearest_squared =
        (distance_to_far_plane*distance_to_far_plane +
         distance_to_far_cylinder*distance_to_far_cylinder);
    }
    else
    {
      // Test against the near normal cone.
      double scale_near = cell_data->depth_min / cos_beta_2;
      double cos_gamma_near =
        ((scale_near - a_dot_c) /
         std::sqrt(c_dot_c - 2*scale_near*a_dot_c + scale_near*scale_near));
      double gamma_near = std::acos(cos_gamma_near);
      if (gamma_near < alpha)
      {
        // Query point is in front of the near normal cone and outside
        // the primal cone.  It is closest to the near disc.
        nearest_squared = this->compute_disc_nearest(this->qc_.center_depth_,
                                                     theta, a_dot_c, beta,
                                                     cell_data->depth_min);
      }
      else
      {
        // Query point is closest to the cone surface.
        nearest_squared = distance_to_cone_surface*distance_to_cone_surface;
      }
    }

    // If the distance to the nearest is at least as far as the
    // current bound then the sphere is disjoint.
    return nearest_squared >= this->qc_.bound_;
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
int
rgtl_sqt_objects_internal<D>
::query_closest(double const p[D], int k, int* ids,
                double* squared_distances, double* points,
                double bound_squared) const
{
  // Setup initial query sphere radius.
  if (bound_squared < 0)
  {
    bound_squared = std::numeric_limits<double>::infinity();
  }
#ifdef RGTL_SQT_OBJECTS_DEBUG_QUERY
  else if (this->query_closest_debug_)
  {
    std::cout << "User initial bound " << bound_squared << std::endl;
  }
#endif

  // Keep track objects already tested to avoid duplicating tests.
  this->object_once_.reset();

  // Collect the results for each face.  Start with the face
  // containing the query point.
  rgtl_sqt_objects_query_closest<D> qc(*this, p, k, bound_squared);
  unsigned int f = qc.center_face_;
  this->face(f).query_closest(p, k, qc.center_parameters_, qc);
  for (unsigned int i=0; i < (D<<1); ++i)
  {
    if (i != f)
    {
      this->face(i).query_closest(p, k, 0, qc);
    }
  }
  return qc.get_result(k, ids, squared_distances, points);
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_objects_face<D, Face>::build(sqt_object_set_ptr& oa)
{
  // Down-cast the pointer to get the per-face object set.
  sqt_object_set_face_ptr
    oaf(static_cast<rgtl_sqt_object_set_face<D, Face>*>(oa.release()));

  // Construct the tree starting at the root cell.
  this->build(cell_geometry_type(cell_location_type(Face)),
              cell_index_type(), oaf);
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_objects_face<D, Face>
::build(cell_geometry_type const& cell_geometry, cell_index_type cell_index,
        sqt_object_set_face_ptr& oa)
{
  cell_location_type const& cell = cell_geometry.location();
#ifdef RGTL_SQT_OBJECTS_DEBUG_BUILD2
  exception_stack es(cell); (void)es;
#endif
#ifdef RGTL_SQT_OBJECTS_DEBUG_BUILD
  std::cout << "Considering " << oa->number_of_objects()
           << " objects in cell " << cell << std::endl;
#endif
  bool tooDeep = cell.level() >= this->internal_.max_level();
  bool tooMany = oa->number_of_objects() > this->internal_.max_per_leaf();
  if (tooMany && !tooDeep)
  {
    // Divide this cell.
    this->tree_.subdivide(cell_index);

    // Compute the bounding cone for this node.
    node_data_type node_data;
    cell_geometry.get_cone(node_data.cone_axis, node_data.cone_angle);

    // Compute the bounding depth range for this node.
    oa->get_depth_range(node_data.cone_axis,
                        node_data.depth_min, node_data.depth_max);

    // Store data in the node.
    this->tree_.set_node_data(cell_index, &node_data);

    // Distribute objects into children.
    sqt_object_set_face_ptr children[1<<(D-1)];
    oa->split(cell_geometry, children);

    // Erase memory used by the object array for this cell.
    oa.reset(0);

    // Compute the child cell geometries.
    typename cell_geometry_type::children_type child_geometry(cell_geometry);

    // Build the children recursively.
    for (child_index_type i(0); i < (1<<(D-1)); ++i)
    {
      this->build(child_geometry[i],
                  this->tree_.get_child(cell_index, i),
                  children[i]);
    }
  }
  else if (oa->number_of_objects() > 0)
  {
    // We will not divide this cell further.
    // Store the objects for this cell.
#ifdef RGTL_SQT_OBJECTS_DEBUG_BUILD
    std::cout << "Storing " << oa->number_of_objects() << " objects in cell "
             << cell << std::endl;
#endif
    typedef typename leaf_data_type::index_type index_type;
    index_type index_begin = index_type(this->object_ids_.size());
    for (int i=0; i < oa->number_of_objects(); ++i)
    {
      this->object_ids_.push_back(oa->original_id(i));
    }
    index_type index_end = index_type(this->object_ids_.size());
    leaf_data_type leaf_data(index_begin, index_end);

    // Compute the bounding cone for this leaf.
    cell_geometry.get_cone(leaf_data.cone_axis, leaf_data.cone_angle);

    // Compute the bounding depth range for this leaf.
    oa->get_depth_range(leaf_data.cone_axis,
                        leaf_data.depth_min, leaf_data.depth_max);

    // Store the data in the leaf.
    this->tree_.set_leaf_data(cell_index, &leaf_data);
  }
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
bool
rgtl_sqt_objects_face<D, Face>::query_ray(double const d[D],
                                          double x[D], double* s) const
{
  double u[D-1];
  space::direction_to_parameters(d, u);
  return this->query_ray(cell_location_type(Face), cell_index_type(),
                         u, d, x, s);
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
bool
rgtl_sqt_objects_face<D, Face>::query_ray(cell_location_type const& cell,
                                          cell_index_type cell_index,
                                          double const u[D-1],
                                          double const d[D],
                                          double x[D],
                                          double* s) const
{
  if (this->tree_.has_children(cell_index))
  {
    // This is a node.  Compute the child containing the ray.
    child_index_type child_index(0);
    cell_bounds_type upper(cell.get_child(child_index_type((1<<(D-1))-1)));
    for (unsigned int j=0; j < D-1; ++j)
    {
      if (u[j] >= upper.origin(j))
      {
        child_index |= (1<<j);
      }
    }

    // Recurse into the chosen child.
    return this->query_ray(cell.get_child(child_index),
                           this->tree_.get_child(cell_index, child_index),
                           u, d, x, s);
  }
  else if (leaf_data_type const* ld = this->tree_.get_leaf_data(cell_index))
  {
    // Test the objects in this leaf.
    bool found = false;
    double min_s = std::numeric_limits<double>::infinity();
    typedef typename leaf_data_type::index_type index_type;
    for (index_type i=ld->index_begin; i != ld->index_end; ++i)
    {
      int id = this->object_ids_[i];
      double cur_x[D];
      double cur_s;
      if (this->query_ray(id, d, cur_x, &cur_s))
      {
        found = true;
        if (cur_s < min_s)
        {
          min_s = cur_s;
          if (x)
          {
            for (unsigned int a=0; a < D; ++a)
            {
              x[a] = cur_x[a];
            }
          }
          if (s)
          {
            *s = min_s;
          }
        }
      }
    }
    return found;
  }
  return false;
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
bool
rgtl_sqt_objects_face<D, Face>::query_ray(int id,
                                          double const d[D],
                                          double x[D],
                                          double* s) const
{
  return
    this->object_array().object_intersects_ray(id,
                                               this->origin().data_block(),
                                               d, x, s);
}

//----------------------------------------------------------------------------
template <unsigned int D, unsigned int Face>
void
rgtl_sqt_objects_face<D, Face>
::query_closest(double const p[D], int k, double const u[D-1],
                rgtl_sqt_objects_query_closest<D>& qc) const
{
  rgtl_sqt_objects_query_closest_face<D, Face> qcf(*this, qc);
  qcf.query(p, k, u);
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_sqt_objects<D>::rgtl_sqt_objects(object_array_type const& oa):
  internal_(new rgtl_sqt_objects_internal<D>(oa))
{
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_sqt_objects<D>::rgtl_sqt_objects(sqt_object_array_type const& soa,
                                      double const origin[D],
                                      int ml, int mpl):
  internal_(new rgtl_sqt_objects_internal<D>(soa.original()))
{
  this->build(soa, origin, ml, mpl);
}

//----------------------------------------------------------------------------
template <unsigned int D>
rgtl_sqt_objects<D>::~rgtl_sqt_objects()
{
}

//----------------------------------------------------------------------------
template <unsigned int D>
void rgtl_sqt_objects<D>::build(sqt_object_array_type const& soa,
                                double const origin[D],
                                int ml, int mpl)
{
  this->internal_->build(soa, origin, ml, mpl);
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool
rgtl_sqt_objects<D>::query_ray(double const d[D],
                               double x[D], double* s) const
{
  return this->internal_->query_ray(d, x, s);
}

//----------------------------------------------------------------------------
template <unsigned int D>
int
rgtl_sqt_objects<D>::query_closest(double const p[D], int k, int* ids,
                                   double* squared_distances, double* points,
                                   double bound_squared) const
{
  return this->internal_->query_closest(p, k, ids, squared_distances, points,
                                        bound_squared);
}

//----------------------------------------------------------------------------
template <unsigned int D>
double rgtl_sqt_objects<D>::origin(unsigned int a) const
{
  assert(a < D);
  return this->internal_->origin(a);
}

//----------------------------------------------------------------------------
template <unsigned int D>
vnl_vector_fixed<double,D> const& rgtl_sqt_objects<D>::origin() const
{
  return this->internal_->origin();
}

//----------------------------------------------------------------------------
template <unsigned int D>
void rgtl_sqt_objects<D>::compute_bounds(double bounds[D][2]) const
{
  // Start with the bounds of the objects.
  this->internal_->object_array().compute_bounds(bounds);

  // Update the bounds to contain the origin.
  vnl_vector_fixed<double,D> const& p = this->origin();
  for (unsigned int a=0; a < D; ++a)
  {
    if (p[a] < bounds[a][0])
    {
      bounds[a][0] = p[a];
    }
    if (p[a] > bounds[a][1])
    {
      bounds[a][1] = p[a];
    }
  }
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool
rgtl_sqt_objects<D>
::has_children(unsigned int face, cell_index_type cell_index) const
{
  assert(face < (D<<1));
  return this->internal_->face(face).tree().has_children(cell_index);
}

//----------------------------------------------------------------------------
template <unsigned int D>
typename rgtl_sqt_objects<D>::cell_index_type
rgtl_sqt_objects<D>
::get_child(unsigned int face, cell_index_type cell_index,
            child_index_type child) const
{
  assert(face < (D<<1));
  return this->internal_->face(face).tree().get_child(cell_index, child);
}

//----------------------------------------------------------------------------
template <unsigned int D>
bool
rgtl_sqt_objects<D>
::get_depth_range(unsigned int face, cell_index_type cell_index,
                  double& depth_min, double& depth_max) const
{
  assert(face < (D<<1));
  typedef rgtl_sqt_objects_face_base<D> face_base;
  typedef typename face_base::tree_type tree_type;
  typedef typename face_base::node_data_type node_data_type;
  typedef typename face_base::leaf_data_type leaf_data_type;
  tree_type const& tree = this->internal_->face(face).tree();
  if (tree.has_children(cell_index))
  {
    if (node_data_type const* nd = tree.get_node_data(cell_index))
    {
      depth_min = nd->depth_min;
      depth_max = nd->depth_max;
      return true;
    }
  }
  else
  {
    if (leaf_data_type const* ld = tree.get_leaf_data(cell_index))
    {
      depth_min = ld->depth_min;
      depth_max = ld->depth_max;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
template <unsigned int D>
void
rgtl_sqt_objects<D>
::set_query_closest_debug(bool b)
{
  this->internal_->set_query_closest_debug(b);
}

//----------------------------------------------------------------------------
template <unsigned int D>
template <class Serializer>
void rgtl_sqt_objects<D>::serialize(Serializer& sr)
{
  sr & *(this->internal_);
}

//----------------------------------------------------------------------------
#define RGTL_SQT_OBJECTS_INSTANTIATE(D) \
  template class rgtl_sqt_objects_internal< D >; \
  template class rgtl_sqt_objects< D >; \
  template void rgtl_sqt_objects< D > \
    ::serialize<rgtl_serialize_ostream>(rgtl_serialize_ostream&); \
  template void rgtl_sqt_objects< D > \
    ::serialize<rgtl_serialize_istream>(rgtl_serialize_istream&)
#define RGTL_SQT_OBJECTS_FACE_INSTANTIATE(D, Face) \
  template class rgtl_sqt_objects_face< D , Face >

#endif
