#include "vsol_curve_2d.h"
#include "vsol_point_2d.h"
//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_curve_2d::~vsol_curve_2d()
{
}
//------------------------------------------------------------------------
// Helper function to determine if curve endpoints are equal. Useful for
// curve equality tests
//
bool vsol_curve_2d::endpoints_equal(const vsol_curve_2d &other) const
{
  vsol_point_2d_sptr p01 = this->p0(), p11 = this->p1();
  vsol_point_2d_sptr p02 = other.p0(), p12 = other.p1();
  bool c1_has_endpoints = p01&&p11;
  bool c2_has_endpoints = p02&&p12;
  if((c1_has_endpoints&&!c2_has_endpoints)
     ||(!c1_has_endpoints&&c2_has_endpoints))
    return false;
  bool endpoints_eq = true;
  if(c1_has_endpoints&&c2_has_endpoints)
    {
      endpoints_eq = *p01 ==*p02;
      endpoints_eq = endpoints_eq && *p11==*p12;
    }      
  return endpoints_eq;
}
