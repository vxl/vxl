#ifndef _HomgLine3D_h
#define _HomgLine3D_h
#ifdef __GNUC__
#pragma interface
#endif

//--------------------------------------------------------------
//
// .NAME HomgLine3D - Homogeneous 3D Line
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgLine3D.h
// .FILE HomgLine3D.cxx
//
// .SECTION Description:
// A class to hold a homogeneous representation of a 3D Line.  The
// line is stored as a pair of HomgPoint3Ds.
//
// .SECTION Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
//

#include <mvl/HomgPoint3D.h>
#include <vcl_iosfwd.h>

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

  // Data Members------------------------------------------------------------
private:
  // any finite point on the line
  HomgPoint3D point_finite_;
  HomgPoint3D point_infinite_;
};

ostream& operator<<(ostream& s, const HomgLine3D& );

#endif // _HomgLine3D_h
