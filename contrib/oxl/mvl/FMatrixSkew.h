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
#ifndef _FMatrixSkew_h
#define _FMatrixSkew_h
#ifdef __GNUC__
#pragma interface
#endif
//--------------------------------------------------------------
//
// Class : FMatrixSkew
//
// .SECTION Description:
//
// A class to hold a Fundamental Matrix of the skew form
// which occurs with a pure translational motion.
// Some common operations e.g. generate epipolar lines,
// are inherited from the class FMatrix.
//
// .NAME FMatrixSkew - Skew fundamental matrix.
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE FMatrixSkew.h 
// .FILE FMatrixSkew.C
//
  
#include <vnl/vnl_matrix.h>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/FMatrix.h>

class PMatrix;

class FMatrixSkew : public FMatrix {
  
  // PUBLIC INTERFACE-------------------------------------------------------
  
public:
  
  // Constructors/Initializers/Destructors----------------------------------
  
  FMatrixSkew();
  FMatrixSkew(const double* f_matrix);
  FMatrixSkew(const vnl_matrix<double>& f_matrix);
  ~FMatrixSkew();

  // Computations

  inline void set_rank2_using_svd();  
  inline FMatrixSkew get_rank2_truncated();
  bool get_epipoles (HomgPoint2D* e1_out, HomgPoint2D* e2_out) const;
  void decompose_to_skew_rank3 (vnl_matrix<double> *skew,
				vnl_matrix<double> *rank3) const;
  void find_nearest_perfect_match (const HomgPoint2D& in1, const HomgPoint2D& in2,
				   HomgPoint2D *out1, HomgPoint2D *out2) const;


  // Data Access------------------------------------------------------------
  
  bool set (const double* f_matrix );
  inline bool set (const vnl_matrix<double>& f_matrix );
  inline bool get_rank2_flag (void) const;
  inline void set_rank2_flag (bool rank2_flag); 
    
  // Data Control-----------------------------------------------------------
  
  // Utility Methods---------------------------------------------------------
  
  // INTERNALS---------------------------------------------------------------
  
protected:
  
private:
  
  // Data Members------------------------------------------------------------

private:
  
};

#endif // _FMatrixSkew_h
