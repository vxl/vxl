// This is core/vcsl/vcsl_translation.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vcsl_translation.h"
#include <vcl_cassert.h>

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Is `this' invertible at time `time'?
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
bool vcsl_translation::is_invertible(double time) const
{
  // require
  assert(valid_time(time));

  return true;
}

//---------------------------------------------------------------------------
// Is `this' correctly set ?
//---------------------------------------------------------------------------
bool vcsl_translation::is_valid(void) const
{
  return (beat_.size()==0&&vector_.size()==1) ||
         (beat_.size()==interpolator_.size()+1&&beat_.size()==vector_.size());
}

//***************************************************************************
// Transformation parameters
//***************************************************************************

//---------------------------------------------------------------------------
// Set the parameters of a static translation
//---------------------------------------------------------------------------
void vcsl_translation::set_static(vnl_vector<double> const& new_vector)
{
  vector_.clear(); vector_.push_back(new_vector);
  vcsl_spatial_transformation::set_static();
}

//---------------------------------------------------------------------------
//: Set the direction vector variation along the time
//---------------------------------------------------------------------------
void vcsl_translation::set_vector(list_of_vectors const& new_vector)
{
  vector_=new_vector;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// Image of `v' by `this'
// REQUIRE: is_valid()
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_translation::execute(const vnl_vector<double> &v,
                                             double time) const
{
  // require
  assert(is_valid());

  vnl_vector<double> value=vector_value(time);
  vnl_vector<double> result(v.size());
  for (unsigned int i=0;i<v.size();++i)
    result.put(i,v.get(i)+value.get(i));

  return result;
}

//---------------------------------------------------------------------------
// Image of `v' by the inverse of `this'
// REQUIRE: is_valid()
// REQUIRE: is_invertible(time)
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_translation::inverse(const vnl_vector<double> &v,
                                             double time) const
{
  // require
  assert(is_valid());
  assert(is_invertible(time));

  vnl_vector<double> value=vector_value(time);
  vnl_vector<double> result(v.size());
  for (unsigned int i=0;i<v.size();++i)
    result.put(i,v.get(i)-value.get(i));

  return result;
}

//---------------------------------------------------------------------------
// Compute the value of the parameter at time `time'
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_translation::vector_value(double time) const
{
  if (beat_.size()==0) // static
    return vector_[0];
  else
  {
    int i=matching_interval(time);
    switch (interpolator_[i])
    {
     case vcsl_linear:
      return lvi(vector_[i],vector_[i+1],i,time);
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
  return vnl_vector<double>(); // never reached if asserts are in effect
}
