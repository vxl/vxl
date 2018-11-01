// This is core/vbl/vbl_sparse_array_base.h
#ifndef vbl_sparse_array_base_h_
#define vbl_sparse_array_base_h_
//:
// \file
// \brief base class for sparse arrays.
// \author Ian Scott, Manchester ISBE
// \date   10 April 2001

#include <functional>
#include <map>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A fully featured sparse array which devolves indexing to its templated type
// If you just want an ordinary sparse array use vbl_sparse_array_1d,
// vbl_sparse_array_2d, or vbl_sparse_array_3d.
//
// Design Decision: Advanced Users only.
//
// The sparse array design has as much of the code as possible in this
// templated base class. This allows us to code harden this class
// while leaving the three derived classes in vbl, simple, easy to
// understand and use.
// I rejected to use templating over the number of dimensions because
// this can lead into recursive templating which is in theory very nice,
// but in practice very horrible. It also makes the interface rather
// unintuitive.
// If you are worried about the speed aspects of using a pair of integers
// instead of a single encoded integer, then you can create
// an encoder class as the index type, and use it directly, or hide the
// details by writing a specialising derivative of vbl_sparse_array_base.

template <class T, class Index>
class vbl_sparse_array_base
{
 protected:
  //: The type of the storage
  typedef std::map<Index, T, std::less<Index> > Map;
  //: This stores a compact list of the values.
  Map storage_;

 public:

  typedef std::size_t size_type;

  //: Return contents at (i)
  T      & operator () (Index i) { return storage_[i]; }

  //: Return contents at (i). Asserts that (i) is non-empty.
  T const& operator () (Index i) const;

  //: Erase element at location (i). Assertion failure if not yet filled.
  void erase(Index );

  //: Return true if location (i) has been filled.
  bool fullp(Index ) const;

  //: Put a value into location (i).
  bool put(Index , const T& );

  //: Return the address of location (i).  0 if not yet filled.
  T* get_addr(Index);

  //: Empty the sparse matrix.
  void clear();

  //: The type of iterators into the efficient storage
  typedef typename Map::const_iterator const_iterator;

  //: Return number of locations that have been assigned a value using "put".
  size_type count_nonempty() const { return storage_.size(); }

  //: The type of objects used to index the sparse array
  typedef Index Index_type;

  //: The type of values stored by the sparse array
  typedef T T_type;

  //: The type of values of the controlled sequence
  // The value_type is a std::pair<Index_type, typename T_type>
  typedef typename Map::value_type sequence_value_type;

  //: A bidirectional iterator pointing at the first non-empty element
  // If the array is empty it points just beyond the end.
  const_iterator begin() const { return storage_.begin(); }

  //: A bidirectional iterator pointing just beyond last non-empty element.
  const_iterator end() const { return storage_.end(); }
};

#endif // vbl_sparse_array_base_h_
