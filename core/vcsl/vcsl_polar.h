#ifndef vcsl_polar_h_
#define vcsl_polar_h_
//:
// \file
// \brief Planar polar coordinate system
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2004/09/10 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vcsl/vcsl_spatial.h>
#include <vcsl/vcsl_polar_sptr.h>

//: Planar coordinate system specified by the parameters rho and theta
class vcsl_polar
  : public vcsl_spatial
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor.
  vcsl_polar();

  // Destructor
  virtual ~vcsl_polar() {}

  //***************************************************************************
  // Because VXL does not necessarily use dynamic_cast<>
  //***************************************************************************

  virtual const vcsl_polar *cast_to_polar() const { return this; }
};

#endif // vcsl_polar_h_
