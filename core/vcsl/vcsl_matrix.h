#ifndef vcsl_matrix_h
#define vcsl_matrix_h

#include <vcsl/vcsl_matrix_param.h>
#include <vcsl/vcsl_matrix_sptr.h>
#include <vcsl/vcsl_spatial_transformation.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>

class vcsl_matrix
  :public vcsl_spatial_transformation
{
public:
#if 0
  //: Direction vector variation along the time
  vcl_vector<vnl_vector<double> *> *_vector;
#endif

  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_matrix(void);

  //: Destructor
  virtual ~vcsl_matrix();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'?
  //  REQUIRE: valid_time(time)
  virtual bool is_invertible(const double time) const;

  //: Is `this' correctly set ?
  virtual bool is_valid(void) const;

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the parameters of a static translation
  virtual void set_static( vcsl_matrix_param_sptr new_matrix);

  //: Set the direction vector variation along the time
  virtual void set_matrix(list_of_vcsl_matrix_param_sptr &new_matrix);


  virtual list_of_vcsl_matrix_param_sptr *matrix_list(void) const;

  virtual vnl_vector<double> * execute(const vnl_vector<double> &v,
                                       const double time) const;

  virtual vnl_vector<double> * inverse(const vnl_vector<double> &v,
                                       const double time) const;

protected:

  vnl_matrix<double> param_to_matrix( vcsl_matrix_param_sptr from,bool type ) const;

  vnl_matrix<double> matrix_value(const double time, bool type) const;

  list_of_vcsl_matrix_param_sptr *matrix_;
};

#endif // vcsl_matrix_h
