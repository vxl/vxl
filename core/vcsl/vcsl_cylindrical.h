#ifndef VCSL_CYLINDRICAL_H
#define VCSL_CYLINDRICAL_H
//*****************************************************************************
//
// .NAME vcsl_cylindrical - 3D coordinate system specified by distance rho,
//                          angle theta and distance z.
// .LIBRARY vcsl
// .HEADER  vxl Package
// .INCLUDE vcsl/vcsl_cylindrical.h
// .FILE    vcsl/vcsl_cylindrical.cxx
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_cylindrical_sptr.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vcsl/vcsl_spatial.h>

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
  virtual ~vcsl_cylindrical();

  //***************************************************************************
  // Because VXL does not use dynamic_cast<> :-(
  //***************************************************************************

  virtual const vcsl_cylindrical *cast_to_cylindrical(void) const;
};

#endif // #ifndef VCSL_CYLINDRICAL_H
