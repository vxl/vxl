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
#ifndef vnl_file_vector_h_
#define vnl_file_vector_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        FileVector - Load vnl_vector<T> from file.
// .LIBRARY     vnl
// .HEADER	Numerics Package
// .INCLUDE     vnl/FileVector.h
// .FILE        vnl/FileVector.cxx
//
// .SECTION Description
//    FileVector is a class to load a vector from a file.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 23 Dec 96
//
// .SECTION Modifications:
//     fsm created by modifying class FileMatrix
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector.h>

template <class T>
class vnl_file_vector : public vnl_vector<T> {
public:
  vnl_file_vector(char const* filename);

  operator bool() const { return ok_; }

private:
  bool ok_;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS FileVector.

