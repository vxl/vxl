#ifndef vbl_sparse_array_2d_h_
#define vbl_sparse_array_2d_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	vbl_sparse_array_2d - Sparse 2D array
// .LIBRARY	vbl
// .HEADER	vxl package
// .INCLUDE	vbl/vbl_sparse_array_2d.h
// .FILE	vbl_sparse_array_2d.txx
//
// .SECTION Description
//    vbl_sparse_array_2d is a sparse 2D array allowing space efficient access of
//    the form s(3000,7000) = 2;
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 02 Oct 96
//
// .SECTION Modifications:
//
//-----------------------------------------------------------------------------

#include <vbl/vbl_sparse_array.h>
#include <vbl/vbl_sparse_array_2d_base.h>
#include <vcl/vcl_iosfwd.h>

template <class T>
class vbl_sparse_array_2d : private vbl_sparse_array<T>, public vbl_sparse_array_2d_base {
public:
  // Constructors/Destructors--------------------------------------------------

// - Construct a vbl_sparse_array_2d which can hold a maximum of (n1 x n2) elements.
// Currently (n1*n2) must be representable in 32 bits, or about 64K per dimension.
// Powers of two might conceivably give better performance -- they won't be worse.
  // not implemented
  // vbl_sparse_array_2d(unsigned max_dim_1 = 0x10000, unsigned max_dim_2 = 0x10000);

  vbl_sparse_array_2d() {}
  vbl_sparse_array_2d(const vbl_sparse_array_2d<T>& that): vbl_sparse_array<T>(that) {}
  vbl_sparse_array_2d& operator=(const vbl_sparse_array_2d<T>& that) {
    vbl_sparse_array<T>::operator=(that);
    return *this;
  }

  // Operations----------------------------------------------------------------

// -- Put a value into location (i,j).
  bool put(unsigned i, unsigned j, const T& t) {
    return Base::put(encode(i,j), t);
  }

// -- Return contents of location (i,j).  Returns an undefined value (in fact a T())
// if location (i,j) has not been filled with a value.
  T& operator () (unsigned i, unsigned j) {
    return Base::operator[](encode(i,j));
  }

// -- Return contents of (i,j).  Assertion failure if not yet filled.
  const T& operator () (unsigned i, unsigned j) const {
    return Base::operator[](encode(i,j));
  }

// -- Return true if location (i,j) has been filled.
  bool fullp(unsigned i, unsigned j) const {
    return Base::fullp(encode(i,j));
  }

// -- Return the address of location (i,j).  0 if not yet filled.
  T* get_addr(unsigned i, unsigned j) {
    return Base::get_addr(encode(i,j));
  }

  // Computations--------------------------------------------------------------
// -- Return number of locations that have been assigned a value using "put".
  vbl_sparse_array<T>::count_nonempty;

  typedef vbl_sparse_array<T>::const_iterator const_iterator;
  vbl_sparse_array<T>::begin;
  vbl_sparse_array<T>::end;

  // Data Control--------------------------------------------------------------
  ostream& print(ostream&) const;
  //friend ostream& operator >> (ostream&, const vbl_sparse_array_2d<T>& );

protected:
  // Data Members--------------------------------------------------------------
  typedef vbl_sparse_array<T> Base;
};

template <class T>
inline ostream& operator << (ostream& s, const vbl_sparse_array_2d<T>& a) {
  return a.print(s);
}

#define VBL_SPARSE_ARRAY_2D_INSTANTIATE_base(T) \
extern "please include vbl/vbl_sparse_array_2d.txx instead"

#define VBL_SPARSE_ARRAY_2D_INSTANTIATE(T) \
extern "please include vbl/vbl_sparse_array_2d.txx instead"

#endif // vbl_sparse_array_2d_h_
