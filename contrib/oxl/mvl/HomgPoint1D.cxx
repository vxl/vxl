// This is oxl/mvl/HomgPoint1D.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "HomgPoint1D.h"

bool HomgPoint1D::get_nonhomogeneous(double& out) const
{
  if (check_infinity())
    return false;

  out = get_x() / get_w();
  return true;
}

bool HomgPoint1D::check_infinity(double tol) const
{
  return get_w() < get_x()*tol;
}
