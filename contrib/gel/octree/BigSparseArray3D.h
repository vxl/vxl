// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*-------------------------------------------------------------------
#ifndef BigSparseArray3D_h_
#define BigSparseArray3D_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \brief Sparse 3D array
//    BigSparseArray3D is a sparse 3D array allowing space efficient access of
//    the form s(300,700,900) = 2;
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 02 Oct 96
//
// \verbatim
//    Modifications:
//     180497 AWF Moved to Basics
// \endverbatim
//-----------------------------------------------------------------------------
#ifndef VCL_WIN32
typedef long long longlong
#else // There are no 64-bit integers on MS-Windows
typedef long longlong
#endif

#include <vcl_functional.h>
#include <vcl_utility.h>
#include <vcl_map.h>

#include <vcl_iostream.h>

template <class T>
class BigSparseArray3D {
public:
  // Constructors/Destructors--------------------------------------------------

//: Construct a BigSparseArray3D which can hold a maximum of (n1 x n2 x n3) elements.
// Currently (n1*n2*n3) must be representable in 32 bits, or about 1625 per dimension.
// Powers of two might conceivably give better performance -- they won't be worse.
  BigSparseArray3D(unsigned max_dim_1 = 2097152, unsigned max_dim_2 = 2097152, unsigned max_dim_3 = 2097152);
 ~BigSparseArray3D();

  // Operations----------------------------------------------------------------
        T& operator () (unsigned, unsigned, unsigned);
  const T& operator () (unsigned, unsigned, unsigned) const;

  bool fullp(unsigned, unsigned, unsigned) const;
  bool put(unsigned, unsigned, unsigned, const T&);

  // Computations--------------------------------------------------------------
  unsigned count_nonempty() const { return _storage.size(); }

  // Data Control--------------------------------------------------------------
  vcl_ostream& print(vcl_ostream&) const;
  //friend vcl_ostream& operator << (vcl_ostream&, const BigSparseArray3D<T>& );
  //friend vcl_ostream& operator >> (vcl_ostream&, const BigSparseArray3D<T>& );

protected:
  // Data Members--------------------------------------------------------------
  unsigned _n1;
  unsigned _n2;
  unsigned _n3;
  vcl_map<unsigned longlong, T, vcl_less<unsigned longlong> > _storage;

  typedef vcl_map<unsigned longlong, T, vcl_less<unsigned longlong> > Map;

public:
  // Helpers-------------------------------------------------------------------

  // Potentially clunky
  BigSparseArray3D(const BigSparseArray3D<T>&);
  BigSparseArray3D& operator=(const BigSparseArray3D<T>&);
};

template <class T>
inline vcl_ostream& operator << (vcl_ostream& s, const BigSparseArray3D<T>& a)
{
  return a.print(s);
}

#include <vcl_compiler.h>

#define INSTANTIATE_BIGSPARSEARRAY3D_base(T) \
template class BigSparseArray3D<T >

#define INSTANTIATE_BIGSPARSEARRAY3D(T) \
INSTANTIATE_BIGSPARSEARRAY3D_base(T); \
VCL_INSTANTIATE_INLINE(vcl_ostream& operator << (vcl_ostream&, const BigSparseArray3D<T > &))

#endif // BigSparseArray3D_h_
