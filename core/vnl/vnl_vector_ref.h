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
#ifndef vnl_vector_ref_h_
#define vnl_vector_ref_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_vector_ref
//
// .SECTION Description
//    See vnl_matrix_ref.
//
// .NAME        vnl_vector_ref - vnl_vector using user-supplied storage
// .LIBRARY     vnl
// .HEADER	Numerics package
// .INCLUDE     vnl/vnl_vector_ref.h
// .FILE        vnl/vnl_vector_ref.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 04 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

//#include <vcl/vcl_new.h>
#include <vnl/vnl_vector.h>

template <class T>
class vnl_vector_ref : public vnl_vector<T> {
public:
  typedef vnl_vector<T> Base;

  // Constructors/Destructors--------------------------------------------------
  // This assumes vnl_vector has been modified to do nothing in a char* ctor.
  vnl_vector_ref(int n, T *space) : vnl_vector<T>( /*(char*)0, (char*)("hack")*/ ) {
    //delete data; // Add this, and remove  ^^^^^^^ for stock COOL. Costs a new[0]/delete
    Base::data = space;
    Base::num_elmts = n;
  }

  ~vnl_vector_ref() {
    // Prevent base dtor from releasing memory we don't own
    Base::data = 0;
  }

private:
  // Private operator new because deleting a pointer to
  // one of these through a baseclass pointer will attempt
  // to free the referenced memory.
  // Therefore disallow newing of these -- if you're paying for
  // one malloc, you can afford two.
  // NOW COMMENTED OUT - PVR, may 97
  //void* operator new(size_t) { return 0; }

public:
  // Privatizing other new means we must offer placement new for STL
  //void* operator new(size_t, void* space) { return space; }
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_vector_ref.
