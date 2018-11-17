// This is core/vcsl/vcsl_matrix.cxx
#include <cmath>
#include <iostream>
#include "vcsl_matrix.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//---------------------------------------------------------------------------
// Is `this' invertible at time `time'?
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
bool vcsl_matrix::is_invertible(double time) const
{
  // require
  assert(valid_time(time));

  return true;
}

//---------------------------------------------------------------------------
// Set the parameters of a static translation
//---------------------------------------------------------------------------
void vcsl_matrix::set_static( const vcsl_matrix_param_sptr& new_matrix)
{
  matrix_.clear(); matrix_.push_back(new_matrix);
  vcsl_spatial_transformation::set_static();
}

//---------------------------------------------------------------------------
// Image of `v' by `this'
// REQUIRE: is_valid()
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_matrix::execute(const vnl_vector<double> &v,
                                        double time) const
{
  // require
  assert(is_valid());
  assert(v.size()==3);

  vnl_vector_fixed<double,4> temp(v(0),v(1),v(2),1.0);

  vnl_matrix<double> value=matrix_value(time,true);
  return value*temp;
}

//---------------------------------------------------------------------------
// Image of `v' by the inverse of `this'
// REQUIRE: is_valid()
// REQUIRE: is_invertible(time)
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_matrix::inverse(const vnl_vector<double> &v,
                                        double time) const
{
  assert(is_valid());
  assert(v.size()==3);

  vnl_vector_fixed<double,4> temp(v(0),v(1),v(2),1.0);

  vnl_matrix<double> value=matrix_value(time,false);
  return value*temp;
}

//---------------------------------------------------------------------------
// Compute the value of the parameter at time `time'
//---------------------------------------------------------------------------

vnl_matrix<double> vcsl_matrix::matrix_value(double time, bool type) const
{
  if (this->duration()==0) // static
    return param_to_matrix(matrix_[0],type);

  else
  {
    int i=matching_interval(time);
    switch (interpolator_[i])
    {
     case vcsl_linear:
      return lmi(param_to_matrix(matrix_[i],type),param_to_matrix(matrix_[i+1],type),i,time);
     case vcsl_cubic:
      assert(!"vcsl_cubic not yet implemented");
      break;
     case vcsl_spline:
      assert(!"vcsl_spline not yet implemented");
      break;
     default:
      assert(!"This is impossible");
      break;
    }
  }
  return vnl_matrix<double>(); // never reached if asserts are in effect
}

vnl_matrix<double> vcsl_matrix::param_to_matrix(const vcsl_matrix_param_sptr& from,bool type ) const
{
  int coef =1;
  if (type) coef = -1;

  vnl_matrix<double> T(3, 4, 0.0);
  T(0,0) = 1.0; T(1,1) = 1.0; T(2,2) = 1.0;
  T(0,3) = -coef*from->xl; T(1,3) = -coef*from->yl; T(2,3) = -coef*from->zl;
  std::cout << "Translation:\n" << T;
  // Rotation matrix (Extrinsic parameters)
  double co = std::cos(coef*from->omega), so = std::sin(coef*from->omega);
  double cp = std::cos(coef*from->phi),   sp = std::sin(coef*from->phi);
  double ck = std::cos(coef*from->kappa), sk = std::sin(coef*from->kappa);
  vnl_matrix<double> R(4, 4, 0.0);
  R(0,0) = cp*ck; R(0,1) = so*sp*ck+co*sk; R(0,2) = -co*sp*ck+so*sk;
  R(1,0) = -cp*sk; R(1,1) = -so*sp*sk+co*ck;  R(1,2) = co*sp*sk+so*ck;
  R(2,0) = sp; R(2,1) = -so*cp; R(2,2) = co*cp;
  R(3,0)=R(3,1)=R(3,2)=R(0,3)=R(1,3)=R(2,3)=0;
  R(3,3)=1;
  std::cout << "Rotation:\n" << R;

  if (type)
    return T*R;
  else
  {
    vnl_matrix<double> temp(3,3);
    for (int i=0;i<3;i++)
      for (int j=0;j<3;j++)
        temp(i,j)=R(i,j);
    return temp*T;
  }
}
