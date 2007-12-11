// This is brl/bbas/bmsh3d/bmsh3d_fuzzy_boolean.h
#ifndef _bmsh3d_fuzzy_boolean_h_
#define _bmsh3d_fuzzy_boolean_h_
//:
// \file
// \brief  bmsh3d fuzzy boolean functions.
// \author Ming-Ching Chang (mcchang@lems.brown.edu)
// \date   May/07/2007
//
// \verbatim
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vcl_cmath.h>

#define BMSH3D_FINE_EPSILON     5E-15
#define BMSH3D_MID_EPSILON      5E-10
#define BMSH3D_COARSE_EPSILON   5E-5

//: the mid-epsilon equality test, use absolute test.
inline bool bmsh3d_eq (const double& a, const double& b, const double& epsilon)
{
  return vcl_fabs(a-b) < epsilon;
}

//: the mid-epsilon equality test, use absolute test.
inline bool bmsh3d_eq_m (const double& a, const double& b)
{
  return vcl_fabs(a-b) < BMSH3D_MID_EPSILON;
}

inline bool bmsh3d_leq_m (const double& a, const double& b)
{
  return a < b || bmsh3d_eq_m (a, b);
}

//: the coarse-epsilon equality test, use absolute test.
inline bool bmsh3d_eq_c (const double& a, const double& b)
{
  return vcl_fabs(a-b) < BMSH3D_COARSE_EPSILON;
}

#endif
