// This is oxl/mvl/HomgLine3D.cxx
//:
//  \file

#include <iostream>
#include "HomgLine3D.h"
#include <mvl/Homg3D.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//--------------------------------------------------------------
//
//: Constructor
HomgLine3D::HomgLine3D() = default;

//--------------------------------------------------------------
//
//: Constructor
HomgLine3D::HomgLine3D( const HomgLine3D &that) = default;

//--------------------------------------------------------------
//
//: Constructor, initialise using the specified distinct points on the line.
HomgLine3D::HomgLine3D (const HomgPoint3D& start,
                        const HomgPoint3D& end)
{
  // ho_quadvecstd_points2_to_line
  bool start_finite = start.w() != 0.0;
  bool end_finite = end.w() != 0.0;

  if (start_finite && end_finite) {
    point_finite_ = start;

    vnl_double_3 start_trivec = start.get_double3();
    vnl_double_3 end_trivec = end.get_double3();
    vnl_double_3 direction = (end_trivec - start_trivec);
    direction.normalize();

    point_infinite_.set(direction[0], direction[1], direction[2], 0.0);
  } else if (end_finite) {
    // Start infinite
    point_finite_ = end;

    const vnl_vector<double>& dir = start.get_vector().as_ref();
    point_infinite_ = HomgPoint3D(dir / dir.magnitude());
  } else {
    // End infinite -- just assign
    point_finite_ = start;
    const vnl_vector<double>& dir = end.get_vector().as_ref();
    point_infinite_ = HomgPoint3D(dir / dir.magnitude());
  }
}

//--------------------------------------------------------------
//
//: Destructor
HomgLine3D::~HomgLine3D() = default;

//-----------------------------------------------------------------------------
//
//: print to std::ostream
std::ostream& operator<<(std::ostream& s, const HomgLine3D& l)
{
  return s << "<HomgLine3D " << l.get_point_finite() << " dir " << l.get_point_infinite() << ">";
}

//: Push point2 off to infinity
void HomgLine3D::force_point2_infinite()
{
}

//: Return line direction as a 3-vector
vnl_double_3 HomgLine3D::dir() const
{
  const vnl_vector<double>& d = point_infinite_.get_vector().as_ref();
  if (d[3] != 0) {
    std::cerr << *this;
    std::cerr << "*** HomgLine3D: Infinite point not at infinity!! ***\n";
  }
  return {d[0], d[1], d[2]};
}
