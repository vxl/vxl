#ifndef VCSL_UTM_H
#define VCSL_UTM_H

//:
// \file 
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/29 François BERTEL Creation. Adapted from IUE
// 10/4/2001 Ian Scott (Manchester) Coverted perceps header to doxygen
// \endverbatim


#include <vcsl/vcsl_utm_sptr.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vcsl/vcsl_geographic.h>
//: Universal Transverse Mercator projection
// Coordinate system in which a point is specified by a grid zone designation,
// an x and y coordinate within the grid zone, and z, the height above the
// surface of the reference ellipsoid
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
