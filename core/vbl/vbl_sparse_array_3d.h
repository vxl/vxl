#ifndef vbl_sparse_array_3d_h_
#define vbl_sparse_array_3d_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .SECTION Description
//    vbl_sparse_array_3d is a sparse 3D array allowing space efficient access of
//    the form s(300,700,900) = 2;
//
// .NAME        vbl_sparse_array_3d - Sparse 3D array.
// .LIBRARY     vbl
// .HEADER	Basics Package
// .INCLUDE     vbl/vbl_sparse_array_3d.h
// .FILE        vbl/vbl_sparse_array_3d.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 02 Oct 96
//
// .SECTION Modifications:
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_map.h>
#include <vcl/vcl_compiler.h>
#include <vcl/vcl_iosfwd.h>

template <class T>
class vbl_sparse_array_3d {
public:
  // Constructors/Destructors--------------------------------------------------

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
  ostream& print(ostream&) const;

protected:
  // Data Members--------------------------------------------------------------
  typedef vcl_map<unsigned, T, vcl_less<unsigned>
#ifdef VCL_SUNPRO_CC
  , allocator<vcl_pair<unsigned,T> >
#endif
  > Map;

  unsigned n1_;
  unsigned n2_;
  unsigned n3_;
  Map storage_;
  
public:
  // Helpers-------------------------------------------------------------------

  // Potentially clunky
  vbl_sparse_array_3d(const vbl_sparse_array_3d<T>&);
  vbl_sparse_array_3d& operator=(const vbl_sparse_array_3d<T>&);
};

template <class T>
inline ostream& operator << (ostream& s, const vbl_sparse_array_3d<T>& a)
{
  return a.print(s);
}


#define VBL_SPARSE_ARRAY_3D_INSTANTIATE_base(T) \
extern "please include vbl/vbl_sparse_array_3d.txx instead"
#define VBL_SPARSE_ARRAY_3D_INSTANTIATE(T) \
extern "please include vbl/vbl_sparse_array_3d.txx instead"

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vbl_sparse_array_3d.

