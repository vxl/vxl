#ifndef vcsl_geographic_h_
#define vcsl_geographic_h_
//:
// \file
// \brief Represent a location relative to the earth
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/29 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2004/09/17 Peter Vanroose  made spheroid() non-virtual - it just returns a member and should not be overloaded
//   2004/09/17 Peter Vanroose  do not pass vcsl_spheroid objects; use vcsl_spheroid_sptr instead
// \endverbatim

#include <vcsl/vcsl_spatial.h>
#include <vcsl/vcsl_geographic_sptr.h>
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
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

 protected:
  // Default constructor
  vcsl_geographic();

 public:
  // Destructor
  virtual ~vcsl_geographic() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Return the spheroid
  vcsl_spheroid_sptr spheroid() const { return spheroid_; }

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set the spheroid
  void set_spheroid(vcsl_spheroid_sptr const& s) { spheroid_=s; }

 protected:
  //***************************************************************************
  // Implementation
  //***************************************************************************

  //: Spheroid
  vcsl_spheroid_sptr spheroid_;
};

#endif // vcsl_geographic_h_
