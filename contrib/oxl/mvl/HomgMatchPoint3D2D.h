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
// Class : HomgMatchPoint3D2D
//
// .SECTION Description:
//
// A class to hold a match between a 3D and 2D point.
//
// .NAME HomgMatchPoint3D2D - A match between a 3D and 2D point.
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgMatchPoint3D2D.h
// .FILE HomgMatchPoint3D2D.C
//

#ifndef _HomgMatchPoint3D2D_h
#define _HomgMatchPoint3D2D_h

#ifdef __GNUC__
#pragma interface
#endif

#include <mvl/HomgPoint2D.h>
#include <mvl/HomgPoint3D.h>

class HomgMatchPoint3D2D {
  
  // PUBLIC INTERFACE--------------------------------------------------------
  
public:

  // Constructors/Initializers/Destructors-----------------------------------
  
  HomgMatchPoint3D2D ();
  HomgMatchPoint3D2D (HomgPoint3D *point3D_ptr, HomgPoint2D *point2D_ptr);   
  ~HomgMatchPoint3D2D ();
  
  // Data Access-------------------------------------------------------------
  
  HomgPoint3D get_point3D (void);
  
  HomgPoint2D get_point2D (void);
  
  void set (HomgPoint3D *point3D_ptr, HomgPoint2D *point2D_ptr);
  
  // Data Control------------------------------------------------------------
  
  // Utility Methods---------------------------------------------------------
  
  // INTERNALS---------------------------------------------------------------
  
protected:
  
private:
  
  // Data Members------------------------------------------------------------

private:

    HomgPoint2D _point2D;
    HomgPoint3D _point3D;
};



#endif
// _HomgMatchPoint3D2D_h

// Some emacs stuff, to insure c++-mode rather than c-mode:
// Local Variables:
// mode: c++
// End:
