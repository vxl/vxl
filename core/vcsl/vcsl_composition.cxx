// This is core/vcsl/vcsl_composition.cxx
#include "vcsl_composition.h"
#include <vcl_cassert.h>

//---------------------------------------------------------------------------
// Is `this' invertible at time `time'?
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
bool vcsl_composition::is_invertible(double time) const
{
  // require
  assert(valid_time(time));

  vcl_vector<vcsl_spatial_transformation_sptr>::const_iterator i;

  bool result=true;
  for (i=transformations_.begin();result&&i!=transformations_.end();++i)
    result=(*i)->is_invertible(time);

  return result;
}

//---------------------------------------------------------------------------
// Is `this' correctly set ?
//---------------------------------------------------------------------------
bool vcsl_composition::is_valid() const
{
  vcl_vector<vcsl_spatial_transformation_sptr>::const_iterator i;
  for (i=transformations_.begin(); i!=transformations_.end(); ++i)
    if (!(*i)->is_valid()) return false;

  return true;
}

//---------------------------------------------------------------------------
// Image of `v' by `this'
// REQUIRE: is_valid()
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_composition::execute(const vnl_vector<double> &v,
                                             double time) const
{
  // require
  assert(is_valid());

  vnl_vector<double> result = v;

  vcl_vector<vcsl_spatial_transformation_sptr>::const_iterator i;
  for (i=transformations_.begin();i!=transformations_.end();++i)
    result=(*i)->execute(result,time);
  return result;
}

//---------------------------------------------------------------------------
// Image of `v' by the inverse of `this'
// REQUIRE: is_valid()
// REQUIRE: is_invertible(time)
//---------------------------------------------------------------------------
vnl_vector<double> vcsl_composition::inverse(const vnl_vector<double> &v,
                                             double time) const
{
  // require
  assert(is_valid());
  assert(is_invertible(time));

  vnl_vector<double> result = v;

  vcl_vector<vcsl_spatial_transformation_sptr>::const_reverse_iterator i;
  for (i=transformations_.rbegin();!(i==transformations_.rend());++i)
    result=(*i)->inverse(result,time);
  return result;
}

