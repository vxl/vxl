// This is core/vcsl/vcsl_rotation.cxx
#include "vcsl_rotation.h"
#include <vcl_cassert.h>

//---------------------------------------------------------------------------
// Are `new_axis' a list of unit axes ?
//---------------------------------------------------------------------------
bool vcsl_rotation::are_unit_axes(list_of_vectors const& new_axis) const
{
  const double epsilon=0.001;

  list_of_vectors::const_iterator i;

  bool result=true;
  for (i=new_axis.begin();result&&i!=new_axis.end();++i)
    result=(((*i).two_norm())-1)<epsilon;

  return result;
}

//---------------------------------------------------------------------------
// Set the parameters of a static 2D rotation
//---------------------------------------------------------------------------
void vcsl_rotation::set_static_2d(double new_angle)
{
  mode_2d_=true;
  angle_.clear(); angle_.push_back(new_angle);
  vcsl_spatial_transformation::set_static();
}

//---------------------------------------------------------------------------
// Set the parameters of a static 3D rotation
//---------------------------------------------------------------------------
void vcsl_rotation::set_static(double new_angle,
                               vnl_vector<double> const& new_axis)
{
  mode_2d_=false;
  angle_.clear(); angle_.push_back(new_angle);
  axis_.clear(); axis_.push_back(new_axis);
  vcsl_spatial_transformation::set_static();
}

//---------------------------------------------------------------------------
// Set the direction vector variation along the time
// REQUIRE: are_unit_axes(new_axis)
//---------------------------------------------------------------------------
void vcsl_rotation::set_axis(list_of_vectors const& new_axis)
{
  // require
  assert(are_unit_axes(new_axis));

  axis_=new_axis;
}

//---------------------------------------------------------------------------
// Image of `v' by `this'
// REQUIRE: is_valid()
// REQUIRE: (is_2d()&&v.size()==2)||(is_3d()&&v.size()==3)
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_rotation::execute(const vnl_vector<double> &v,
                                          double time) const
{
  // require
  assert(is_valid());
  assert((is_2d()&&v.size()==2)||(is_3d()&&v.size()==3));

  vnl_quaternion<double> q=quaternion(time);
  vnl_vector<double> result(3);
  if (mode_2d_)
  {
    result.put(0,v.get(0));
    result.put(1,v.get(1));
    result.put(2,0);
  }
  else
    result=v;
  result = q.rotate(result);
  if (mode_2d_)
  {
    vnl_vector<double> tmp(2);
    result.put(0,result.get(0));
    result.put(1,result.get(1));
    return tmp;
  }
  else
    return result;
}

//---------------------------------------------------------------------------
// Image of `v' by the inverse of `this'
// REQUIRE: is_valid()
// REQUIRE: is_invertible(time)
// REQUIRE: (is_2d()&&v.size()==2)||(is_3d()&&v.size()==3)
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_rotation::inverse(const vnl_vector<double> &v,
                                          double time) const
{
  // require
  assert(is_valid());
  assert(is_invertible(time));
  assert((is_2d()&&v.size()==2)||(is_3d()&&v.size()==3));

  vnl_vector<double> result(3);

  if (mode_2d_)
  {
    result.put(0,v.get(0));
    result.put(1,v.get(1));
    result.put(2,0);
  }
  else
    result=v;
  vnl_quaternion<double> q=quaternion(time);
  result = q.conjugate().rotate(result);
  if (mode_2d_)
  {
    vnl_vector<double> tmp(2);
    result.put(0,result.get(0));
    result.put(1,result.get(1));
    return tmp;
  }
  else
    return result;
}

//---------------------------------------------------------------------------
// Compute the value of the quaternion at time `time'
//---------------------------------------------------------------------------
vnl_quaternion<double> vcsl_rotation::quaternion(double time) const
{
  vnl_quaternion<double> result;

  if (this->duration()==0) // static
  {
    if (mode_2d_)
    {
      vnl_vector<double> axis_2d(3);
      axis_2d.put(0,0);
      axis_2d.put(1,0);
      axis_2d.put(2,1);
      result=vnl_quaternion<double>(axis_2d,angle_[0]);
    }
    else
      result=vnl_quaternion<double>(axis_[0],angle_[0]);
  }
  else
  {
    int i=matching_interval(time);
    vnl_vector<double> axis_2d(3);

    if (mode_2d_)
    {
      axis_2d.put(0,0);
      axis_2d.put(1,0);
      axis_2d.put(2,1);
    }

    switch (interpolator_[i])
    {
     case vcsl_linear:
     {
      vnl_quaternion<double> q0, q1;
      if (mode_2d_)
      {
        q0=vnl_quaternion<double>(axis_2d,angle_[i]);
        q1=vnl_quaternion<double>(axis_2d,angle_[i+1]);
      }
      else
      {
        q0=vnl_quaternion<double>(axis_[i],angle_[i]);
        q1=vnl_quaternion<double>(axis_[i+1],angle_[i+1]);
      }
      result=lqi(q0,q1,i,time);
      break;
     }
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
  return result;
}
