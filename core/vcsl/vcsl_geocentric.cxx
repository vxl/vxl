#include <vcsl/vcsl_geocentric.h>

#include <vcsl/vcsl_axis.h>
#include <vcsl/vcsl_angle.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor.
//---------------------------------------------------------------------------
vcsl_geocentric::vcsl_geocentric(void)
{
  vcsl_axis_ref a;
  a=new vcsl_axis;
  _axes.push_back(a);
  a=new vcsl_axis(*(vcsl_angle::instance().ptr()));
  _axes.push_back(a);
  a=new vcsl_axis(*(vcsl_angle::instance().ptr()));
  _axes.push_back(a);
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_geocentric::~vcsl_geocentric()
{
}
