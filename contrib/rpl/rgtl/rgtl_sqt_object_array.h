/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt) */

#ifndef rgtl_sqt_object_array_h
#define rgtl_sqt_object_array_h
//:
// \file
// \brief Abstract interface for an object array during SQT construction.
// \author Brad King
// \date April 2007

#include <iostream>
#include <memory>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <unsigned int D> class rgtl_object_array;
template <unsigned int D> class rgtl_sqt_cell_location;
template <unsigned int D, unsigned int Face> class rgtl_sqt_cell_geometry;

//----------------------------------------------------------------------------
//: Base class for a set of objects in D dimensions belonging to an SQT cell during construction.
template <unsigned int D>
class rgtl_sqt_object_set
{
  typedef rgtl_sqt_object_set<D> self_type;
 public:
  //: Pointer to an instance of this class.
  typedef std::unique_ptr<self_type> sqt_object_set_ptr;

  //: Need a virtual destructor.
  virtual ~rgtl_sqt_object_set() {}
};

//----------------------------------------------------------------------------
//: Base class for a set of objects in D dimensions belonging to an SQT cell in a particular face during construction.
template <unsigned int D, unsigned int Face>
class rgtl_sqt_object_set_face: public rgtl_sqt_object_set<D>
{
  typedef rgtl_sqt_object_set_face<D, Face> self_type;
 public:
  //: Type representing SQT cell logical location.
  typedef rgtl_sqt_cell_location<D> cell_location_type;

  //: Type representing SQT cell geometry.
  typedef rgtl_sqt_cell_geometry<D, Face> cell_geometry_type;

  //: Pointer to an instance of this class.
  typedef std::unique_ptr<self_type> sqt_object_set_face_ptr;

  //: Get the number of objects in this set.
  virtual int number_of_objects() const = 0;

  //: Get the id of the original object that was copied or split to form an object in the set.
  virtual int original_id(int) const = 0;

  //: Split the current set of objects into the children of the current SQT cell.
  virtual void split(cell_geometry_type const& cell_geometry,
                     sqt_object_set_face_ptr children[1<<(D-1)]) = 0;

  //: Get the range of projection of objects in this set onto the given direction.
  virtual void get_depth_range(float const direction[D],
                               float& depth_min,
                               float& depth_max) const = 0;
  virtual void get_depth_range(double const direction[D],
                               double& depth_min,
                               double& depth_max) const = 0;
};

//----------------------------------------------------------------------------
//: Abstract interface for an object array given to rgtl_sqt_objects at the start of its construction.
template <unsigned int D>
class rgtl_sqt_object_array
{
 public:
  //: General-purpose object array interface type.
  typedef rgtl_object_array<D> object_array_type;

  //: Pointer to a set of objects in once cell during construction.
  typedef std::unique_ptr< rgtl_sqt_object_set<D> > sqt_object_set_ptr;

  //: Get a set of the objects that belong to the given SQT face with respect to the given SQT origin.
  virtual sqt_object_set_ptr new_set(double const origin[D],
                                     unsigned int face) const = 0;

  //: Get the original array of objects.
  virtual object_array_type const& original() const = 0;

  //: Need a virtual destructor.
  virtual ~rgtl_sqt_object_array() {}
};

#endif
