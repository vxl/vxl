#include <vcsl/vcsl_scale.h>

#include <vcl_cassert.h>

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

  return ((beat_==0)&&((*scale_)[0]!=0))||(scale_value(time)!=0);
}

//---------------------------------------------------------------------------
// Is `this' correctly set ?
//---------------------------------------------------------------------------
bool vcsl_scale::is_valid(void) const
{
  return (scale_!=0)&&(((beat_==0)&&(interpolator_==0)&&(scale_->size()==1))
                        ||
                        ((beat_!=0)&&(interpolator_!=0)
                         &&(beat_->size()==(interpolator_->size()+1))
                         &&(beat_->size()==scale_->size())));
}

//***************************************************************************
// Transformation parameters
//***************************************************************************

//---------------------------------------------------------------------------
// Set the scale value of a static scale
//---------------------------------------------------------------------------
void vcsl_scale::set_static(const double new_scale)
{
  if(scale_==0||scale_->size()!=1)
    scale_=new list_of_scalars(1);
  (*scale_)[0]=new_scale;
  beat_=0;
  interpolator_=0;
}

//---------------------------------------------------------------------------
// Set the scale variation along the time
//---------------------------------------------------------------------------
void vcsl_scale::set_scale(vcl_vector<double> &new_scale)
{
  if(scale_!=0&&scale_->size()==1)
    delete scale_;
  scale_=&new_scale;
}

//---------------------------------------------------------------------------
// Return the scale variation along the time
//---------------------------------------------------------------------------
vcl_vector<double> *vcsl_scale::scale(void) const
{
  return scale_;
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

  double value=scale_value(time);
  vnl_vector<double> *result=new vnl_vector<double>(v.size());
  for(unsigned int i=0;i<v.size();++i)
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

  double value=scale_value(time);
  vnl_vector<double> *result=new vnl_vector<double>(v.size());
  for(unsigned int i=0;i<v.size();++i)
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

  if(beat_==0) // static
    result=(*scale_)[0];
  else
    {
      i=matching_interval(time);
      switch((*interpolator_)[i])
        {
        case vcsl_linear:
          result=lsi((*scale_)[i],(*scale_)[i+1],i,time);
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
