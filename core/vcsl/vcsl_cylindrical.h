#ifndef VCSL_CYLINDRICAL_H
#define VCSL_CYLINDRICAL_H
//:
// \file
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2004/09/10 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vcsl/vcsl_cylindrical_sptr.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vcsl/vcsl_spatial.h>
//: 3D coordinate system specified by distance rho, angle theta and distance z.
class vcsl_cylindrical
  : public vcsl_spatial
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor.
  explicit vcsl_cylindrical(void);

  //: Destructor
  virtual ~vcsl_cylindrical() {}

  //***************************************************************************
  // Because VXL does not necessarily use dynamic_cast<>
  //***************************************************************************

  virtual const vcsl_cylindrical *cast_to_cylindrical(void) const { return this; }
};

#endif // VCSL_CYLINDRICAL_H
