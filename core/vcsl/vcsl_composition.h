#ifndef VCSL_COMPOSITION_H
#define VCSL_COMPOSITION_H
//*****************************************************************************
//
// .NAME vcsl_composition - Composition of transformations
// .LIBRARY vcsl
// .HEADER  vxl Package
// .INCLUDE vcsl/vcsl_composition.h
// .FILE    vcsl/vcsl_composition.cxx
//
// .SECTION Description
// This transformation handles a composition of transformations, that is,
// at a given time, all the transformations are applied on a given point
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/08/08 François BERTEL Creation.
//*****************************************************************************

#include <vcsl/vcsl_composition_ref.h>

//*****************************************************************************
// External declarations
//*****************************************************************************
#include <vcsl/vcsl_spatial_transformation.h>
#include <vcl_vector.h>

class vcsl_composition
  :public vcsl_spatial_transformation
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_composition(void);

  //: Destructor
  virtual ~vcsl_composition();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'?
  //: REQUIRE: valid_time(time)
  virtual bool is_invertible(const double time) const;

  //: Is `this' correctly set ?
  virtual bool is_valid(void) const;

  //: Return the list of transformations
  virtual vcl_vector<vcsl_spatial_transformation_ref> *composition(void) const;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set the list of transformations of the composition
  //: The transformations are performed in the order of the list
  virtual void
  set_composition(vcl_vector<vcsl_spatial_transformation_ref> &new_transformations);


  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Image of `v' by `this'
  //: REQUIRE: is_valid()
  virtual vnl_vector<double> *execute(const vnl_vector<double> &v,
                                      const double time) const;

  //: Image of `v' by the inverse of `this'
  //: REQUIRE: is_valid()
  //: REQUIRE: is_invertible(time)
  virtual vnl_vector<double> *inverse(const vnl_vector<double> &v,
                                      const double time) const;
protected:
  vcl_vector<vcsl_spatial_transformation_ref> *transformations_;
};

#endif // #ifndef VCSL_COMPOSITION_H
