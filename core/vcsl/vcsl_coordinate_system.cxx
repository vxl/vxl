#ifdef __GNUC__
#pragma implementation
#endif
#include <vcsl/vcsl_coordinate_system.h>

#include <vcl_cassert.h>

#include <vcsl/vcsl_axis.h>
#include <vcsl/vcsl_unit.h>

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Number of axes
//---------------------------------------------------------------------------
int vcsl_coordinate_system::dimensionnality(void) const
{
  return axes_.size();
}

//---------------------------------------------------------------------------
// Is `i' an index on an axis ?
//---------------------------------------------------------------------------
bool vcsl_coordinate_system::valid_axis(int i) const
{
  return (i>=0)&&(i<int(axes_.size()));
}

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

//***************************************************************************
// Because VXL does not use dynamic_cast<> :-(
//***************************************************************************

const vcsl_spatial *vcsl_coordinate_system::cast_to_spatial(void) const
{
  return 0;
}

//***************************************************************************
// Conversion
//***************************************************************************

//---------------------------------------------------------------------------
// Convert `v', exprimed with cs units, to standard units
// REQUIRE: v.size()==dimensionnality()
//---------------------------------------------------------------------------
vnl_vector<double>
vcsl_coordinate_system::from_cs_to_standard_units(const vnl_vector<double> &v) const
{
  vnl_vector<double> result(v.size());

  int j=0;
  vcl_vector<vcsl_axis_sptr>::const_iterator i;
  for(i=axes_.begin();i!=axes_.end();++i,++j)
    result.put(j,v.get(j)/(*i)->unit()->units_per_standard_unit());

  return result;
}

//---------------------------------------------------------------------------
// Convert `v', exprimed with standard units, to cs units
// REQUIRE: v.size()==dimensionnality()
//---------------------------------------------------------------------------
vnl_vector<double>
vcsl_coordinate_system::from_standard_units_to_cs(const vnl_vector<double> &v) const
{
  vnl_vector<double> result(v.size());

  int j=0;
  vcl_vector<vcsl_axis_sptr>::const_iterator i;
  for(i=axes_.begin();i!=axes_.end();++i,++j)
    result.put(j,v.get(j)*(*i)->unit()->units_per_standard_unit());

  return result;
}
