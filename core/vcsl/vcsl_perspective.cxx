// This is core/vcsl/vcsl_perspective.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "vcsl_perspective.h"
#include <vcl_cassert.h>

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Is `this' invertible at time `time'? Never !
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
bool vcsl_perspective::is_invertible(double time) const
{
  // require
  assert(valid_time(time));
  return false;
}

//---------------------------------------------------------------------------
// Is `this' correctly set ?
//---------------------------------------------------------------------------
bool vcsl_perspective::is_valid(void) const
{
  return (beat_.size()==0&&focal_.size()==1) ||
         (beat_.size()==interpolator_.size()+1&&beat_.size()==focal_.size());
}

//***************************************************************************
// Transformation parameters
//***************************************************************************

//---------------------------------------------------------------------------
// Set the focal in meters of a static perspective projection
//---------------------------------------------------------------------------
void vcsl_perspective::set_static(double new_focal)
{
  focal_.clear(); focal_.push_back(new_focal);
  vcsl_spatial_transformation::set_static();
}

//---------------------------------------------------------------------------
// Set the focal variation along the time in meters
//---------------------------------------------------------------------------
void vcsl_perspective::set_focal(list_of_scalars const& new_focal)
{
  focal_=new_focal;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// Image of `v' by `this'
// REQUIRE: is_valid()
// REQUIRE: v.size()==3 and v[2]<0
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_perspective::execute(const vnl_vector<double> &v,
                                             double time) const
{
  assert(is_valid());
  assert(v.size()==3);
  assert(v[2]<0);

  double f;
  double lambda;

  vnl_vector<double> result(2);
  f=focal_value(time);
  lambda=-f/v[2];
  result[0]=v[0]*lambda;
  result[1]=v[1]*lambda;
  return result;
}

//---------------------------------------------------------------------------
// Image of `v' by the inverse of `this'
// REQUIRE: is_valid()
// REQUIRE: is_invertible(time) and v.size()==2
// The first pre-condition is never true. You can not use this method
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_perspective::inverse(const vnl_vector<double> &v,
                                             double time) const
{
  // require
  assert(is_valid());
  assert((is_invertible(time))&&(v.size()==2));
  return vnl_vector<double>(); // To avoid compilation warning/error message
}

//---------------------------------------------------------------------------
// Compute the parameter at time `time'
//---------------------------------------------------------------------------
double vcsl_perspective::focal_value(double time) const
{
  if (beat_.size()==0) // static
    return focal_[0];
  else
  {
    int i=matching_interval(time);
    switch (interpolator_[i])
    {
     case vcsl_linear:
      return lsi(focal_[i],focal_[i+1],i,time);
     case vcsl_cubic:
      assert(!"vcsl_cubic net yet implemented");
      break;
     case vcsl_spline:
      assert(!"vcsl_spline net yet implemented");
      break;
     default:
      assert(!"This is impossible");
      break;
    }
  }
  return 0.0; // never reached if asserts are in effect
}
