#ifndef VCSL_UTM_H
#define VCSL_UTM_H
//*****************************************************************************
//
// .NAME vcsl_utm - Universal Transverse Mercator projection
// .LIBRARY vcsl
// .HEADER  vxl Package
// .INCLUDE vcsl/vcsl_utm.h
// .FILE    vcsl/vcsl_utm.cxx
//
// .SECTION Description
// Coordinate system in which a point is specified by a grid zone designation,
// an x and y coordinate within the grid zone, and z, the height above the
// surface of the reference ellipsoid
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/29 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_utm_ref.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vcsl/vcsl_geographic.h>

class vcsl_utm
  : public vcsl_geographic
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor.
  explicit vcsl_utm(void);

  //: Destructor
  virtual ~vcsl_utm();
};

#endif // #ifndef VCSL_UTM_H
