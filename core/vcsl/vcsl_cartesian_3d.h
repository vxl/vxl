#ifndef vcsl_cartesian_3d_h
#define vcsl_cartesian_3d_h
//:
// \file
// \brief 3D Cartesian coordinate system
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2004/09/10 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vcsl/vcsl_cartesian_3d_sptr.h>
#include <vcsl/vcsl_spatial.h>

//: 3D Cartesian coordinate system
class vcsl_cartesian_3d
  : public vcsl_spatial
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor.
  explicit vcsl_cartesian_3d(void);

  //: Destructor
  virtual ~vcsl_cartesian_3d() {}

  //***************************************************************************
  // Because VXL does not necessarily use dynamic_cast<>
  //***************************************************************************

  virtual const vcsl_cartesian_3d *cast_to_cartesian_3d(void)const{return this;}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Are the axes of `this' right handed ?
  virtual bool is_right_handed(void) const { return right_handed_; }

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set whether the coordinate system is right handed or not
  virtual void set_right_handed(bool val) { right_handed_ = val; }

 protected:
  //***************************************************************************
  // Implementation
  //***************************************************************************

  //:  True if the axes of `this' are right handed
  bool right_handed_;
};

#endif // vcsl_cartesian_3d_h
