// This is vxl/vbl/vbl_big_sparse_array_3d.h
#ifndef vbl_big_sparse_array_3d_h_
#define vbl_big_sparse_array_3d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Sparse 3D array
//
//  vbl_big_sparse_array_3d is a sparse 3D array allowing space efficient
//  access of the form    s(300,700,900) = 2;
//  It uses the 64-bit integer type "long long" (whenever supported by the
//  compiler) to store the 3D index: 21 bits per dimension.
//  Hence the largest possible coordinate in each dimension is 2^21-1 = 2097151.
//  On platforms that do not have 64-bit integers, the maximum is 2^10-1 = 1023.
//  (Actually, for some dimensions, it could be a factor 2 higher.)
//
// Example usage:
// \code
//  vbl_big_sparse_array_3d<double> x;
//
//  x(1,2,3) = 1.23;
//  x(100,200,3) = 100.2003;
//  x.put(200,300,4, 200.3004);
//
//  vcl_cout << "123 = " << x(1,2,3) << vcl_endl
//           << "222 = " << x(2,2,2) << vcl_endl
//           << "333 is full? " << x.fullp(3,3,3) << vcl_endl
//           << x;
// \endcode
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 02 Oct 96
//
// \verbatim
// Modifications:
//  180497 AWF - Moved to Basics
//  261001 Peter Vanroose - documentation added about implementation
//  261001 Peter Vanroose - bug fixed in bigencode - had 11,22 instead of 21,42.
//  271001 Peter Vanroose - ported to vxl from BigSparseArray3; removed n1,n2,n3
// \endverbatim
//-----------------------------------------------------------------------------

#include <vxl_config.h>

#if VXL_HAS_INT_64
# define VCL_HAS_LONG_LONG 1
typedef vxl_uint_64 ulonglong;
#elif VXL_HAS_INT_32
# define VCL_HAS_LONG_LONG 0
typedef vxl_uint_32 ulonglong;
#else
# error "only implemented with 32 and 64-bit ints"
#endif

#include <vcl_functional.h>
#include <vcl_map.h>
#include <vcl_iostream.h>

template <class T>
class vbl_big_sparse_array_3d
{
 public:
  // Constructors/Destructor---------------------------------------------------

  //: Construct a vbl_big_sparse_array_3d
  vbl_big_sparse_array_3d();
 ~vbl_big_sparse_array_3d();

  // Potentially clunky copy constructor
  vbl_big_sparse_array_3d(vbl_big_sparse_array_3d<T> const&);
  // Potentially clunky assignment operator
  vbl_big_sparse_array_3d& operator=(vbl_big_sparse_array_3d<T> const&);

  // Operations----------------------------------------------------------------
  T      & operator() (unsigned, unsigned, unsigned);
  T const& operator() (unsigned, unsigned, unsigned) const;

  //: Has this cell been assigned a value?
  bool fullp(unsigned, unsigned, unsigned) const;
  //: Put a value in a certain cell
  bool put(unsigned, unsigned, unsigned, T const&);

  // Computations--------------------------------------------------------------
  unsigned count_nonempty() const { return storage_.size(); }

  // Data Control--------------------------------------------------------------
  vcl_ostream& print(vcl_ostream&) const;

 protected:
  // Data Members--------------------------------------------------------------
  typedef vcl_map<ulonglong, T, vcl_less<ulonglong> > Map;
  Map storage_;
};

template <class T>
inline vcl_ostream& operator<< (vcl_ostream& s, vbl_big_sparse_array_3d<T> const& a)
{
  return a.print(s);
}

#include <vcl_compiler.h>

#define VBL_BIG_SPARSE_ARRAY_3D_INSTANTIATE(T) \
extern "Please #include <vbl/vbl_big_sparse_array_3d.txx> instead"

#endif // vbl_big_sparse_array_3d_h_
