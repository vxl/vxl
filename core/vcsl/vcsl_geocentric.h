#ifndef VCSL_GEOCENTRIC_H
#define VCSL_GEOCENTRIC_H
//*****************************************************************************
//
// .NAME vcsl_geocentric - Intended to represent a location relative to the
//                         earth
// .LIBRARY vcsl
// .HEADER  vxl Package
// .INCLUDE vcsl/vcsl_geocentric.h
// .FILE    vcsl/vcsl_geocentric.cxx
//
// .SECTION Description
// 3D coordinate system that is intended to represent a location relative to
// the earth, although its use is not restricted to earthbound coordinate
// systems. The geocentric latitude is the angle made by a line to the center
// of the ellipsoid with the equatorial plane. The coordinates of a point are
// specified by the angles `lat' and `lon', and the distance from the surface
// of the ellipsoid, `z'.
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/29 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_geocentric_ref.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vcsl/vcsl_geographic.h>

class vcsl_geocentric
  : public vcsl_geographic
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor.
  explicit vcsl_geocentric(void);

  //: Destructor
  virtual ~vcsl_geocentric();
};

#endif // #ifndef VCSL_GEOCENTRIC_H
