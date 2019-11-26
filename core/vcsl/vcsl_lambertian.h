#ifndef vcsl_lambertian_h_
#define vcsl_lambertian_h_
//:
// \file
// \brief Lambert Conformal Conic Projection
// \author Francois BERTEL
//
// \verbatim
//  Modifications
//   2000/06/30 Francois BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
// \endverbatim

#include "vcsl_geographic.h"
#include "vcsl_lambertian_sptr.h"

//: Lambert Conformal Conic Projection
// Coordinate system in which shapes are preserved while scale is nonuniform.
// Coordinates are specified in terms of x, y, and z, where z is the altitude
// above the surface of the reference ellipsoid (or sphere).
class vcsl_lambertian
  : public vcsl_geographic
{
 private:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor.
  vcsl_lambertian();

 public:
  // Destructor
  ~vcsl_lambertian() override = default;
};

#endif // vcsl_lambertian_h_
