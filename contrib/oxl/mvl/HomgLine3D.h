// This is oxl/mvl/HomgLine3D.h
#ifndef HomgLine3D_h_
#define HomgLine3D_h_
//:
// \file
// \brief Homogeneous 3D Line
//
// A class to hold a homogeneous representation of a 3D Line.  The
// line is stored as a pair of HomgPoint3Ds.
//
// \verbatim
// Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <mvl/HomgPoint3D.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class HomgLine3D
{
  // Data Members------------------------------------------------------------
  HomgPoint3D point_finite_; //!< any finite point on the line
  HomgPoint3D point_infinite_;

 public:
  // Constructors/Initializers/Destructors-----------------------------------

  HomgLine3D();
  HomgLine3D(const HomgLine3D&);
  HomgLine3D(const HomgPoint3D& point_finite,const HomgPoint3D& point_infinite);
 ~HomgLine3D();

  // Data Access-------------------------------------------------------------
  HomgPoint3D const & get_point_finite() const { return point_finite_; }
  HomgPoint3D       & get_point_finite() { return point_finite_; }
  HomgPoint3D const & get_point_infinite() const { return point_infinite_; }
  HomgPoint3D       & get_point_infinite() { return point_infinite_; }
  vnl_double_3 dir() const;

  // Utility Methods---------------------------------------------------------
  void force_point2_infinite();
  bool operator==(HomgLine3D const& p) const {
    return point_finite_==p.point_finite_ && point_infinite_==p.point_infinite_;
  }
};

std::ostream& operator<<(std::ostream& s, const HomgLine3D& );

#endif // HomgLine3D_h_
