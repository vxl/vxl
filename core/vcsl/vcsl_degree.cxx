// This is core/vcsl/vcsl_degree.cxx
#include "vcsl_degree.h"
#include <vnl/vnl_math.h>

//---------------------------------------------------------------------------
// Returns the number of units of `this' equal of the standard_unit for
// the dimension
//---------------------------------------------------------------------------
double vcsl_degree::units_per_standard_unit() const
{
  static double u = 180*vnl_math::one_over_pi; // = 57.29577951
  return u;
}

//---------------------------------------------------------------------------
// Return the reference to the unique vcsl_degree object
//---------------------------------------------------------------------------
vcsl_degree_sptr vcsl_degree::instance()
{
  static vcsl_degree_sptr instance_=new vcsl_degree;
  return instance_;
}
