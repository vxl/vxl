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
//
// .NAME HomgPoint2D - Homogeneous 3-vector for a 2D point.
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgPoint2D.h
// .FILE HomgPoint2D.C
//
// .SECTION Description:
//
// A class to hold a homogeneous 3-vector for a 2D point.
//
// .SECTION Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
//

#ifndef _HomgPoint2D_h
#define _HomgPoint2D_h

#ifdef __GNUC__
#pragma interface
#endif

#include <mvl/Homg2D.h>
#include <vcl/vcl_iosfwd.h>
#include <vnl/vnl_double_2.h>

class IUPoint;

class HomgPoint2D : public Homg2D {

    // PUBLIC INTERFACE--------------------------------------------------------
	   
public:
  
  // Constructors/Initializers/Destructors-----------------------------------
  
  HomgPoint2D () {}
  HomgPoint2D (const HomgPoint2D& that): Homg2D(that) {}
  HomgPoint2D (double px, double py, double pw = 1.0): Homg2D(px,py,pw) {}
  HomgPoint2D (const vnl_vector<double>& vector_ptr): Homg2D(vector_ptr) {}
  ~HomgPoint2D () {}

  HomgPoint2D& operator=(const HomgPoint2D& that)
  {
    Homg2D::operator=(that);
    return *this;
  }

  // Operations------------------------------------------------------------
  bool check_infinity(double tol = infinitesimal_tol) const;
  bool get_nonhomogeneous(double& px, double& py) const;
  vnl_double_2 get_double2() const;
  inline vnl_double_2 get_nonhomogeneous() const { return get_double2(); }
  IUPoint* get_iupoint() const;

  HomgPoint2D get_unitized() const;
  
  // Data Access-------------------------------------------------------------
  
  // Utility Methods---------------------------------------------------------
  static HomgPoint2D read(istream&, bool is_homogeneous = false);
  
  // INTERNALS---------------------------------------------------------------
  
protected:

private:
  
  // Data Members------------------------------------------------------------
  
private:
  
};

istream& operator>>(istream& is, HomgPoint2D& p);
ostream& operator<<(ostream& s, const HomgPoint2D& );

#endif
