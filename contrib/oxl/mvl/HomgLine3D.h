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
// Class : HomgLine3D
//
// .SECTION Description:
//
// A class to hold a homogeneous representation of a 3D Line.  The
// line is stored as a pair of HomgPoint3Ds.
//
// .NAME HomgLine3D - Homogeneous 3D Line.
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgLine3D.h
// .FILE HomgLine3D.C
//
// .SECTION Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
//

#ifndef _HomgLine3D_h
#define _HomgLine3D_h

#ifdef __GNUC__
#pragma interface
#endif

#include <mvl/HomgPoint3D.h>
#include <vcl/vcl_iosfwd.h>

class HomgLine3D {

  // PUBLIC INTERFACE--------------------------------------------------------

public:

  // Constructors/Initializers/Destructors-----------------------------------

  HomgLine3D (); 
  HomgLine3D ( const HomgLine3D & );
  HomgLine3D (const HomgPoint3D& point_finite, const HomgPoint3D& point_infinite);
 ~HomgLine3D ();

  // Data Access-------------------------------------------------------------
  HomgPoint3D const & get_point_finite() const { return point_finite_; }
  HomgPoint3D       & get_point_finite() { return point_finite_; }
  HomgPoint3D const & get_point_infinite() const { return point_infinite_; } 
  HomgPoint3D       & get_point_infinite() { return point_infinite_; }
  vnl_double_3 dir() const;
  
  // Data Control------------------------------------------------------------

  // Utility Methods---------------------------------------------------------
  void force_point2_infinite();
  bool operator==(HomgLine3D const& p) const {
    return point_finite_==p.point_finite_ && point_infinite_==p.point_infinite_;
  }

  // INTERNALS---------------------------------------------------------------

protected:

private:

  // Data Members------------------------------------------------------------

private:
  // any finite point on the line
  HomgPoint3D point_finite_;
  HomgPoint3D point_infinite_;
};

ostream& operator<<(ostream& s, const HomgLine3D& );

#endif
// _HomgLine3D_h

// Some emacs stuff, to insure c++-mode rather than c-mode:
// Local Variables:
// mode: c++
// End:
