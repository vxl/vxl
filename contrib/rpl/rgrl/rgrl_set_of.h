#ifndef rgrl_set_of_h_
#define rgrl_set_of_h_

//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include <vcl_vector.h>

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
  unsigned
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

private:
  // disabled to prevent accidental pass-by-value
  rgrl_set_of( rgrl_set_of const& );

private:
  vcl_vector< T > data_;
};

#endif // rgrl_set_of_h_
