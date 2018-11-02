#ifndef rgrl_set_of_h_
#define rgrl_set_of_h_

//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include <vector>
#include <iostream>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A collection of objects
//
// This is a std::vector with a subset of the interface. The idea is
// to allow easy change of underlying data structures if it proves
// necessary later on.
//
template<typename T>
class rgrl_set_of
{
public:
  //: Construct an empty set
  rgrl_set_of();

  // compiler generated copy and assignment are good enough.

  //: Add a match set to the collection.
  void
  push_back( T const& item );

  //: The number of match sets in the collection.
  std::size_t
  size() const;

  //:
  T const& operator[]( unsigned i ) const;

  //:
  T& operator[]( unsigned i );

  //: Reset to an empty collection
  void
  clear();

  //: fill current set
  void fill( T const& item );

  //: Same idea as std::vector::resize()
  void
  resize( unsigned n );

  //: is it an empty set?
  bool empty() const;

private:
  std::vector< T > data_;
};

#endif // rgrl_set_of_h_
