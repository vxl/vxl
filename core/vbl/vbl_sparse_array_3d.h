#ifndef vbl_sparse_array_3d_h_
#define vbl_sparse_array_3d_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_sparse_array_3d.h

//:
// \file
// \brief Sparse 3d array.
// \author Andrew W. Fitzgibbon, Oxford RRG, 02 Oct 96
// Description
//    vbl_sparse_array_3d is a sparse 3D array allowing space 
//    efficient access of the form s(300,700,900) = 2;
// \verbatim
// Modifications:
// 26 March 01 cjb updated documentation
// 10 April 01 IMS (Manchester ISBE) modified to use vbl_sparse_array_base
// \endverbatim


#include <vbl/vbl_sparse_array_base.h>
#include <vcl_iosfwd.h>

//: An index for 3d arrays
// Used specifically with vbl_sparse_array_3d, but can be used
// for other things.
struct vbl_index_3d
{
  //: Construct 3d index.
  vbl_index_3d(unsigned index_i, unsigned index_j,
               unsigned index_k):
    i(index_i), j(index_j), k(index_k) {}

  unsigned i;
  unsigned j;
  unsigned k;
};

//: Defines an ordering on vbl_index_2d
inline bool operator< (const vbl_index_3d &a, const vbl_index_3d &other)
{
  if (a.i == other.i) return (a.j < other.j);
  else return (a.i < other.i);
}



//: Sparse 3d array allowing space efficient access 
// You can use this as s(300,700,900) = 2.
template <class T>
class vbl_sparse_array_3d : public vbl_sparse_array_base<T, vbl_index_3d>
{
public:

    //: Put a value into location (i,j).
  bool put(unsigned i, unsigned j, unsigned k, const T& t)
  {
    return vbl_sparse_array_base<T, vbl_index_3d>::put(vbl_index_3d(i, j, k), t);
  }

    //: Return contents of location (i,j,k).
    //  Returns an undefined value (in fact 
    //  a T()) if location (i,j,k) has not been filled with a value.
  T& operator () (unsigned i, unsigned j, unsigned k)
  {
    return vbl_sparse_array_base<T, vbl_index_3d>::operator() (vbl_index_3d(i, j, k));
  }

    //: Return contents of (i,j,k).  Assertion failure if not yet filled.
  const T& operator () (unsigned i, unsigned j, unsigned k) const
  {
    return vbl_sparse_array_base<T, vbl_index_3d>::operator() (vbl_index_3d(i, j, k));
  }

    //: Return true if location (i,j,k) has been filled.
  bool fullp(unsigned i, unsigned j, unsigned k) const
  {
    return vbl_sparse_array_base<T, vbl_index_3d>::fullp(vbl_index_3d(i, j, k));
  }

    //: Return the address of location (i,j,k).  0 if not yet filled.
  T* get_addr(unsigned i, unsigned j, unsigned k)
  {
    return vbl_sparse_array_base<T, vbl_index_3d>::get_addr(vbl_index_3d(i, j, k));
  }

  //: Print the Array to a stream in "(i,j,k): value" format.
  vcl_ostream& print(vcl_ostream&) const;

};

template <class T>
  //: Stream operator - print the Array to a stream in "(i,j,k): value" format.
inline vcl_ostream& operator << 
(vcl_ostream& s, const vbl_sparse_array_3d<T>& a)
{
  return a.print(s);
}


#define VBL_SPARSE_ARRAY_3D_INSTANTIATE_base(T) \
extern "please include vbl/vbl_sparse_array_3d.txx instead"
#define VBL_SPARSE_ARRAY_3D_INSTANTIATE(T) \
extern "please include vbl/vbl_sparse_array_3d.txx instead"

#endif // vbl_sparse_array_3d_h_
