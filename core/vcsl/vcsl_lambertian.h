#ifndef VCSL_LAMBERTIAN_H
#define VCSL_LAMBERTIAN_H

//:
// \file 
// \author François BERTEL
//
// \verbatim
// Modifications
// 2000/06/30 François BERTEL Creation. Adapted from IUE
// 10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
// \endverbatim

#include <vcsl/vcsl_lambertian_sptr.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vcsl/vcsl_geographic.h>
//: Lambert Conformal Conic Projection
// Coordinate system in which shapes are preserved while scale is nonuniform.
// Coordinates are specified in terms of x, y, and z, where z is the altitude
// above the surface of the reference ellipsoid (or sphere).
class vcsl_lambertian
  : public vcsl_geographic
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor.
  explicit vcsl_lambertian(void);

  //: Destructor
  virtual ~vcsl_lambertian();
};

#endif // #ifndef VCSL_LAMBERTIAN_H
