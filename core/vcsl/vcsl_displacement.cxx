// This is core/vcsl/vcsl_displacement.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "vcsl_displacement.h"
#include <vcl_cassert.h>

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Is `this' correctly set ?
//---------------------------------------------------------------------------
bool vcsl_displacement::is_valid(void) const
{
  return
       ((beat_.size()==0)&&(interpolator_.size()==0)&&(point_.size()==1)&&
        (axis_.size()==1)&&(angle_.size()==1)
        )
       ||
       (
        (beat_.size()==interpolator_.size()+1)
        &&(beat_.size()==point_.size())
        &&(beat_.size()==axis_.size())
        &&(beat_.size()==angle_.size())
        );
}

//***************************************************************************
// Transformation parameters
//***************************************************************************

//---------------------------------------------------------------------------
// Set the point for a static displacement
//---------------------------------------------------------------------------
void vcsl_displacement::set_static_point(vnl_vector<double> const& new_point)
{
  point_.clear(); point_.push_back(new_point);
  vcsl_spatial_transformation::set_static();
}

//---------------------------------------------------------------------------
// Set the variation of the point of the axis along the time
//---------------------------------------------------------------------------
void vcsl_displacement::set_point(list_of_vectors const& new_point)
{
  point_=new_point;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// Image of `v' by `this'
// REQUIRE: is_valid()
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_displacement::execute(const vnl_vector<double> &v,
                                              double time) const
{
  // require
  assert(is_valid());
  assert((is_2d()&&v.size()==2)||(is_3d()&&v.size()==3));

  vnl_vector<double> translation=vector_value(time);

  vnl_vector<double> result(3);

  if (mode_2d_)
  {
    result.put(0,v.get(0)-translation.get(0));
    result.put(1,v.get(1)-translation.get(1));
    result.put(2,0);
  }
  else
    result=v-translation;

  vnl_quaternion<double> q=quaternion(time);
  result = q.rotate(result);

  if (mode_2d_)
  {
    vnl_vector<double> tmp(2);
    tmp.put(0,result.get(0)+translation.get(0));
    tmp.put(1,result.get(1)+translation.get(1));
    return tmp;
  }
  else
    return result+translation;
}

//---------------------------------------------------------------------------
// Image of `v' by the inverse of `this'
// REQUIRE: is_valid()
// REQUIRE: is_invertible(time)
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_displacement::inverse(const vnl_vector<double> &v,
                                              double time) const
{
  // require
  assert(is_valid());
  assert(is_invertible(time));
  assert((is_2d()&&v.size()==2)||(is_3d()&&v.size()==3));

  vnl_vector<double> translation=vector_value(time);

  vnl_vector<double> result(3);

  if (mode_2d_)
  {
    result.put(0,v.get(0)-translation.get(0));
    result.put(1,v.get(1)-translation.get(1));
    result.put(2,0);
  }
  else
    result=v-translation;

  vnl_quaternion<double> q=quaternion(time);
  result = q.conjugate().rotate(result);

  if (mode_2d_)
  {
    vnl_vector<double> tmp(2);
    tmp.put(0,result.get(0)+translation.get(0));
    tmp.put(1,result.get(1)+translation.get(1));
    return tmp;
  }
  else
    return result+translation;
}

//---------------------------------------------------------------------------
// Compute the value of the vector at time `time'
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_displacement::vector_value(double time) const
{
  if (beat_.size()==0) // static
    return point_[0];
  else
  {
    int i=matching_interval(time);
    switch (interpolator_[i])
    {
     case vcsl_linear:
      return lvi(point_[i],point_[i+1],i,time);
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
