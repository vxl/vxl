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
//
//--------------------------------------------------------------
//
// Class : PMatrixAffine
//
// .SECTION Description:
//
// A class to hold an affine camera, a constrained form of the
// perspective projection matrix, and use it to
// perform common operations e.g. projecting point in 3d space to
// its image on the image plane
//
// .NAME PMatrixAffine - Affine 3x4 projection matrix
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/PMatrixAffine.h
// .FILE PMatrixAffine.C
//


#ifndef _PMatrixAffine_h
#define _PMatrixAffine_h

#ifdef __GNUC__
#pragma interface
#endif
  
#include <vnl/vnl_matrix.h>
#include <mvl/HomgPoint2D.h>    
#include <mvl/HomgPoint3D.h>    
#include <mvl/PMatrix.h>    

class PMatrixAffine : public PMatrix {

  // PUBLIC INTERFACE--------------------------------------------------------
  
public:
  
  // Constructors/Initializers/Destructors-----------------------------------
  
  PMatrixAffine(void);
  ~PMatrixAffine(void);
  
  // Data Access-------------------------------------------------------------
  
  // Data Control------------------------------------------------------------
  
  // Utility Methods---------------------------------------------------------
  
  // INTERNALS---------------------------------------------------------------

protected:
  
private:
  
  // Data Members------------------------------------------------------------
  
private:

};

#endif
// _PMatrixAffine_h

// Some emacs stuff, to insure c++-mode rather than c-mode:
// Local Variables:
// mode: c++
// End:
