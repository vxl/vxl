#include "boxm_apm_traits.h"

const char* boxm_apm_types::app_model_strings[]  = { "apm_mog_grey", "apm_mog_rgb", "apm_simple_grey", "apm_simple_rgb"};

boxm_apm_type boxm_apm_types::str_to_enum(const char* s)
{
  for (int i=0; i < int(boxm_apm_type::BOXM_APM_UNKNOWN); i++)
    if (vcl_strcmp(s, boxm_apm_types::app_model_strings[i]) == 0)
      return (boxm_apm_type) i;
  return boxm_apm_type::BOXM_APM_UNKNOWN;
}

