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
// Class : PMatrixEuclidean
//
// .SECTION Description:
//
// A class to hold a calibrated perspective projection matrix,
// also called the perspective camera, and use it to
// perform common operations e.g. projecting point in 3D space to
// its image on the image plane
//
// .NAME PMatrixEuclidean - 3x4 projection matrix
// .HEADER MultiView package
// .LIBRARY MViewBasics
// .INCLUDE mvl/PMatrixEuclidean.h
// .FILE PMatrixEuclidean.h
// .FILE PMatrixEuclidean.C
//

#ifndef _PMatrixEuclidean_h
#define _PMatrixEuclidean_h

#ifdef __GNUC__
#pragma interface
#endif
  
#include <vnl/vnl_matrix.h>
#include <mvl/HomgPoint2D.h>    
#include <mvl/HomgPoint3D.h>    
#include <mvl/PMatrix.h>    

class PMatrixEuclidean : public PMatrix {

  // PUBLIC INTERFACE----------------------------------------------------------
  
public:
  
  // Constructors/Initializers/Destructors-------------------------------------
  
  PMatrixEuclidean(void);
  ~PMatrixEuclidean(void);
  
  // Operations----------------------------------------------------------------

  // Computations--------------------------------------------------------------

  bool decompose_to_intrinsic_extrinsic_params (vnl_matrix<double> *camera_matrix,
						vnl_matrix<double> *R,
						vnl_vector<double> *t);

  // Data Access---------------------------------------------------------------
  
  // Utility Methods-----------------------------------------------------------
  
  // INTERNALS-----------------------------------------------------------------

protected:
  
private:
  
  // Data Members--------------------------------------------------------------
  
private:

};

#endif
