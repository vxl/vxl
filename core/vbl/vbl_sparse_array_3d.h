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
// cjb updated documentation
// \endverbatim


#include <vcl_functional.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_iosfwd.h>

template <class T>
//: Sparse 3d array allowing space efficient access of the form 
// s(300,700,900) = 2.
class vbl_sparse_array_3d {
public:
  typedef vcl_map<unsigned, T, vcl_less<unsigned> > Map;

//: Construct a vbl_sparse_array_3d which can hold a maximum of (n1 x n2 x n3) elements.
// Currently (n1*n2*n3) must be representable in 32 bits, or about 1625 per dimension.
// Powers of two might conceivably give better performance -- they won't be worse.
  vbl_sparse_array_3d(unsigned max_dim_1 = 1024, unsigned max_dim_2 = 1024, unsigned max_dim_3 = 1024);

  //: Destructor
  ~vbl_sparse_array_3d();

  // Operations----------------------------------------------------------------
  //: Return contents at (i,j,k)
        T& operator () (unsigned i, unsigned j, unsigned k);
  //: Return contents at (i,j,k).  Assertion failure if not yet filled.
  const T& operator () (unsigned i, unsigned j, unsigned k) const;

  //: Return true if location (i,j,k) has been filled.
  bool fullp(unsigned i, unsigned j, unsigned k) const;
  //: Put a value into location (i,j,k).
  bool put(unsigned i, unsigned j, unsigned k, const T&);

  // Computations--------------------------------------------------------------
  //: Return number of locations that have been assigned a value using "put".
  unsigned count_nonempty() const { return storage_.size(); }

  // Data Control--------------------------------------------------------------
  //: Print the Array to a stream in "(i,j,k): value" format.
  vcl_ostream& print(vcl_ostream&) const;

protected:
  //: max entries in 1st dimension
  unsigned n1_;
  //: max entries in 2nd dimension
  unsigned n2_;
  //: max entries in 3rd dimension
  unsigned n3_;
  //: Map containing the data
  Map storage_;

public:
  // Helpers-------------------------------------------------------------------
  // Potentially clunky
  //: Copy constructor
  vbl_sparse_array_3d(vbl_sparse_array_3d<T> const &);
  //: Assignment
  vbl_sparse_array_3d<T> & operator=(vbl_sparse_array_3d<T> const &);
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
