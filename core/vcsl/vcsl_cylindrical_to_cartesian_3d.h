#ifndef VCSL_CYLINDRICAL_TO_CARTESIAN_3D_H
#define VCSL_CYLINDRICAL_TO_CARTESIAN_3D_H
//*****************************************************************************
//
// .NAME vcsl_cylindrical_to_cartesian_3d
// .LIBRARY vcsl
// .HEADER  vcsl/vcsl_cylindrical_to_cartesian_3d.h
// .INCLUDE vcsl/vcsl_cylindrical_to_cartesian_3d_ref.h
// .INCLUDE vcsl/vcsl_spatial_transformation
// .FILE    vcsl/vcsl_cylindrical_to_cartesian_3d.cxx
//
// .SECTION Description
// Convert 3D cartesian coordinates to cylindrical coordinates
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/08/08 François BERTEL Creation.
//*****************************************************************************

#include <vcsl/vcsl_cylindrical_to_cartesian_3d_ref.h>

//*****************************************************************************
// External declarations
//*****************************************************************************
#include <vcsl/vcsl_spatial_transformation.h>

class vcsl_cylindrical_to_cartesian_3d
  : public vcsl_spatial_transformation
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************
  
  //: Destructor
  virtual ~vcsl_cylindrical_to_cartesian_3d();

  //***************************************************************************
  // Status report
  //***************************************************************************
  
  //: Is `this' invertible at time `time'?
  //: REQUIRE: valid_time(time)
  virtual bool is_invertible(const double time) const;

  //: Is `this' correctly set ?
  virtual bool is_valid(void) const;

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Image of `v' by `this'
  //: REQUIRE: is_valid()
  //: REQUIRE: v.size()==3
  virtual vnl_vector<double> *execute(const vnl_vector<double> &v,
                                      const double time) const;

  //: Image of `v' by the inverse of `this'
  //: REQUIRE: is_valid()
  //: REQUIRE: is_invertible(time)
  //: REQUIRE: v.size()==3
  virtual vnl_vector<double> *inverse(const vnl_vector<double> &v,
                                      const double time) const;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_length object
  static vcsl_cylindrical_to_cartesian_3d_ref instance(void);

protected:
  //: Default constructor
  explicit vcsl_cylindrical_to_cartesian_3d(void);

  //: Reference to the unique vcsl_cylindrical_to_cartesian_3d object
  static vcsl_cylindrical_to_cartesian_3d_ref _instance;
};

#endif // #ifndef VCSL_CYLINDRICAL_TO_CARTESIAN_3D_H
