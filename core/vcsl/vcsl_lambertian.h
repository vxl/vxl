#ifndef VCSL_LAMBERTIAN_H
#define VCSL_LAMBERTIAN_H
//*****************************************************************************
//
// .NAME vcsl_lambertian - Lambert Conformal Conic Projection
// .LIBRARY vcsl
// .HEADER  vcsl/vcsl_lambertian.h
// .INCLUDE vcsl/vcsl_lambertian_ref.h
// .INCLUDE vcsl/vcsl_geographic.h
// .FILE    vcsl/vcsl_lambertian.cxx
//
// .SECTION Description
// Coordinate system in which shapes are preserved while scale is nonuniform.
// Coordinates are specified in terms of x, y, and z, where z is the altitude
// above the surface of the reference ellipsoid (or sphere).
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/30 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_lambertian_ref.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vcsl/vcsl_geographic.h>

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
