// This is vxl/vcsl/vcsl_lambertian.cxx
#include "vcsl_lambertian.h"

#include <vcsl/vcsl_axis.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_lambertian::vcsl_lambertian(void)
{
  vcsl_axis_sptr a;
  a=new vcsl_axis;
  axes_.push_back(a);
  a=new vcsl_axis(*(a.ptr()));
  axes_.push_back(a);
  a=new vcsl_axis(*(a.ptr()));
  axes_.push_back(a);
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_lambertian::~vcsl_lambertian()
{
}
