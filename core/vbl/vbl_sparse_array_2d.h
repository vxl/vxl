#ifndef vbl_sparse_array_2d_h_
#define vbl_sparse_array_2d_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_sparse_array_2d.h

//:
// \file
// \brief a space efficient 2d array
// \author Andrew W. Fitzgibbon, Oxford RRG, 02 Oct 96
// Description
//    vbl_sparse_array_2d is a sparse 2D array allowing space 
//    efficient access of the form s(3000,7000) = 2.
// \verbatim
// Modifications:
// 26 March 01 cjb updated documentation
// 10 April 01 IMS (Manchester ISBE) modified to use vbl_sparse_array_base
// 11 April 01 Peter Vanroose - vbl_index_2d moved to separate file
// \endverbatim
//---------------------------------------------------------------------------

#include <vcl_iosfwd.h>
#include <vbl/vbl_sparse_array_base.h>
#include <vbl/vbl_index_2d.h>

//: Sparse 2D array allowing space efficient access of the form  s(300,700) =2
template <class T>
class vbl_sparse_array_2d : public vbl_sparse_array_base<T, vbl_index_2d>
{
public:

  //: Put a value into location (i,j).
  bool put(unsigned i, unsigned j, const T& t)
  {
    return vbl_sparse_array_base<T, vbl_index_2d>::put(vbl_index_2d(i, j), t);
  }

  //: Return contents of location (i,j).
  //  Returns an undefined value (in fact 
  //  a T()) if location (i,j) has not been filled with a value.
  T& operator () (unsigned i, unsigned j)
  {
    return vbl_sparse_array_base<T, vbl_index_2d>::operator() (vbl_index_2d(i, j));
  }

  //: Return contents of (i,j).  Assertion failure if not yet filled.
  const T& operator () (unsigned i, unsigned j) const
  {
    return vbl_sparse_array_base<T, vbl_index_2d>::operator() (vbl_index_2d(i, j));
  }

  //: Return true if location (i,j) has been filled.
  bool fullp(unsigned i, unsigned j) const
  {
    return vbl_sparse_array_base<T, vbl_index_2d>::fullp(vbl_index_2d(i, j));
  }

  //: Return the address of location (i,j).  0 if not yet filled.
  T* get_addr(unsigned i, unsigned j)
  {
    return vbl_sparse_array_base<T, vbl_index_2d>::get_addr(vbl_index_2d(i, j));
  }

  //: Print the Array to a stream in "(i,j): value" format.
  vcl_ostream& print(vcl_ostream&) const;
};

//: Stream operator - print the Array to a stream in "(i,j): value" format.
template <class T>
inline vcl_ostream& operator << 
(vcl_ostream& s, const vbl_sparse_array_2d<T>& a) {
  return a.print(s);
}


#define VBL_SPARSE_ARRAY_2D_INSTANTIATE(T) \
extern "please include vbl/vbl_sparse_array_2d.txx instead"

#endif // vbl_sparse_array_2d_h_
