// This is core/vcsl/vcsl_spatial_transformation.cxx
#include "vcsl_spatial_transformation.h"
#include <vcl_cmath.h> // for acos(), sin()
#include <vcl_cassert.h>

//---------------------------------------------------------------------------
// Is `time' between the two time bounds ?
//---------------------------------------------------------------------------
bool vcsl_spatial_transformation::valid_time(double time) const
{
  if (beat_.size() == 0) return true;
  return (beat_[0]<=time)&&(time<=beat_[beat_.size()-1]);
}

//---------------------------------------------------------------------------
// Return the index of the beat inferior or equal to `time'
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
int vcsl_spatial_transformation::matching_interval(double time) const
{
  // require
  assert(valid_time(time));

  // Dichotomic research of the index
  int inf=0;
  int sup=beat_.size()-1;
  while (sup-inf > 1)
  {
    int mid=(inf+sup)/2;
    if (beat_[mid]>time)
      sup=mid;
    else
      inf=mid;
  }
  return inf;
}

//---------------------------------------------------------------------------
// Empty the time clock and interpolators, thereby making the transf static
//---------------------------------------------------------------------------
void vcsl_spatial_transformation::set_static()
{
  beat_.clear();
  interpolator_.clear();
}

//---------------------------------------------------------------------------
// Linear interpolation on scalar values
//---------------------------------------------------------------------------
double vcsl_spatial_transformation::lsi(double v0,
                                        double v1,
                                        int index,
                                        double time) const
{
  assert(index>=0 && (unsigned)index+1<beat_.size());
  double t0=beat_[index];
  double t1=beat_[index+1];

  return (v0*(t1-time)+v1*(time-t0))/(t1-t0);
}

//---------------------------------------------------------------------------
// Linear interpolation on vnl_vectors
//---------------------------------------------------------------------------
vnl_vector<double>
vcsl_spatial_transformation::lvi(const vnl_vector<double> &v0,
                                 const vnl_vector<double> &v1,
                                 int index,
                                 double time) const
{
  int size=v0.size();
  assert(index>=0 && (unsigned)index+1<beat_.size());
  double t0=beat_[index];
  double t1=beat_[index+1];

  double denominator=1/(t1-t0);
  double dt1=(t1-time)*denominator;
  double dt0=(time-t0)*denominator;

  vnl_vector<double> result(size);
  for (int i=0;i<size;++i)
    result.put(i,v0.get(i)*dt1+v1.get(i)*dt0);

  return result;
}

//---------------------------------------------------------------------------
// Linear interpolation on vnl_matrices
//---------------------------------------------------------------------------
vnl_matrix<double>
vcsl_spatial_transformation::lmi(const vnl_matrix<double> &m0,
                                 const vnl_matrix<double> &m1,
                                 int index,
                                 double time) const
{
  int rows=m0.rows();
  int cols=m0.cols();
  assert(index>=0 && (unsigned)index+1<beat_.size());
  double t0=beat_[index];
  double t1=beat_[index+1];

  double denominator=1/(t1-t0);
  double dt1=(t1-time)*denominator;
  double dt0=(time-t0)*denominator;

  vnl_matrix<double> result(rows,cols);
  for (int i=0;i<rows;++i)
  for (int j=0;j<cols;++j)
    result.put(i,j,m0.get(i,j)*dt1+m1.get(i,j)*dt0);

  return result;
}

//---------------------------------------------------------------------------
// Linear interpolation on quaternions
//---------------------------------------------------------------------------
vnl_quaternion<double>
vcsl_spatial_transformation::lqi(const vnl_quaternion<double> &v0,
                                 const vnl_quaternion<double> &v1,
                                 int index,
                                 double time) const
{
  assert(index>=0 && (unsigned)index+1<beat_.size());
  double t0=beat_[index];
  double t1=beat_[index+1];
  double t=(time-t0)/(t1-t0);

  double cosangle=dot_product(v0.as_ref(), v1.as_ref());
  double angle=vcl_acos(cosangle);
  double invsin=1/vcl_sin(angle);
  double coef1=vcl_sin((1-t)*angle)*invsin;
  double coef2=vcl_sin(t*angle)*invsin;

  return vnl_quaternion<double>(v0.x()*coef1+v1.x()*coef2,
                                v0.y()*coef1+v1.y()*coef2,
                                v0.z()*coef1+v1.z()*coef2,
                                v0.r()*coef1+v1.r()*coef2);
}
