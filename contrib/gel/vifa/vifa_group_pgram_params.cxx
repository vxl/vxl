// This is gel/vifa/vifa_group_pgram_params.cxx
#include <iostream>
#include <sstream>
#include "vifa_group_pgram_params.h"
//:
// \file

#include <vcl_compiler.h>

vifa_group_pgram_params::
vifa_group_pgram_params(const vifa_group_pgram_params&  dp)
  : gevd_param_mixin(), vul_timestamp(), vbl_ref_count()
{
  init_params(dp.angle_increment_,
              dp.max_suppress_radius_,
              dp.length_ratio_);
}

bool vifa_group_pgram_params::
SanityCheck()
{
  std::stringstream  msg;
  bool        valid = true;

  if ((angle_increment_ <= 0) || (angle_increment_ > 45))
  {
    msg << "ERROR: Value of angle increment is out of range (0, 45]" << std::ends;
    valid = false;
  }

  if (max_suppress_radius_ <= 0)
  {
    msg << "ERROR: Maximum suppression radius must be greater than 0" << std::ends;
    valid = false;
  }

  if ((length_ratio_ <= 0) || (length_ratio_ > 1.0))
  {
    msg << "ERROR: length_ratio is out of range (0, 1.0]" << std::ends;
    valid = false;
  }

  SetErrorMsg(msg.str().c_str());
  return valid;
}

void vifa_group_pgram_params::
print_info(void)
{
  std::cout << "vifa_group_pgram_params:\n"
           << "  angle increment     = " << angle_increment_ << std::endl
           << "  max suppress radius = " << max_suppress_radius_ << std::endl
           << "  length ratio        = " << length_ratio_ << std::endl;
}
