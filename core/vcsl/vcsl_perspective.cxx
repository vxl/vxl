#include <vcsl/vcsl_perspective.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_perspective::vcsl_perspective(void)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_perspective::~vcsl_perspective()
{
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Is `this' invertible at time `time'? Never !
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
bool vcsl_perspective::is_invertible(const double time) const
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
  return (focal_!=0)&&(((beat_==0)&&(interpolator_==0)&&(focal_->size()==1))
                        ||
                        ((beat_!=0)&&(interpolator_!=0)
                         &&(beat_->size()==(interpolator_->size()+1))
                         &&(beat_->size()==focal_->size())));
}

//***************************************************************************
// Transformation parameters
//***************************************************************************

//---------------------------------------------------------------------------
// Set the focal in meters of a static perspective projection
//---------------------------------------------------------------------------
void vcsl_perspective::set_static(const double new_focal)
{
  if(focal_==0||focal_->size()!=1)
    focal_=new list_of_scalars(1);
  (*focal_)[0]=new_focal;
  beat_=0;
  interpolator_=0;
}

//---------------------------------------------------------------------------
// Set the focal variation along the time in meters
//---------------------------------------------------------------------------
void vcsl_perspective::set_focal(list_of_scalars &new_focal)
{
   if(focal_!=0&&focal_->size()==1)
    delete focal_;
  focal_=&new_focal;
}

//---------------------------------------------------------------------------
// Return the focal variation along the time in meters
//---------------------------------------------------------------------------
list_of_scalars *vcsl_perspective::focal(void) const
{
  return focal_;
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
                                             const double time) const
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
                                             const double time) const
{
  // require
  assert(is_valid());
  assert((is_invertible(time))&&(v.size()==2));
  return vnl_vector<double>(); // To avoid compilation warning/error message
}

//---------------------------------------------------------------------------
// Compute the parameter at time `time'
//---------------------------------------------------------------------------
double vcsl_perspective::focal_value(const double time) const
{
  if(beat_==0) // static
    return (*focal_)[0];
  else
    {
      int i=matching_interval(time);
      switch((*interpolator_)[i])
        {
        case vcsl_linear:
          return lsi((*focal_)[i],(*focal_)[i+1],i,time);
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
  return 0.0; // never reached
}
