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
#ifndef _Homg2D_h
#define _Homg2D_h
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : Homg2D
//
// .SECTION Description:
//
// Base class for 2D homogeneous features.  This provides get/set.
//
// .NAME Homg2D - Base class for 2D homogeneous features.
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/Homg2D.h
// .FILE Homg2D.h
// .FILE Homg2D.C
//
// .SECTION Description:
//
// Base class for 2D homogeneous features.  This provides get/set.
//
// .SECTION Author:
//             Paul Beardsley, 29.03.96
//             Oxford University, UK
//
// .SECTION Modifications :
//    210297 AWF Switched to fixed-length vectors for speed.
//    110397 Peter Vanroose - added operator==
//
//-----------------------------------------------------------------------------

#include <mvl/Homg.h>
#include <vnl/vnl_double_3.h>

class Homg2D : public vnl_double_3, public Homg {

  // PUBLIC INTERFACE--------------------------------------------------------
  
public:
  
  // Constructors/Initializers/Destructors-----------------------------------

// -- Default constructor  
  Homg2D () {}

// -- Copy constructor  
  Homg2D (const Homg2D& that) { *this = that; }

// -- Construct a Homg2D from three doubles.
  Homg2D (double px, double py, double pw) { set(px,py,pw); }

// -- Construct from 3-vector.
  Homg2D (const vnl_vector<double>& v) { set(v); }

// -- Destructor
 ~Homg2D () {}

// -- Assignment  
  Homg2D& operator=(const Homg2D& that) {
    vnl_double_3::operator=(that);
    return *this;
  }
  
  // Data Access-------------------------------------------------------------

  const vnl_double_3& get_vector() const { return *this; }
  
// -- Retrieve components.
  void get (double *x_ptr, double *y_ptr, double *w_ptr) const {
    *x_ptr = data[0];
    *y_ptr = data[1];
    *w_ptr = data[2];
  }
  
// -- Return x
  double get_x () const { return data[0]; }
  
// -- Return y
  double get_y () const { return data[1]; }
  
// -- Return w
  double get_w () const { return data[2]; }
  
// -- Set x,y,w.
  void set (double px, double py, double pw) {
    data[0] = px;
    data[1] = py;
    data[2] = pw;
  }

// -- Set from vector
  void set (const vnl_vector<double>& v) { vnl_double_3::operator = (v); }

// -- Set element.
  void set (unsigned int element_index, double element) {
    data[element_index] = element;
  }

  // Data Control------------------------------------------------------------

// @{ ACCESS TO COMPONENTS: @}

// -- @{\vspace{-1.5\baselineskip} @}
  double& x() { return data[0]; }
// -- @{\vspace{-\baselineskip} @}
  double  x() const { return data[0]; }

// -- @{\vspace{-1.5\baselineskip} @}
  double& y() { return data[1]; }
// -- @{\vspace{-\baselineskip} @}
  double  y() const { return data[1]; }

// -- @{\vspace{-1.5\baselineskip} @}
  double& w() { return data[2]; }
// -- 
  double  w() const { return data[2]; }

  // INTERNALS---------------------------------------------------------------

protected:
};

#endif
