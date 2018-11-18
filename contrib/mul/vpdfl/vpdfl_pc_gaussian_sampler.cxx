// This is mul/vpdfl/vpdfl_pc_gaussian_sampler.cxx
//:
// \file
// \author Ian Scott
// \date 21-Jul-2000
// \brief Interface for Multi-variate principal Component gaussian PDF sampler.
// Modifications
// 23 April 2001 IMS - Ported to VXL

#include <iostream>
#include <string>
#include "vpdfl_pc_gaussian_sampler.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================
// Method: is_a
//=======================================================================
std::string vpdfl_pc_gaussian_sampler::is_a() const
{
  return std::string("vpdfl_pc_gaussian_sampler");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_pc_gaussian_sampler::is_class(std::string const& s) const
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
