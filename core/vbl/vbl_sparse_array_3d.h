#ifndef vbl_sparse_array_3d_h_
#define vbl_sparse_array_3d_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	vbl_sparse_array_3d - Sparse 3D array
// .LIBRARY	vbl
// .HEADER	vxl package
// .INCLUDE	vbl/vbl_sparse_array_3d.h
// .FILE	vbl_sparse_array_3d.txx
//
// .SECTION Description
//    vbl_sparse_array_3d is a sparse 3D array allowing space efficient access of
//    the form s(300,700,900) = 2;
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 02 Oct 96
//
// .SECTION Modifications:
//
//-----------------------------------------------------------------------------

#include <vcl_functional.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_iosfwd.h>

template <class T>
class vbl_sparse_array_3d {
public:
  typedef vcl_map<unsigned, T, vcl_less<unsigned> > Map;

// -- Construct a vbl_sparse_array_3d which can hold a maximum of (n1 x n2 x n3) elements.
// Currently (n1*n2*n3) must be representable in 32 bits, or about 1625 per dimension.
// Powers of two might conceivably give better performance -- they won't be worse.
  vbl_sparse_array_3d(unsigned max_dim_1 = 1024, unsigned max_dim_2 = 1024, unsigned max_dim_3 = 1024);
 ~vbl_sparse_array_3d();

  // Operations----------------------------------------------------------------
        T& operator () (unsigned, unsigned, unsigned);
  const T& operator () (unsigned, unsigned, unsigned) const;

  bool fullp(unsigned, unsigned, unsigned) const;
  bool put(unsigned, unsigned, unsigned, const T&);

  // Computations--------------------------------------------------------------
  unsigned count_nonempty() const { return storage_.size(); }

  // Data Control--------------------------------------------------------------
  vcl_ostream& print(vcl_ostream&) const;

protected:
  unsigned n1_;
  unsigned n2_;
  unsigned n3_;
  Map storage_;

public:
  // Helpers-------------------------------------------------------------------
  // Potentially clunky
  vbl_sparse_array_3d(vbl_sparse_array_3d<T> const &);
  vbl_sparse_array_3d<T> & operator=(vbl_sparse_array_3d<T> const &);
};

template <class T>
inline vcl_ostream& operator << (vcl_ostream& s, const vbl_sparse_array_3d<T>& a)
{
  return a.print(s);
}


#define VBL_SPARSE_ARRAY_3D_INSTANTIATE_base(T) \
extern "please include vbl/vbl_sparse_array_3d.txx instead"
#define VBL_SPARSE_ARRAY_3D_INSTANTIATE(T) \
extern "please include vbl/vbl_sparse_array_3d.txx instead"

#endif // vbl_sparse_array_3d_h_
