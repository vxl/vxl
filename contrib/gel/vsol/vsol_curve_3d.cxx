#include "vsol_curve_3d.h"
#include "vsol_point_3d.h"

//------------------------------------------------------------------------
// Helper function to determine if curve endpoints are equal (in any order).
// Useful for curve equality tests.
//
bool vsol_curve_3d::endpoints_equal(const vsol_curve_3d &other) const
{
  vsol_point_3d_sptr p01 = this->p0(), p11 = this->p1();
  vsol_point_3d_sptr p02 = other.p0(), p12 = other.p1();
  bool c1_has_endpoints = p01&&p11;
  bool c2_has_endpoints = p02&&p12;
  if ((c1_has_endpoints&&!c2_has_endpoints)
     ||(!c1_has_endpoints&&c2_has_endpoints))
    return false;
  else if (c1_has_endpoints&&c2_has_endpoints)
    return *p01 ==*p02 && *p11==*p12;
  else if (!p01&&!p11&&!p02&&!p12) // no endpoints at all
    return true;
  else if (p01&&!p11&&p02&&!p12)
    return *p01==*p02;
  else if (p01&&!p11&&!p02&&p12)
    return *p01==*p12;
  else if (!p01&&p11&&p02&&!p12)
    return *p11==*p02;
  else if (!p01&&p11&&!p02&&p12)
    return *p11==*p12;
  else
    return false;
}
