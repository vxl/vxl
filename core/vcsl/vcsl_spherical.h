#ifndef vcsl_spherical_h_
#define vcsl_spherical_h_
//:
// \file
// \brief 3D spherical coordinate system
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2004/09/10 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vcsl/vcsl_spatial.h>
#include <vcsl/vcsl_spherical_sptr.h>

//: 3D coordinate system specified by distance rho, angles theta and phi.
class vcsl_spherical
  : public vcsl_spatial
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_spherical();

  // Destructor
  virtual ~vcsl_spherical() {}

  //***************************************************************************
  // Because VXL does not necessarily use dynamic_cast<>
  //***************************************************************************

  virtual const vcsl_spherical *cast_to_spherical() const { return this; }
};

#endif // vcsl_spherical_h_
