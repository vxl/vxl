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
//-*- c++ -*-------------------------------------------------------------
#ifndef _Homg_h
#define _Homg_h
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : Homg
//
// .SECTION Description:
//
// This is the private base class for homogeneous vectors.  It provides the
// get/set interface, and also a static variable Homg::infinity which is used
// throughout when returning infinite nonhomogeneous values.
//
// .NAME	Homg - Private base class for homogeneous vectors.
// .LIBRARY	MViewBasics
// .HEADER	MultiView Package
// .INCLUDE	mvl/Homg.h
// .FILE	Homg.cxx
//
// .SECTION Author:
//             Paul Beardsley, 29.03.96
//             Oxford University, UK
//
// .SECTION Modifications:
//    210297 AWF Switched to fixed-length vectors for speed.
//
//-------------------------------------------------------------------------------

#include <vcl/vcl_cassert.h>    
#if defined(VCL_GCC_295)
# include <vnl/vnl_vector_fixed.h>
#endif

class Homg {

  // PUBLIC INTERFACE----------------------------------------------------------
  
public:

// -- Standard placeholder for methods that wish to return infinity.
  static double infinity;


// -- The tolerance used in "near zero" tests in the Homg subclasses.
  static double infinitesimal_tol;

// -- Static method to set the default tolerance used for infinitesimal checks.
// The default is 1e-12.
  static void set_infinitesimal_tol(double tol);
};

#endif
