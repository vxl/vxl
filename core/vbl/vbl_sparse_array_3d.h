// This is core/vbl/vbl_sparse_array_3d.h
#ifndef vbl_sparse_array_3d_h_
#define vbl_sparse_array_3d_h_
//:
// \file
// \brief a space efficient 3d array
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   02 Oct 96
//
//    vbl_sparse_array_3d is a sparse 3D array allowing space
//    efficient access of the form s(300,700,900) = 2.
//
// \verbatim
//  Modifications
//   26 March 2001 cjb updated documentation
//   10 April 2001 IMS (Manchester ISBE) modified to use vbl_sparse_array_base
//   11 April 2001 Peter Vanroose - vbl_index_3d moved to separate file
//   25 June  2001 IMS - vbl_index_3d replaced with vbl_triple
// \endverbatim
//---------------------------------------------------------------------------


#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_sparse_array_base.h>
#include <vbl/vbl_triple.h>


//: Sparse 3d array allowing space efficient access
// You can use this as e.g. s(300,700,900) = T(2).
template <class T>
class vbl_sparse_array_3d : public vbl_sparse_array_base<T, vbl_triple<unsigned, unsigned, unsigned> >
{
 public:

  //: Put a value into location (i,j,k).
  bool put(unsigned i, unsigned j, unsigned k, const T& t)
  {
    return vbl_sparse_array_base<T,
      vbl_triple<unsigned, unsigned, unsigned> >::
      put(vbl_make_triple(i, j, k), t);
  }

  //: Return contents of location (i,j,k).
  //  Returns an undefined value (in fact
  //  a T()) if location (i,j,k) has not been filled with a value.
  T& operator () (unsigned i, unsigned j, unsigned k)
  {
    return vbl_sparse_array_base<T,
      vbl_triple<unsigned, unsigned, unsigned> >::
      operator() (vbl_make_triple(i, j, k));
  }

  //: Return contents of (i,j,k).  Assertion failure if not yet filled.
  const T& operator () (unsigned i, unsigned j, unsigned k) const
  {
    return vbl_sparse_array_base<T,
      vbl_triple<unsigned, unsigned, unsigned> >::
      operator() (vbl_make_triple(i, j, k));
  }

  //: Return true if location (i,j,k) has been filled.
  bool fullp(unsigned i, unsigned j, unsigned k) const
  {
    return vbl_sparse_array_base<T,
      vbl_triple<unsigned, unsigned, unsigned> >::
      fullp(vbl_make_triple(i, j, k));
  }

  //: Return the address of location (i,j,k).  0 if not yet filled.
  T* get_addr(unsigned i, unsigned j, unsigned k)
  {
    return vbl_sparse_array_base<T,
      vbl_triple<unsigned, unsigned, unsigned> >::
      get_addr(vbl_make_triple(i, j, k));
  }

  //: Print the Array to a stream in "(i,j,k): value" format.
  std::ostream& print(std::ostream&) const;
};

//: Stream operator - print the Array to a stream in "(i,j,k): value" format.
template <class T>
inline std::ostream& operator <<
(std::ostream& s, const vbl_sparse_array_3d<T>& a)
{
  return a.print(s);
}


#ifndef VBL_SPARSE_ARRAY_BASE_INSTANTIATE
#define VBL_SPARSE_ARRAY_BASE_INSTANTIATE(T) \
extern "please include vbl/vbl_sparse_array_base.hxx instead"
#endif // VBL_SPARSE_ARRAY_BASE_INSTANTIATE
#define VBL_SPARSE_ARRAY_3D_INSTANTIATE(T) \
extern "please include vbl/vbl_sparse_array_3d.hxx instead"

#endif // vbl_sparse_array_3d_h_
