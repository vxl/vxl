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
#ifndef Homg1D_h_
#define Homg1D_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME        Homg1D - Base class for 1D homogeneous primitives
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/Homg1D.h
// .FILE        Homg1D.h
// .FILE        Homg1D.C
//
// .SECTION Description:
// Homg1D is the base class for one-dimensional homogeneous primitives.
//
// .SECTION Author:
//     Andrew W. Fitzgibbon, Oxford RRG, 15 Oct 96
//
// .SECTION Modifications:
//
//-----------------------------------------------------------------------------

#include <mvl/Homg.h>
#include <vnl/vnl_double_2.h>

class Homg1D : public vnl_double_2 , public Homg {
public:
  // Constructors/Destructors--------------------------------------------------
  
// -- Default constructor
  Homg1D() {}

// -- Construct point $(px, pw)$ where $pw = 1$ by default.
  Homg1D(double px, double pw = 1) : vnl_double_2(px, pw) {}

// -- Construct from first 2 components of a vector.
  Homg1D(const vnl_vector<double>& v): vnl_double_2(v) {}

// -- Copy constructor
  Homg1D(const Homg1D& that) { *this=that; }

// -- Destructor
 ~Homg1D() {}

// -- Assignment
  Homg1D& operator=(const Homg1D& that) { vnl_double_2::operator=(that); return *this; }
  void set (double px, double pw) { data[0] = px; data[1] = pw; }

  // Operations----------------------------------------------------------------

// -- Data access
  double get_x() const { return (*this)[0]; }
// -- Data access
  double get_w() const { return (*this)[1]; }

  const vnl_double_2& get_vector() const { return *this; }
  
  // Computations--------------------------------------------------------------

  // Data Access---------------------------------------------------------------

  // Data Control--------------------------------------------------------------

protected:

  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS Homg1D.

