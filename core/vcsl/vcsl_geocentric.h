#ifndef vcsl_geocentric_h_
#define vcsl_geocentric_h_
//:
// \file
// \brief A specific geographic coordinate system
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/29 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
// \endverbatim

#include <vcsl/vcsl_geographic.h>
#include <vcsl/vcsl_geocentric_sptr.h>

//: A specific geographic coordinate system
// Intended to represent a location relative to the earth
// 3D coordinate system that is intended to represent a location relative to
// the earth, although its use is not restricted to earthbound coordinate
// systems. The geocentric latitude is the angle made by a line to the center
// of the ellipsoid with the equatorial plane. The coordinates of a point are
// specified by the angles `lat' and `lon', and the distance from the surface
// of the ellipsoid, `z'.
class vcsl_geocentric
  : public vcsl_geographic
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor.
  vcsl_geocentric();

 public:
  // Destructor
  virtual ~vcsl_geocentric() {}
};

#endif // vcsl_geocentric_h_
