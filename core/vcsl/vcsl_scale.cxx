#include <vcsl/vcsl_scale.h>

#include <vcl/vcl_cassert.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_scale::vcsl_scale(void)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_scale::~vcsl_scale()
{
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Is `this' invertible at time `time'?
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
bool vcsl_scale::is_invertible(const double time) const
{
  // require
  assert(valid_time(time));

  return ((_beat==0)&&((*_scale)[0]!=0))||(scale_value(time)!=0);
}

//---------------------------------------------------------------------------
// Is `this' correctly set ?
//---------------------------------------------------------------------------
bool vcsl_scale::is_valid(void) const
{
  return (_scale!=0)&&(((_beat==0)&&(_interpolator==0)&&(_scale->size()==1))
                        ||
                        ((_beat!=0)&&(_interpolator!=0)
                         &&(_beat->size()==(_interpolator->size()+1))
                         &&(_beat->size()==_scale->size())));
}

//***************************************************************************
// Transformation parameters
//***************************************************************************

//---------------------------------------------------------------------------
// Set the scale value of a static scale
//---------------------------------------------------------------------------
void vcsl_scale::set_static(const double new_scale)
{
  if(_scale==0||_scale->size()!=1)
    _scale=new list_of_scalars(1);
  (*_scale)[0]=new_scale;
  _beat=0;
  _interpolator=0;
}

//---------------------------------------------------------------------------
// Set the scale variation along the time
//---------------------------------------------------------------------------
void vcsl_scale::set_scale(vcl_vector<double> &new_scale)
{
  if(_scale!=0&&_scale->size()==1)
    delete _scale;
  _scale=&new_scale;
}

//---------------------------------------------------------------------------
// Return the scale variation along the time
//---------------------------------------------------------------------------
vcl_vector<double> *vcsl_scale::scale(void) const
{
  return _scale;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// Image of `v' by `this'
// REQUIRE: is_valid()
//---------------------------------------------------------------------------
vnl_vector<double> *vcsl_scale::execute(const vnl_vector<double> &v,
                                        const double time) const
{
  // require
  assert(is_valid());

  vnl_vector<double> *result;
  double value;
  int i;
  
  value=scale_value(time);
  result=new vnl_vector<double>(v.size());
  for(i=0;i<v.size();++i)
    result->put(i,value*v.get(i));

  return result;
}

//---------------------------------------------------------------------------
// Image of `v' by the inverse of `this'
// REQUIRE: is_valid()
// REQUIRE: is_invertible(time)
//---------------------------------------------------------------------------
vnl_vector<double> *vcsl_scale::inverse(const vnl_vector<double> &v,
                                        const double time) const
{
  // require
  assert(is_valid());
  assert(is_invertible(time));

  vnl_vector<double> *result;
  double value;
  int i;

  value=scale_value(time);
  result=new vnl_vector<double>(v.size());
  for(i=0;i<v.size();++i)
    result->put(i,v.get(i)/value);

  return result; 
}

//---------------------------------------------------------------------------
// Compute the value of the parameter at time `time'
//---------------------------------------------------------------------------
double vcsl_scale::scale_value(const double time) const
{
  double result;
  int i;

  if(_beat==0) // static
    result=(*_scale)[0];
  else
    {
      i=matching_interval(time);
      switch((*_interpolator)[i])
        {
        case vcsl_linear:
          result=lsi((*_scale)[i],(*_scale)[i+1],i,time);
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
    }
  return result;
}
