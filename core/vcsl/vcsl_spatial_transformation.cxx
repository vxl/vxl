#include <vcsl/vcsl_spatial_transformation.h>
#include <vcl/vcl_cmath.h> // for acos(), sin()

#include <vcl/vcl_cassert.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_spatial_transformation::vcsl_spatial_transformation(void)
{
  beat_=0;
  interpolator_=0;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_spatial_transformation::~vcsl_spatial_transformation()
{
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Return the list of time clocks
//---------------------------------------------------------------------------
vcl_vector<double> *vcsl_spatial_transformation::beat(void) const
{
  return beat_;
}

//---------------------------------------------------------------------------
// Return the list of interpolators
//---------------------------------------------------------------------------
vcl_vector<vcsl_interpolator> *
vcsl_spatial_transformation::interpolators(void) const
{
  return interpolator_;
}

//---------------------------------------------------------------------------
// Is `time' between the two time bounds ?
//---------------------------------------------------------------------------
bool vcsl_spatial_transformation::valid_time(const double time) const
{
  return ((*beat_)[0]<=time)&&(time<=(*beat_)[beat_->size()-1]);
}

//---------------------------------------------------------------------------
// Is `this' correctly set ?
//---------------------------------------------------------------------------
bool vcsl_spatial_transformation::is_valid(void) const
{
  return ((beat_==0)&&(interpolator_==0))||
    ((beat_!=0)&&(interpolator_!=0)
     &&(beat_->size()==(interpolator_->size()+1)));
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// Return the index of the beat inferior or equal to `time'
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
int vcsl_spatial_transformation::matching_interval(const double time) const
{
  // require
  assert(valid_time(time));

  int result;
  int inf;
  int sup;
  int mid;

  // Dichotomic research of the index

  inf=0;
  sup=beat_->size()-1;
  while((sup-inf)!=1)
    {
      mid=(inf+sup)/2;
      if((*beat_)[mid]>time)
        sup=mid;
      else
        inf=mid;
    }
  result=inf;
  
  return result;
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
// Set the list of time clocks
//---------------------------------------------------------------------------
void vcsl_spatial_transformation::set_beat(vcl_vector<double> &new_beat)
{
  beat_=&new_beat;
}

//---------------------------------------------------------------------------
// Set the list of interpolators
//---------------------------------------------------------------------------
void
vcsl_spatial_transformation::set_interpolators(vcl_vector<vcsl_interpolator> &new_interpolators)
{
  interpolator_=&new_interpolators;
}

//***************************************************************************
// Interpolators
//***************************************************************************

//---------------------------------------------------------------------------
// Linear interpolation on scalar values
//---------------------------------------------------------------------------
double vcsl_spatial_transformation::lsi(const double v0,
                                const double v1,
                                const int index,
                                const double time) const
{
  double result;

  double t0;
  double t1;

  t0=(*beat_)[index];
  t1=(*beat_)[index+1];

  result=(v0*(t1-time)+v1*(time-t0))/(t1-t0);

  return result;
}

//---------------------------------------------------------------------------
// Linear interpolation on vnl_vectors
//---------------------------------------------------------------------------
vnl_vector<double> *
vcsl_spatial_transformation::lvi(const vnl_vector<double> &v0,
                                 const vnl_vector<double> &v1,
                                 const int index,
                                 const double time) const
{
  vnl_vector<double> *result;
  int i;
  int size;

  double t0;
  double t1;
  double dt0;
  double dt1;
  double denominator;

  size=v0.size();
  t0=(*beat_)[index];
  t1=(*beat_)[index+1];

  result=new vnl_vector<double>(size);

  denominator=1/(t1-t0);
  dt1=(t1-time)*denominator;
  dt0=(time-t0)*denominator;

  for(i=0;i<size;++i)
    result->put(i,v0.get(i)*dt1+v1.get(i)*dt0);

  return result;
}

//---------------------------------------------------------------------------
// Linear interpolation on quaternions
//---------------------------------------------------------------------------
vnl_quaternion<double> *
vcsl_spatial_transformation::lqi(const vnl_quaternion<double> &v0,
                                 const vnl_quaternion<double> &v1,
                                 const int index,
                                 const double time) const
{
  vnl_quaternion<double> *result;
  double t0;
  double t1;
  double t;
  double cosangle;
  double angle;
  double invsin;
  double coef1;
  double coef2;

  t0=(*beat_)[index];
  t1=(*beat_)[index+1];
  t=(time-t0)/(t1-t0);

  { // sunpro 5.0 is overload challenged
    vnl_vector<double> const &v0_ = v0;
    vnl_vector<double> const &v1_ = v1;
    cosangle=dot_product(v0_, v1_);
  }
  angle=acos(cosangle);
  invsin=1/sin(angle);
  coef1=sin((1-t)*angle)*invsin;
  coef2=sin(t*angle)*invsin;

  result=new vnl_quaternion<double>(v0.x()*coef1+v1.x()*coef2,
                                    v0.y()*coef1+v1.y()*coef2,
                                    v0.z()*coef1+v1.z()*coef2,
                                    v0.r()*coef1+v1.r()*coef2);

  return result;
}
