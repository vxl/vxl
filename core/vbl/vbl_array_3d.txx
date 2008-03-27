// This is core/vbl/vbl_array_3d.txx
#ifndef vbl_array_3d_txx_
#define vbl_array_3d_txx_

//:
// \file

#include "vbl_array_3d.h"
#include <vcl_cassert.h>

//--------------------------------------------------------------
//
// Constructor and Destructor Functions
//
//--------------------------------------------------------------

//: Constructor utility.
// This allocates a 3D array which can be referenced using the form myarray[a][b][c].
// Useful in C although maybe superfluous here as access is via a get function anyway.
template <class T>
void vbl_array_3d<T>::construct(size_type n1, size_type n2, size_type n3)
{
  assert(n1 >= 0);
  assert(n2 >= 0);
  assert(n3 >= 0);

  row1_count_ = n1;
  row2_count_ = n2;
  row3_count_ = n3;

  // If any of the dimensions are 0, don't allocate memory, just return.
  if ((n1 * n2 * n3)==0) {
    element_ = 0;
    return;
  }

  // allocate the memory for the first level pointers.
  element_ = new T** [n1];

  // set the first level pointers and allocate the memory for the second level pointers.
  {
    element_[0] = new T* [n1 * n2];
    for (size_type row1_index = 0; row1_index < n1; row1_index++)
      element_ [row1_index] = element_[0] + n2 * row1_index;
  }

  T* array_ptr = new T [n1*n2*n3];

  // set the second level pointers.
  for (size_type row1_index = 0; row1_index < n1; row1_index++)
    for (size_type row2_index = 0; row2_index < n2; row2_index++) {
      element_ [row1_index][row2_index] = array_ptr;
      array_ptr += n3;
    }
}

template <class T>
void vbl_array_3d<T>::destruct()
{
  if (element_) {
    // remove the actual members.
    delete [] element_ [0][0];

    // remove the second level pointers.
    delete [] element_ [0];

    // remove the first level pointers.
    delete [] element_;
  }
}

template <class T>
void vbl_array_3d<T>::resize(size_type n1, size_type n2, size_type n3)
{
  if (n1 == row1_count_ && n2 == row2_count_ && n3 == row3_count_)
    return;
  destruct();
  construct(n1, n2, n3);
}

//: Fill from static array of Ts.
//  The final index fills fastest, so if we consider the tensor as a set of
// matrices (M[i])[j][k] then the matrices are filled in the usual C order.
template <class T>
void vbl_array_3d<T>::set(T const* p)
{
  for (size_type row1_index = 0; row1_index < row1_count_; row1_index++)
    for (size_type row2_index = 0; row2_index < row2_count_; row2_index++)
      for (size_type row3_index = 0; row3_index < row3_count_; row3_index++)
        element_ [row1_index][row2_index][row3_index] = *p++;
}

//: Get into array
template <class T>
void vbl_array_3d<T>::get(T* p) const
{
  for (size_type row1_index = 0; row1_index < row1_count_; row1_index++)
    for (size_type row2_index = 0; row2_index < row2_count_; row2_index++)
      for (size_type row3_index = 0; row3_index < row3_count_; row3_index++)
        *p++ = element_ [row1_index][row2_index][row3_index];
}

//: Fill with constant
template <class T>
void vbl_array_3d<T>::fill(T const& value)
{
  size_type n = row1_count_ * row2_count_ * row3_count_;
  T* d = data_block();
  T* e = d + n;
  while (d < e)
    *d++ = value;
}

//--------------------------------------------------------------------------------

#include <vcl_iostream.h>

template <class T>
vcl_ostream & operator<<(vcl_ostream& os, vbl_array_3d<T> const& A)
{
  typedef vbl_array_3d<T>::size_type size_type;
  for (size_type i=0; i<A.get_row1_count(); ++i) {
    for (size_type j=0; j<A.get_row2_count(); ++j) {
      for (size_type k=0; k<A.get_row3_count(); ++k) {
        if (k)
          os << ' ';
        os << A(i,j,k);
      }
      os << vcl_endl;
    }
    os << vcl_endl;
  }
  return os;
}

template <class T>
vcl_istream & operator>>(vcl_istream& is, vbl_array_3d<T>& A)
{
  typedef vbl_array_3d<T>::size_type size_type;
  for (size_type i=0; i<A.get_row1_count(); ++i)
    for (size_type j=0; j<A.get_row2_count(); ++j)
      for (size_type k=0; k<A.get_row3_count(); ++k)
        is >> A(i,j,k);
  return is;
}

//--------------------------------------------------------------------------------

#undef VBL_ARRAY_3D_INSTANTIATE
#define VBL_ARRAY_3D_INSTANTIATE(T) template class vbl_array_3d<T >

#undef VBL_ARRAY_3D_IO_INSTANTIATE
#define VBL_ARRAY_3D_IO_INSTANTIATE(T) \
template vcl_ostream & operator<<(vcl_ostream &,vbl_array_3d<T > const &); \
template vcl_istream & operator>>(vcl_istream &,vbl_array_3d<T > &)

#endif // vbl_array_3d_txx_
