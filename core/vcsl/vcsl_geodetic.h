#ifndef vcsl_geodetic_h_
#define vcsl_geodetic_h_
//:
// \file
// \brief Coordinate system to represent a location relative to the earth
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/29 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2004/09/10 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vcsl/vcsl_geographic.h>
#include <vcsl/vcsl_geodetic_sptr.h>

//: 3D coordinate system intended to represent a location relative to the earth.
// Although its use is not restricted to earthbound coordinate systems.
// The geodetic latitude is the angle made by a line to the center of the
// ellipsoid with the equatorial plane. The coordinates of a point are
// specified by the angles `lat' and `lon', and the distance from the surface
// of the ellipsoid, `z'.

class vcsl_geodetic
  : public vcsl_geographic
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor.
  vcsl_geodetic() {}

 public:
  // Destructor
  virtual ~vcsl_geodetic() {}
};

#endif // vcsl_geodetic_h_
