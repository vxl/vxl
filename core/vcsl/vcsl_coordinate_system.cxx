// This is core/vcsl/vcsl_coordinate_system.cxx
#include "vcsl_coordinate_system.h"
#include <vcl_cassert.h>
#include <vcsl/vcsl_axis.h>
#include <vcsl/vcsl_unit.h>

//---------------------------------------------------------------------------
// Return the axis `i'
// REQUIRE: valid_axis(i)
//---------------------------------------------------------------------------
vcsl_axis_sptr vcsl_coordinate_system::axis(int i) const
{
  // require
  assert(valid_axis(i));

  return axes_[i];
}

//---------------------------------------------------------------------------
// Convert `v', expressed with cs units, to standard units
// REQUIRE: v.size()==dimensionality()
//---------------------------------------------------------------------------
vnl_vector<double>
vcsl_coordinate_system::from_cs_to_standard_units(const vnl_vector<double> &v) const
{
  vnl_vector<double> result(v.size());

  int j=0;
  vcl_vector<vcsl_axis_sptr>::const_iterator i;
  for (i=axes_.begin();i!=axes_.end();++i,++j)
    result.put(j,v.get(j)/(*i)->unit()->units_per_standard_unit());

  return result;
}

//---------------------------------------------------------------------------
// Convert `v', expressed with standard units, to cs units
// REQUIRE: v.size()==dimensionality()
//---------------------------------------------------------------------------
vnl_vector<double>
vcsl_coordinate_system::from_standard_units_to_cs(const vnl_vector<double> &v) const
{
  vnl_vector<double> result(v.size());

  int j=0;
  vcl_vector<vcsl_axis_sptr>::const_iterator i;
  for (i=axes_.begin();i!=axes_.end();++i,++j)
    result.put(j,v.get(j)*(*i)->unit()->units_per_standard_unit());

  return result;
}
