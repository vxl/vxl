#ifndef VCSL_GEODETIC_H
#define VCSL_GEODETIC_H

//:
// \file 
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/29 François BERTEL Creation. Adapted from IUE
// 10/4/2001 Ian Scott (Manchester.) Coverted perceps header to doxygen
// \endverbatim

#include <vcsl/vcsl_geodetic_sptr.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vcsl/vcsl_geographic.h>
//: Intended to represent a location relative to the earth
// 3D coordinate system that is intended to represent a location relative to
// the earth, although its use is not restricted to earthbound coordinate
// systems. The geodetic latitude is the angle made by a line to the center
// of the ellipsoid with the equatorial plane. The coordinates of a point are
// specified by the angles `lat' and `lon', and the distance from the surface
// of the ellipsoid, `z'.

class vcsl_geodetic
  : public vcsl_geographic
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor.
  explicit vcsl_geodetic(void);

  //: Destructor
  virtual ~vcsl_geodetic();
};

#endif // #ifndef VCSL_GEODETIC_H
