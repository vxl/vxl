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
//--------------------------------------------------------------
//
// Class : PMatrixCompute
//
// .SECTION Description:
//
// Base class of classes to generate a perspective projection matrix from
// a set of 3D-to-2D point correspondences.
// 
// .NAME PMatrixCompute
// .LIBRARY MViewCompute
// .HEADER MultiView package
// .INCLUDE mvl/PMatrixCompute.h
// .FILE PMatrixCompute.C
//

#ifndef _PMatrixCompute_h
#define _PMatrixCompute_h

#include <vcl/vcl_list.h>

class PMatrix;
class HomgMatchPoint3D2D;

class PMatrixCompute {

  // PUBLIC INTERFACE------------------------------------------------------
	   
public:

  // Constructors/Initializers/Destructors---------------------------------
  
  PMatrixCompute();
  virtual ~PMatrixCompute();
  
  // Data Access-----------------------------------------------------------
  
  
  // Data Control----------------------------------------------------------
  
  virtual bool compute (PMatrix *p_matrix_ptr);

  // Utility Methods-------------------------------------------------------

  // INTERNALS-------------------------------------------------------------
  
protected:
  
private:

  // Data Members----------------------------------------------------------

protected:

  // the matched points.
};


#endif
// _PMatrixCompute_h


// Some emacs stuff, to insure c++-mode rather than c-mode:
// Local Variables:
// mode: c++
// End:

