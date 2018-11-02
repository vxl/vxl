#ifndef rgtl_separating_axis_projection_h
#define rgtl_separating_axis_projection_h
//:
// \file
// \brief Represent the projection of an object onto a separating axis.
// \author Brad King
// \date February 2007
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <limits>
#include "rgtl_serialize_access.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Represent the projection of an object onto a separating axis.
template <class T>
class rgtl_separating_axis_projection
{
  typedef std::numeric_limits<T> limits;
 public:
  //: Default construct with the range +inf..-inf which is an inside-out projection on the entire line.
  rgtl_separating_axis_projection(): lower_(limits::infinity()),
                                     upper_(-limits::infinity()) {}

  //: Construct with an explicitly provided initial range.
  rgtl_separating_axis_projection(T l, T r): lower_(l), upper_(r) {}

  //: Update the lower bound given a projected point on the object.
  void update_lower(T p)
  {
    if (p < this->lower_)
    {
      this->lower_ = p;
    }
  }

  //: Update the upper bound given a projected point on the object.
  void update_upper(T p)
  {
    if (p > this->upper_)
    {
      this->upper_ = p;
    }
  }

  //: Update the lower and upper bounds given a projected point on the object.
  void update(T p)
  {
    this->update_lower(p);
    this->update_upper(p);
  }

  //: Update the lower or upper bound for a point at infinity in the given direction.
  void update_ray(T p)
  {
    if (p < 0)
    {
      this->lower_ = -limits::infinity();
    }
    else if (p > 0)
    {
      this->upper_ = limits::infinity();
    }
  }

  //: Update the lower and upper bound for an interval centered at the given point with the given radius.
  void update_sphere(T c, T r)
  {
    this->update_lower(c-r);
    this->update_upper(c+r);
  }

  //: Set the lower and upper bounds of the projection range.
  void set_lower(T l) { this->lower_ = l; }
  void set_upper(T u) { this->upper_ = u; }

  //: Get the lower and upper bounds of the projection range.
  T lower() const { return this->lower_; }
  T upper() const { return this->upper_; }

  //: Return whether or not two projections are disjoint.
  static bool disjoint(rgtl_separating_axis_projection<T> const& a,
                       rgtl_separating_axis_projection<T> const& b)
  {
    return a.upper() < b.lower() || a.lower() > b.upper();
  }

 private:
  //: Store the lower and upper bounds of the projection range.
  T lower_;
  T upper_;

  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
  {
    sr& lower_;
    sr& upper_;
  }
};

#endif // rgtl_separating_axis_projection_h
