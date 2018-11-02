// This is core/vcsl/vcsl_cylindrical_to_cartesian_3d.cxx
#include <cmath>
#include "vcsl_cylindrical_to_cartesian_3d.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//---------------------------------------------------------------------------
// Is `this' invertible at time `time'?
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
bool vcsl_cylindrical_to_cartesian_3d::is_invertible(double time) const
{
  // require
  assert(valid_time(time));

  return true;
}

//---------------------------------------------------------------------------
// Image of `v' by `this'
// REQUIRE: is_valid()
// REQUIRE: v.size()==3
//---------------------------------------------------------------------------
vnl_vector<double>
vcsl_cylindrical_to_cartesian_3d::execute(const vnl_vector<double> &v,
                                          double /*time*/) const
{
  // require
  assert(is_valid());
  assert(v.size()==3);

  vnl_vector<double> result(3);

  double rho=v.get(0);
  double theta=v.get(1);
  double z=v.get(2);

  double x=rho*std::cos(theta);
  double y=rho*std::sin(theta);

  result.put(0,x);
  result.put(1,y);
  result.put(2,z);

  return result;
}

//---------------------------------------------------------------------------
// Image of `v' by the inverse of `this'
// REQUIRE: is_valid()
// REQUIRE: is_invertible(time)
// REQUIRE: v.size()==3
//---------------------------------------------------------------------------
vnl_vector<double>
vcsl_cylindrical_to_cartesian_3d::inverse(const vnl_vector<double> &v,
                                          double time) const
{
  // require
  assert(is_valid());
  assert(is_invertible(time));
  assert(v.size()==3);

  vnl_vector<double> result(3);

  double x=v.get(0);
  double y=v.get(1);
  double z=v.get(2);

  double rho=std::sqrt(x*x+y*y);
  double theta=std::atan2(y,x);

  result.put(0,rho);
  result.put(1,theta);
  result.put(2,z);

  return result;
}

// Return the reference to the unique vcsl_length object
vcsl_cylindrical_to_cartesian_3d_sptr
vcsl_cylindrical_to_cartesian_3d::instance()
{
  static vcsl_cylindrical_to_cartesian_3d_sptr instance_
         = new vcsl_cylindrical_to_cartesian_3d;
  return instance_;
}
