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
// Class : BaseTriTensorCompute
//
// .SECTION Description:
//
// A class to generate a Trifocal Tensor from point/line segment matched triplets,
// with separate functions for the linear/non-linear computations.
// Linear computation uses RANSAC and is robust to the presence of
// incorrect matches.
//
// .NAME BaseTriTensorCompute
// .LIBRARY MViewCompute
// .HEADER MultiView package
// .INCLUDE mvl/TriTensorCompute.h
// .FILE TriTensorCompute.C
//
// .SECTION Author:
//             Paul Beardsley, 29.03.96
//             Oxford University, UK
//
// .SECTION Modifications :
//   <none yet>
//
//---------------------------------------------------------------------------

#ifndef _BaseTriTensorCompute_h
#define _BaseTriTensorCompute_h

#ifdef __GNUC__
#pragma interface
#endif
  
#include <vcl/vcl_list.h>

class HomgMatchLineSeg2D2D2D;
class HomgMatchPoint2D2D2D;
class TriTensor;

class BaseTriTensorCompute {
  
  // PUBLIC INTERFACE------------------------------------------------------
  
public:

  // Constructors/Initializers/Destructors---------------------------------

  BaseTriTensorCompute();
  ~BaseTriTensorCompute();
  
  // Data Access-----------------------------------------------------------
  
  void add_matches (vcl_listP<HomgMatchLineSeg2D2D2D*> *match_list);
	
  void add_matches (vcl_listP<HomgMatchPoint2D2D2D*> *match_list);
  
  void clear_matches_line (void);
  void clear_matches_point (void);
  
  // Data Control----------------------------------------------------------
  
  // Utility Methods-------------------------------------------------------
  
  // INTERNALS-------------------------------------------------------------

protected:

private:

    // Data Members----------------------------------------------------------

protected:

  // the matched points.
  vcl_listP<HomgMatchPoint2D2D2D*> *_matchpoint_list_ptr;

  // the matched line segments.
  vcl_listP<HomgMatchLineSeg2D2D2D*> *_matchlineseg_list_ptr;
};


#endif
// _BaseTriTensorCompute_h

// Some emacs stuff, to insure c++-mode rather than c-mode:
// Local Variables:
// mode: c++
// End:
