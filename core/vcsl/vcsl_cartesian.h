#ifndef VCSL_CARTESIAN_H
#define VCSL_CARTESIAN_H
//:
// \file 
// \brief Descibes vcsl_cartesian - Cartesian coordinate system
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/28 François BERTEL Creation. Adapted from IUE
// \endverbatim

#include <vcsl/vcsl_cartesian_sptr.h>

//*****************************************************************************
// External declarations
//*****************************************************************************
#include <vcsl/vcsl_coordinate_system.h>

//: Cartesian coordinate system
// The axes of a cartesian coordinate system are orthogonal and normally right
// handed. This is not necessarily a spatial coordinate system.
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
