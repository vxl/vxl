// This is core/vcsl/vcsl_scale.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vcsl_scale.h"

#include <vcl_cassert.h>

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Is `this' invertible at time `time'?
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
bool vcsl_scale::is_invertible(double time) const
{
  // require
  assert(valid_time(time));

  return ((beat_.size()==0)&&(scale_[0]!=0.0))||(scale_value(time)!=0.0);
}

//---------------------------------------------------------------------------
// Is `this' correctly set ?
//---------------------------------------------------------------------------
bool vcsl_scale::is_valid(void) const
{
  return (scale_.size()!=0)&&
         ((beat_.size()==0&&interpolator_.size()==0&&scale_.size()==1) ||
          (beat_.size()==interpolator_.size()+1&&beat_.size()==scale_.size()));
}

//***************************************************************************
// Transformation parameters
//***************************************************************************

//---------------------------------------------------------------------------
// Set the scale value of a static scale
//---------------------------------------------------------------------------
void vcsl_scale::set_static(double new_scale)
{
  scale_.clear();
  scale_.push_back(new_scale);
  vcsl_spatial_transformation::set_static();
}

//---------------------------------------------------------------------------
//: Set the scale variation along the time
//---------------------------------------------------------------------------
void vcsl_scale::set_scale(vcl_vector<double> const& new_scale)
{
  scale_=new_scale;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// Image of `v' by `this'
// REQUIRE: is_valid()
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_scale::execute(const vnl_vector<double> &v,
                                       double time) const
{
  // require
  assert(is_valid());

  double value=scale_value(time);
  vnl_vector<double> result(v.size());
  for (unsigned int i=0;i<v.size();++i)
    result.put(i,value*v.get(i));

  return result;
}

//---------------------------------------------------------------------------
// Image of `v' by the inverse of `this'
// REQUIRE: is_valid()
// REQUIRE: is_invertible(time)
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_scale::inverse(const vnl_vector<double> &v,
                                       double time) const
{
  // require
  assert(is_valid());
  assert(is_invertible(time));

  double value=scale_value(time);
  vnl_vector<double> result(v.size());
  for (unsigned int i=0;i<v.size();++i)
    result.put(i,v.get(i)/value);

  return result;
}

//---------------------------------------------------------------------------
// Compute the value of the parameter at time `time'
//---------------------------------------------------------------------------
double vcsl_scale::scale_value(double time) const
{
  if (beat_.size()==0) // static
    return scale_[0];
  else
  {
    int i=matching_interval(time);
    switch (interpolator_[i])
    {
     case vcsl_linear:
      return lsi(scale_[i],scale_[i+1],i,time);
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
