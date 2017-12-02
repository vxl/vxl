/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_serialize_base_h
#define rgtl_serialize_base_h

//:
// \file
// \brief Helper class to serialize the base part of a derived object.
// \author Brad King
// \date February 2008

class rgtl_serialize_access;

//: Helper class to serialize the base part of a derived object.
//  Example usage:
//    template <typename Serializer>
//    void serialize(Serializer& sr)
//      {
//      sr & rgtl_serialize_base<my_base_class>(*this);
//      sr & member_;
//      }
template <typename Base>
class rgtl_serialize_base
{
public:
  //: Construct by storing a reference to the base part of the object
  rgtl_serialize_base(Base& b): base_(b) {}
  Base& base() const { return base_; }
private:
  Base& base_;
};

//: Provide "serializer & rgtl_serialize_base<derived>(*this)" syntax
template <class Serializer, typename Base>
Serializer& operator&(Serializer& sr, rgtl_serialize_base<Base> const& b)
{
  return sr & b.base();
}

#endif
