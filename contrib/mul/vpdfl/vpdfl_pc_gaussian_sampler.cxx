// This is mul/vpdfl/vpdfl_pc_gaussian_sampler.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott
// \date 21-Jul-2000
// \brief Interface for Multi-variate principal Component gaussian PDF sampler.
// Modifications
// 23 April 2001 IMS - Ported to VXL

#include "vpdfl_pc_gaussian_sampler.h"

#include <vcl_string.h>

//=======================================================================
// Method: is_a
//=======================================================================
vcl_string vpdfl_pc_gaussian_sampler::is_a() const
{
  return vcl_string("vpdfl_pc_gaussian_sampler");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_pc_gaussian_sampler::is_class(vcl_string const& s) const
{
  return vpdfl_gaussian_sampler::is_class(s) || s==vpdfl_pc_gaussian_sampler::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short vpdfl_pc_gaussian_sampler::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

vpdfl_sampler_base* vpdfl_pc_gaussian_sampler::clone() const
{
  return new vpdfl_pc_gaussian_sampler(*this);
}

