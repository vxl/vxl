// This is vxl/vcsl/vcsl_utm.cxx
#include "vcsl_utm.h"

#include <vcsl/vcsl_axis.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//--------------------------------------------------------------------------
// Default constructor
//--------------------------------------------------------------------------
vcsl_utm::vcsl_utm(void)
{
  vcsl_axis_sptr a;
  a=new vcsl_axis;
  axes_.push_back(a);
  a=new vcsl_axis;
  axes_.push_back(a);
  a=new vcsl_axis;
  axes_.push_back(a);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
vcsl_utm::~vcsl_utm()
{
}
