// This is core/vcsl/vcsl_matrix.h
#ifndef vcsl_matrix_h
#define vcsl_matrix_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author François BERTEL
//
// \verbatim
// Modifications
// 2000/07/19 François BERTEL Creation.
// 2002/01/22 Peter Vanroose - return type of execute() and inverse() changed to non-ptr
// 2002/01/28 Peter Vanroose - vcl_vector member matrix_ changed to non-ptr
// 2002/12/30 Peter Vanroose - removed unused and commented out member _vector
// \endverbatim

#include <vcsl/vcsl_matrix_param.h>
#include <vcsl/vcsl_matrix_sptr.h>
#include <vcsl/vcsl_spatial_transformation.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

class vcsl_matrix
  :public vcsl_spatial_transformation
{
 public:

  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_matrix(void) {}

  //: Destructor
  virtual ~vcsl_matrix() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'?
  //  REQUIRE: valid_time(time)
  virtual bool is_invertible(double time) const;

  //: Is `this' correctly set ?
  virtual bool is_valid(void) const;

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the parameters of a static translation
  void set_static( vcsl_matrix_param_sptr new_matrix);

  //: Set the direction vector variation along the time
  virtual void set_matrix(list_of_vcsl_matrix_param_sptr const& new_matrix);

  virtual list_of_vcsl_matrix_param_sptr matrix_list(void) const { return matrix_; }

  virtual vnl_vector<double> execute(const vnl_vector<double> &v,
                                     double time) const;

  virtual vnl_vector<double> inverse(const vnl_vector<double> &v,
                                     double time) const;

 protected:

  vnl_matrix<double> param_to_matrix( vcsl_matrix_param_sptr from,bool type ) const;

  vnl_matrix<double> matrix_value(double time, bool type) const;

  list_of_vcsl_matrix_param_sptr matrix_;
};

#endif // vcsl_matrix_h
