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
#ifndef vnl_matrix_ref_h_
#define vnl_matrix_ref_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_matrix_ref
//
// .SECTION Description
//    vnl_matrix_ref is a vnl_matrix for which the data space has been
//    supplied externally.  This is useful for two main tasks:
//    (a) Treating some row-based "C" matrix as a vnl_matrix in order to
//    perform vnl_matrix operations on it.
//    (b) Declaring a vnl_matrix that uses stack-based storage for the
//    matrix (See MatrixFixed).  Note however that the rows are still allocated
//    on the heap.  See MatrixFixed for a fully stack-based solution.
//
//    This is a dangerous class.  I believe that I've covered all the bases, but
//    it's really only intended for interfacing with the Fortran routines.
//
//    The big warning is that returning a vnl_matrix_ref pointer will free non-heap
//    memory if deleted through a vnl_matrix pointer.  This should be
//    very difficult though, as vnl_matrix_ref objects may not be constructed using
//    operator new, and are therefore unlikely to be the unwitting subject
//    of an operator delete.
//
// .NAME        vnl_matrix_ref - vnl_matrix reference to user-supplied storage.
// .LIBRARY     vnl
// .HEADER	Numerics package
// .INCLUDE     vnl/vnl_matrix_ref.h
// .FILE        vnl/vnl_matrix_ref.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 04 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <new.h>
#include <vnl/vnl_matrix.h>

template <class T>
class vnl_matrix_ref : public vnl_matrix<T> {
  typedef vnl_matrix<T> Base;

public:
  // Constructors/Destructors--------------------------------------------------
  vnl_matrix_ref(int m, int n, T *datablck) {
    Base::data = new T* [m];
    for(int i = 0; i < m; ++i)
      Base::data[i] = datablck + i * n;
    Base::num_rows = m;
    Base::num_cols = n;
  }
  ~vnl_matrix_ref() {
    Base::data[0] = 0; // Prevent base dtor from releasing our memory
  }

private:
  // Private operator new because deleting a pointer to
  // one of these through a baseclass pointer will attempt
  // to free this in-class memory.
  // Therefore disallow newing of these -- if you're paying for
  // one malloc, you can afford three.
#if !defined(VCL_GCC_295)
  void* operator new(size_t) { return 0; }
#endif

  // Disallow resizing
  bool resize (unsigned int, unsigned int) { return 0; }

  // Disallow this as it would create a non-const alias to the Matrix
  vnl_matrix_ref(const vnl_matrix<T>&) {}

  // You can't assign one of these from a matrix, cos' you don't have any space
  vnl_matrix_ref(const vnl_matrix_ref<T>&) {}
  vnl_matrix_ref<T>& operator=(const vnl_matrix<T>&) { return *this; }
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_matrix_ref.
