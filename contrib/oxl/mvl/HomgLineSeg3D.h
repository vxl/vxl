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
// .NAME HomgLineSeg3D - Homogeneous 3D line segment
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgLineSeg3D.h
// .FILE HomgLineSeg3D.cxx
//
// .SECTION Description:
//
// A class to hold a homogeneous representation of a 3D linesegment
//
// .SECTION Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
//

#ifndef _HomgLineSeg3D_h
#define _HomgLineSeg3D_h

#ifdef __GNUC__
#pragma interface
#endif

#include <mvl/HomgLine3D.h>    
#include <mvl/HomgOperator3D.h>
#include <mvl/HomgPoint3D.h>
#include <vcl/vcl_iosfwd.h>

class HomgLineSeg3D : public HomgLine3D {

  // PUBLIC INTERFACE----------------------------------------------------------
  
public:
  
  // Constructors/Initializers/Destructors-------------------------------------
  
  HomgLineSeg3D ();   
  HomgLineSeg3D (const HomgPoint3D &point1, const HomgPoint3D& point2);
  ~HomgLineSeg3D ();
  
  // Data Access---------------------------------------------------------------
  
  // get the line through the two points.
  const HomgLine3D& get_line () const;
  const HomgPoint3D& get_point1 () const;
  const HomgPoint3D& get_point2 () const;

  void set (const HomgPoint3D& point1, const HomgPoint3D& point2);
  
  // Data Control--------------------------------------------------------------
  // Derived operator== is now working properly so this is not needed:
  // bool operator==(HomgLineSeg3D const& p) const { return HomgLine3D::operator==(p); }
  
  // Utility Methods-----------------------------------------------------------
  
  // INTERNALS-----------------------------------------------------------------
  
protected:

private:

  // Data Members--------------------------------------------------------------
  
private:
  
  HomgPoint3D _point1;
  HomgPoint3D _point2;
    
};

ostream& operator<<(ostream& s, const HomgLineSeg3D& );

#endif
// _HomgLineSeg3D_h

// Some emacs stuff, to insure c++-mode rather than c-mode:
// Local Variables:
// mode: c++
// End:
