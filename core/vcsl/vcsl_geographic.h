#ifndef VCSL_GEOGRAPHIC_H
#define VCSL_GEOGRAPHIC_H

//:
// \file 
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/29 François BERTEL Creation. Adapted from IUE
// 10/4/2001 Ian Scott (Manchester) Coverted perceps header to doxygen
// \endverbatim


#include <vcsl/vcsl_geographic_sptr.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vcsl/vcsl_spatial.h>
#include <vcsl/vcsl_spheroid_sptr.h>
//: Represent a location relative to the earth
// 3D coordinate system that is intended to represent a location relative to
// the earth, although its use is not restricted to earthbound coordinate
// systems. See the book "Map Projections Used by the U.S. Geological Survey"
// (Snyder, John P., "Map Projections Used by the U.S. Geological Survey,"
// Geological Survey Bulletin 1532, U.S. Government Printing Office,
// Washington, 1982.) for further detail on most of these coordinate systems.
class vcsl_geographic
  :public vcsl_spatial
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_geographic(void);

  //: Destructor
  virtual ~vcsl_geographic();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Return the spheroid
  virtual vcsl_spheroid_sptr spheroid(void) const;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set the spheroid
  virtual void set_spheroid(vcsl_spheroid &new_spheroid);

protected:
  //***************************************************************************
  // Implementation
  //***************************************************************************
  
  //: Spheroid
  vcsl_spheroid_sptr spheroid_;
};

#endif // #ifndef VCSL_GEOGRAPHIC_H
