#include <vcsl/vcsl_coordinate_system.h>

#include <vcl/vcl_cassert.h>

#include <vcsl/vcsl_axis.h>
#include <vcsl/vcsl_unit.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_coordinate_system::vcsl_coordinate_system(void)
{

}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_coordinate_system::~vcsl_coordinate_system()
{
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Number of axes
//---------------------------------------------------------------------------
int vcsl_coordinate_system::dimensionnality(void) const
{
  return _axes.size();
}

//---------------------------------------------------------------------------
// Is `i' an index on an axis ?
//---------------------------------------------------------------------------
bool vcsl_coordinate_system::valid_axis(const int i) const
{
  return (i>=0)&&(i<_axes.size());
}

//---------------------------------------------------------------------------
// Return the axis `i'
// REQUIRE: valid_axis(i)
//---------------------------------------------------------------------------
vcsl_axis_ref vcsl_coordinate_system::axis(const int i) const
{
  // require
  assert(valid_axis(i));

  return _axes[i];
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
vnl_vector<double> *
vcsl_coordinate_system::from_cs_to_standard_units(const vnl_vector<double> &v) const
{
  vnl_vector<double> *result;
  vcl_vector<vcsl_axis_ref>::const_iterator i;
  int j;
  
  result=new vnl_vector<double>(v.size());

  j=0;
  for(i=_axes.begin();i!=_axes.end();++i)
    {
      result->put(j,v.get(j)/(*i)->unit()->units_per_standard_unit());
      ++j;
    }

  return result;
}

//---------------------------------------------------------------------------
// Convert `v', exprimed with standard units, to cs units
// REQUIRE: v.size()==dimensionnality()
//---------------------------------------------------------------------------
vnl_vector<double> *
vcsl_coordinate_system::from_standard_units_to_cs(const vnl_vector<double> &v) const
{
  vnl_vector<double> *result;
  vcl_vector<vcsl_axis_ref>::const_iterator i;
  int j;

  result=new vnl_vector<double>(v.size());
  j=0;
  for(i=_axes.begin();i!=_axes.end();++i)
    {
      result->put(j,v.get(j)*(*i)->unit()->units_per_standard_unit());
      ++j;
    }

  return result;
}
