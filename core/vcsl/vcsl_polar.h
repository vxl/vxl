#ifndef VCSL_POLAR_H
#define VCSL_POLAR_H
//*****************************************************************************
//
// .NAME vcsl_polar - Planar coordinate system specified by the parameters rho and theta
// .LIBRARY vcsl
// .HEADER  vxl Package
// .INCLUDE vcsl/vcsl_polar.h
// .FILE    vcsl/vcsl_polar.cxx
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_polar_sptr.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vcsl/vcsl_spatial.h>

class vcsl_polar
  : public vcsl_spatial
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor.
  explicit vcsl_polar(void);

  //: Destructor
  virtual ~vcsl_polar();

  //***************************************************************************
  // Because VXL does not use dynamic_cast<> :-(
  //***************************************************************************
  
  virtual const vcsl_polar *cast_to_polar(void) const;
};

#endif // #ifndef VCSL_POLAR_H
