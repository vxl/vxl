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
#ifndef _Homg3D_h
#define _Homg3D_h
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : Homg3D
//
// .SECTION Description:
//
// Base class for 3D homogeneous features.
//
// .NAME Homg3D - Base class for 3D homogeneous features.
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/Homg3D.h
// .FILE Homg3D.h
// .FILE Homg3D.C
//
// .SECTION Description:
//
// Base class for 3D homogeneous features.
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
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_vector.h>
 
class Homg3D : public Homg {

  // PUBLIC INTERFACE----------------------------------------------------------
	   
public:

  // Constructors/Initializers/Destructors-------------------------------------
  
  Homg3D () {}
  Homg3D (const Homg3D& that):_homg_vector(that._homg_vector) {}
  Homg3D (double px, double py, double pz, double pw = 1): _homg_vector(px,py,pz,pw) {}
  Homg3D (const vnl_vector<double>& v): _homg_vector(v) { }
 ~Homg3D () {}
  
  Homg3D& operator=(const Homg3D& that) { _homg_vector = that._homg_vector; return *this; } 
  // default ok. nope, egcs chokes
  
  // Data Access---------------------------------------------------------------

  vnl_double_4& asVector () { return _homg_vector; }
  const vnl_double_4& get_vector() const { return _homg_vector; }
  
  // get x,y,z,w.
  void get (double *x_ptr, double *y_ptr, double *z_ptr, double *w_ptr) const {
    *x_ptr = _homg_vector[0];
    *y_ptr = _homg_vector[1];
    *z_ptr = _homg_vector[2];
    *w_ptr = _homg_vector[3];
  }
  
// -- Get x.
  double get_x() const { return _homg_vector[0]; }
  double x() const { return _homg_vector[0]; }
  
// -- Get y.
  double get_y() const { return _homg_vector[1]; }
  double y() const { return _homg_vector[1]; }    

// -- Get z.
  double get_z() const { return _homg_vector[2]; }
  double z() const { return _homg_vector[2]; }    

// -- Get w.
  double get_w() const { return _homg_vector[3]; }
  double w() const { return _homg_vector[3]; }

// -- Set x,y,z,w.
  void set (double px, double py, double pz, double pw = 1) {
    _homg_vector[0] = px;
    _homg_vector[1] = py;
    _homg_vector[2] = pz;
    _homg_vector[3] = pw;
  }

  // Data Control--------------------------------------------------------------
  
  // Utility Methods-----------------------------------------------------------
  bool operator== (Homg3D const& p) const { return _homg_vector == p.get_vector(); }
  
  // INTERNALS-----------------------------------------------------------------

protected:

    // Data Members--------------------------------------------------------------

protected:
  vnl_double_4 _homg_vector;
};


#endif
// _Homg3D_h
// Some emacs stuff, to insure c++-mode rather than c-mode:
// Local Variables:
// mode: c++
// End:
