// This is brl/bbas/bmsh3d/bmsh3d_fuzzy_boolean.h
#ifndef bmsh3d_fuzzy_boolean_h__
#define bmsh3d_fuzzy_boolean_h__
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

#include <iostream>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define BMSH3D_FINE_EPSILON     5E-15
#define BMSH3D_MID_EPSILON      5E-10
#define BMSH3D_COARSE_EPSILON   5E-5

//: the mid-epsilon equality test, use absolute test.
inline bool bmsh3d_eq (const double& a, const double& b, const double& epsilon)
{
  return std::fabs(a-b) < epsilon;
}

//: the mid-epsilon equality test, use absolute test.
inline bool bmsh3d_eq_m (const double& a, const double& b)
{
  return std::fabs(a-b) < BMSH3D_MID_EPSILON;
}

inline bool bmsh3d_leq_m (const double& a, const double& b)
{
  return a < b || bmsh3d_eq_m (a, b);
}

//: the coarse-epsilon equality test, use absolute test.
inline bool bmsh3d_eq_c (const double& a, const double& b)
{
  return std::fabs(a-b) < BMSH3D_COARSE_EPSILON;
}

#endif // bmsh3d_fuzzy_boolean_h__
