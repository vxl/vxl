#include <vcsl/vcsl_lambertian.h>

#include <vcsl/vcsl_axis.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_lambertian::vcsl_lambertian(void)
{
  vcsl_axis_ref a;
  a=new vcsl_axis;
  _axes.push_back(a);
  a=new vcsl_axis(*(a.ptr()));
  _axes.push_back(a);
  a=new vcsl_axis(*(a.ptr()));
  _axes.push_back(a);
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_lambertian::~vcsl_lambertian()
{
}
