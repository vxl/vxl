#include <vcsl/vcsl_rotation.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_rotation::vcsl_rotation(void)
{
  _angle=0;
  _axis=0;
  _mode_2d=false;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_rotation::~vcsl_rotation()
{
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Is `this' invertible at time `time'?
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
bool vcsl_rotation::is_invertible(const double time) const
{
  return true;
}

//---------------------------------------------------------------------------
// Is `this' correctly set ?
//---------------------------------------------------------------------------
bool vcsl_rotation::is_valid(void) const
{
  return (_axis!=0)&&(_angle!=0)
    &&(
       ((_beat==0)&&(_interpolator==0)&&
        (_axis->size()==1)&&(_angle->size()==1)
        )
       ||
       (
        (_beat!=0)&&(_interpolator!=0)
        &&(_beat->size()==(_interpolator->size()+1))
        &&(_beat->size()==_axis->size())
        &&(_beat->size()==_angle->size())
        ));
}

//---------------------------------------------------------------------------
// Are `new_axis' a list of unit axes ?
//---------------------------------------------------------------------------
bool vcsl_rotation::are_unit_axes(list_of_vectors &new_axis) const
{
  bool result;
  const double epsilon=0.001;

  list_of_vectors::const_iterator i;

  result=true;
  for(i=new_axis.begin();i!=new_axis.end()&&result;++i)
    result=(((*i)->two_norm())-1)<epsilon;

  return result;
}

//---------------------------------------------------------------------------
// Is `this' a 2D rotation ?
//---------------------------------------------------------------------------
bool vcsl_rotation::is_2d(void) const
{
  return _mode_2d;
}

//---------------------------------------------------------------------------
  //: Is `this' a 3D rotation ?
//---------------------------------------------------------------------------
bool vcsl_rotation::is_3d(void) const
{
  return !_mode_2d;
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
// Set `this' as a 2D rotation
//---------------------------------------------------------------------------
void vcsl_rotation::set_2d(void)
{
  _mode_2d=true;
}

//---------------------------------------------------------------------------
// Set `this' as a 3D rotation
//---------------------------------------------------------------------------
void vcsl_rotation::set_3d(void)
{
  _mode_2d=false;
}

//***************************************************************************
// Transformation parameters
//***************************************************************************

//---------------------------------------------------------------------------
// Set the parameters of a static 2D rotation
//---------------------------------------------------------------------------
void vcsl_rotation::set_static_2d(const double new_angle)
{
  _mode_2d=true;
  if(_angle==0||_angle->size()!=1)
    _angle=new vcl_vector<double>(1);
  (*_angle)[0]=new_angle;
  _beat=0;
  _interpolator=0;
}

//---------------------------------------------------------------------------
// Set the parameters of a static 3D rotation
//---------------------------------------------------------------------------
void vcsl_rotation::set_static(const double new_angle,
                               vnl_vector<double> &new_axis)
{
  _mode_2d=false;
  if(_angle==0||_angle->size()!=1)
    {
      _angle=new vcl_vector<double>(1);
      _axis=new vcl_vector<vnl_vector<double> *>(1);
    }
  (*_angle)[0]=new_angle;
  (*_axis)[0]=&new_axis;
  _beat=0;
  _interpolator=0;
}

//---------------------------------------------------------------------------
// Set the angle variation along the time
//---------------------------------------------------------------------------
void vcsl_rotation::set_angle(list_of_scalars &new_angle)
{
  if(_angle!=0&&_angle->size()==1) // static rotation
    delete _angle;
  _angle=&new_angle;
}

//---------------------------------------------------------------------------
// Return the angle variation along the time
//---------------------------------------------------------------------------
list_of_scalars *vcsl_rotation::angle(void) const
{
  return _angle;
}

//---------------------------------------------------------------------------
// Set the direction vector variation along the time
// REQUIRE: are_unit_axes(new_axis)
//---------------------------------------------------------------------------
void vcsl_rotation::set_axis(list_of_vectors &new_axis)
{
  // require
  assert(are_unit_axes(new_axis));

  if(_axis!=0&&_axis->size()==1) // static rotation
    delete _axis;

  _axis=&new_axis;
}

//---------------------------------------------------------------------------
// Return the angle variation along the time
//---------------------------------------------------------------------------
list_of_vectors *vcsl_rotation::axis(void) const
{
  return _axis;
}
  
//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// Image of `v' by `this'
// REQUIRE: is_valid()
// REQUIRE: (is_2d()&&v.size()==2)||(is_3d()&&v.size()==3)
//---------------------------------------------------------------------------
vnl_vector<double> *vcsl_rotation::execute(const vnl_vector<double> &v,
                                           const double time) const
{
  // require
  assert(is_valid());
  assert((is_2d()&&v.size()==2)||(is_3d()&&v.size()==3));

  vnl_vector<double> *result;
  vnl_quaternion<double> *q;
  const vnl_vector<double> *tmp;
  vnl_vector<double> *tmp2;


  q=quaternion(time);
  if(_mode_2d)
    {
      tmp2=new vnl_vector<double>(3);
      tmp2->put(0,v.get(0));
      tmp2->put(1,v.get(1));
      tmp2->put(2,0);
      tmp=tmp2;
    }
  else
    tmp=&v;
  result=new vnl_vector<double>(q->rotate(*tmp));
  if(_mode_2d)
    {
      delete tmp;
      tmp=result;
      result=new vnl_vector<double>(2);
      result->put(0,tmp->get(0));
      result->put(1,tmp->get(1));
      delete tmp;
    }
  delete q;

  return result;
}

//---------------------------------------------------------------------------
// Image of `v' by the inverse of `this'
// REQUIRE: is_valid()
// REQUIRE: is_invertible(time)
// REQUIRE: (is_2d()&&v.size()==2)||(is_3d()&&v.size()==3)
//---------------------------------------------------------------------------
vnl_vector<double> *vcsl_rotation::inverse(const vnl_vector<double> &v,
                                           const double time) const
{
  // require
  assert(is_valid());
  assert(is_invertible(time));
  assert((is_2d()&&v.size()==2)||(is_3d()&&v.size()==3));

  vnl_vector<double> *result;
  vnl_quaternion<double> *q0;
  vnl_quaternion<double> *q1;
  const vnl_vector<double> *tmp;
  vnl_vector<double> *tmp2;

  q0=quaternion(time);
  q1=new vnl_quaternion<double>(q0->conjugate());
  delete q0;

  if(_mode_2d)
    {
      tmp2=new vnl_vector<double>(3);
      tmp2->put(0,v.get(0));
      tmp2->put(1,v.get(1));
      tmp2->put(2,0);
      tmp=tmp2;
    }
  else
    tmp=&v;
  result=new vnl_vector<double>(q1->rotate(*tmp));
  if(_mode_2d)
    {
      delete tmp;
      tmp=result;
      result=new vnl_vector<double>(2);
      result->put(0,tmp->get(0));
      result->put(1,tmp->get(1));
      delete tmp;
    }
  delete q1;

  return result;
}

//---------------------------------------------------------------------------
// Compute the value of the quaternion at time `time'
//---------------------------------------------------------------------------
vnl_quaternion<double> *vcsl_rotation::quaternion(const double time) const
{
  vnl_quaternion<double> *result;

  vnl_vector<double> *axis_2d;
  int i;
  vnl_quaternion<double> *q0;
  vnl_quaternion<double> *q1;

  if(_beat==0) // static
    {
      if(_mode_2d)
        {
          axis_2d=new vnl_vector<double>(3);
          axis_2d->put(0,0);
          axis_2d->put(1,0);
          axis_2d->put(2,1);
          result=new vnl_quaternion<double>(*axis_2d,(*_angle)[0]);
          delete axis_2d;
        }
      else
        result=new vnl_quaternion<double>(*(*_axis)[0],(*_angle)[0]);
    }
  else
    {
      i=matching_interval(time);
      
      if(_mode_2d)
        {
          axis_2d=new vnl_vector<double>(3);
          axis_2d->put(0,0);
          axis_2d->put(1,0);
          axis_2d->put(2,1);
        }
      
      switch((*_interpolator)[i])
        {
        case vcsl_linear:
          if(_mode_2d)
            {
              q0=new vnl_quaternion<double>(*axis_2d,(*_angle)[i]);
              q1=new vnl_quaternion<double>(*axis_2d,(*_angle)[i+1]);
            }
          else
            {
              q0=new vnl_quaternion<double>(*(*_axis)[i],(*_angle)[i]);
              q1=new vnl_quaternion<double>(*(*_axis)[i+1],(*_angle)[i+1]);
            }
          result=lqi(*q0,*q1,i,time);
          delete q1;
          delete q0;
          break;
        case vcsl_cubic:
          assert(false); // Not yet implemented
          break;
        case vcsl_spline:
          assert(false); // Not yet implemented
          break;
        default:
          assert(false); // Impossible
          break;
        }

      if(_mode_2d)
        delete axis_2d;
    }
  return result;
}
