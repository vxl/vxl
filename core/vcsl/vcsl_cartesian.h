#ifndef VCSL_CARTESIAN_H
#define VCSL_CARTESIAN_H
//*****************************************************************************
//
// .NAME vcsl_cartesian - Cartesian coordinate system
// .LIBRARY vcsl
// .HEADER  vcsl/vcsl_cartesian.h
// .INCLUDE vcsl/vcsl_cartesian_ref.h
// .INCLUDE vcsl/vcsl_coordinate_system.h
// .FILE    vcsl/vcsl_cartesian.cxx
//
// .SECTION Description
// The axes of a cartesian coordinate system are orthogonal and normally right
// handed. This is not necessarily a spatial coordinate system.
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_cartesian_ref.h>

//*****************************************************************************
// External declarations
//*****************************************************************************
#include <vcsl/vcsl_coordinate_system.h>

class vcsl_cartesian
  : public vcsl_coordinate_system
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor. Set itself right handed
  explicit vcsl_cartesian(void);

  //: Destructor
  virtual ~vcsl_cartesian();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Are the axes of `this' right handed ?
  virtual bool is_right_handed(void) const;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set whether the coordinate system is right handed or not
  virtual void set_right_handed(const bool new_right_handed);

protected:
  //***************************************************************************
  // Implementation
  //***************************************************************************

  //:  True if the axes of `this' are right handed
  bool _right_handed;
};

#endif // #ifndef VCSL_CARTESIAN_H
