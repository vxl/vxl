// This is core/vcsl/vcsl_geographic.cxx
#include "vcsl_geographic.h"
#include <vcsl/vcsl_spheroid.h>

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_geographic::vcsl_geographic()
  : spheroid_(new vcsl_spheroid)
{
}
