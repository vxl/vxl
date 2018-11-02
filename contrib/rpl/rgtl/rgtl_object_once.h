/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_object_once_h
#define rgtl_object_once_h

//:
// \file
// \brief Manage object visitation marks efficiently.
// \author Brad King
// \date April 2007

class rgtl_serialize_access;

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Efficiently keep track of objects visited to avoid repeat visits.
class rgtl_object_once
{
  typedef unsigned int mark_type;
  typedef std::vector<mark_type>::size_type size_type;
public:
  //: Construct for zero objects.
  rgtl_object_once();

  //: Construct for a fixed number of objects.
  rgtl_object_once(size_type n);

  //: Set the number of objects to track.
  void set_number_of_objects(size_type n);

  //: Reset every object to not visited.
  void reset() const;

  //: Return whether to visit an object.  Marks the object as done.
  bool visit(size_type i) const;

  //: Return whether an object has already been visited.
  bool visited(size_type i) const;

private:
  // TODO: Reimplement this to use per-object thread-local storage.
  mutable std::vector<mark_type> marks_;
  mutable mark_type mark_;

  friend class rgtl_serialize_access;
  template <class Serializer> void serialize(Serializer& sr);
  template <class Serializer> void serialize_load(Serializer& sr);
  template <class Serializer> void serialize_save(Serializer& sr);
};

#endif
