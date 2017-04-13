#ifndef rgtl_separating_axis_h
#define rgtl_separating_axis_h
//:
// \file
// \brief Represent a potential separating axis and a projection onto it.
// \author Brad King
// \date February 2007
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "rgtl_separating_axis_projection.h"

#include <vnl/vnl_vector_fixed.h>

//: Represent a potential separating axis and the current projection.
//
// This class wraps up some basic operations of projecting an object
// onto a potential separating axis.
template <unsigned int D, class T>
class rgtl_separating_axis
{
 public:
  typedef rgtl_separating_axis_projection<T> projection_type;

  //: Default constructor is meant only for initializing arrays of axes.
  //  Assignment from a fully-constructed object should be
  //  used to initialize the object later.
  rgtl_separating_axis(): projection_() {}

  //: Construct with a given axis and empty projection range.
  rgtl_separating_axis(T const a[D]): projection_()
  {
    this->init(a);
  }
  rgtl_separating_axis(vnl_vector_fixed<T, D> const& a): projection_()
  {
    this->init(a.data_block());
  }

  //: Update the lower bound given a point on the object.
  void update_lower(T const p[D])
  {
    this->projection_.update_lower(this->project(p));
  }
  void update_lower(vnl_vector_fixed<T, D> const& p)
  {
    this->update_lower(p.data_block());
  }

  //: Update the upper bound given a point on the object.
  void update_upper(T const p[D])
  {
    this->projection_.update_upper(this->project(p));
  }
  void update_upper(vnl_vector_fixed<T, D> const& p)
  {
    this->update_upper(p.data_block());
  }

  //: Update the lower and upper bounds given a point on the object.
  void update(T const p[D])
  {
    this->projection_.update(this->project(p));
  }
  void update(vnl_vector_fixed<T, D> const& p)
  {
    this->update(p.data_block());
  }

  //: Update the lower or upper bound for a point on the plane at infinity in the given direction.
  void update_ray(T const p[D])
  {
    this->projection_.update_ray(this->project(p));
  }
  void update_ray(vnl_vector_fixed<T, D> const& p)
  {
    this->update_ray(p.data_block());
  }

  //: Update the lower and upper bound for a sphere centered at the given point with the given radius.
  void update_sphere(T const c[D], T r)
  {
    this->projection_.update_sphere(this->project(c), r);
  }
  void update_sphere(vnl_vector_fixed<T, D> const& c, T r)
  {
    this->update_sphere(c.data_block(), r);
  }

  //: Set the lower or upper bound explicitly.
  void set_lower(T p) { this->projection_.set_lower(p); }
  void set_upper(T p) { this->projection_.set_upper(p); }

  //: Get the axis of projection.
  T const* axis() const { return this->axis_; }

  //: Get the current projection range.
  projection_type const& projection() const { return this->projection_; }

 private:
  void init(T const a[D])
  {
    for (unsigned int i=0; i < D; ++i)
    {
      this->axis_[i] = a[i];
    }
  }
  T project(T const p[D])
  {
    T d(0);
    for (unsigned int i=0; i < D; ++i)
    {
      d += this->axis_[i]*p[i];
    }
    return d;
  }
  projection_type projection_;
  T axis_[D];
 private:
  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr& projection_;
    sr& axis_;
  }
};

#endif // rgtl_separating_axis_h
