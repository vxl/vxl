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
//
// Class: vbl_sparse_array_2d
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 02 Oct 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include "vbl_sparse_array_2d.h"

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>

// -- Print the Array to a stream in "(i,j): value" format.
template <class T>
ostream& vbl_sparse_array_2d<T>::print(ostream& out) const
{
  const_iterator enD = storage_.end();
  for(const_iterator p = storage_.begin(); p != enD; ++p) {
    unsigned i,j;
    vbl_sparse_array_2d_base::decode((*p).first, i, j);
    out << "(" << i << "," << j << "): " << (*p).second << endl;
  }
  return out;
}

#undef VBL_SPARSE_ARRAY_2D_INSTANTIATE_base
#define VBL_SPARSE_ARRAY_2D_INSTANTIATE_base(T)\
template class vbl_sparse_array_2d<T>

#undef VBL_SPARSE_ARRAY_2D_INSTANTIATE 
#define VBL_SPARSE_ARRAY_2D_INSTANTIATE(T) \
VBL_SPARSE_ARRAY_2D_INSTANTIATE_base(T); \
VCL_INSTANTIATE_INLINE(ostream& operator<< (ostream&, const vbl_sparse_array_2d<T> &))
 

#ifdef MAIN
#undef MAIN
#include "vbl_sparse_array.txx"

int main()
{
  vbl_sparse_array_2d<double> x;

  x(1,2) = 1.23;
  x(100,200) = 100.2003;

  cout << "2,3 was " << x.put(2,3, 7) << endl;
  cout << "2,3 is " << *x.get_addr(2,3) << endl;

  cout << "12 = " << x(1,2) << endl;
  cout << "22 = " << x(2,2) << endl;

  cout << "100, 200 = " << x(100,200) << endl;

  cout << "333 is full? " << x.fullp(3,3) << endl;
  
  cout << x;
}
#endif
