#ifndef vcsl_composition_h
#define vcsl_composition_h

//:
// \file
// \brief Composition of transformations
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/28 François BERTEL Creation. Adapted from IUE
// \endverbatim


#include <vcsl/vcsl_composition_sptr.h>

//*****************************************************************************
// External declarations
//*****************************************************************************
#include <vcsl/vcsl_spatial_transformation.h>
#include <vcl_vector.h>

//: Composition of transformations
// This transformation handles a composition of transformations, that is,
// at a given time, all the transformations are applied on a given point
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
  //  REQUIRE: valid_time(time)
  virtual bool is_invertible(const double time) const;

  //: Is `this' correctly set ?
  virtual bool is_valid(void) const;

  //: Return the list of transformations
  virtual vcl_vector<vcsl_spatial_transformation_sptr> *composition(void) const;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set the list of transformations of the composition
  //  The transformations are performed in the order of the list
  virtual void
  set_composition(vcl_vector<vcsl_spatial_transformation_sptr> &new_transformations);


  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Image of `v' by `this'
  //  REQUIRE: is_valid()
  virtual vnl_vector<double> *execute(const vnl_vector<double> &v,
                                      const double time) const;

  //: Image of `v' by the inverse of `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: is_invertible(time)
  virtual vnl_vector<double> *inverse(const vnl_vector<double> &v,
                                      const double time) const;
protected:
  vcl_vector<vcsl_spatial_transformation_sptr> *transformations_;
};

#endif // vcsl_composition_h
