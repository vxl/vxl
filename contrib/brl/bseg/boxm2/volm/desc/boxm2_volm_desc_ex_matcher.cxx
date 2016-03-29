#include "boxm2_volm_desc_ex_matcher.h"
//:
// \file

float boxm2_volm_desc_ex_matcher::score(volm_desc_sptr const& a, volm_desc_sptr const& b)
{
  // currently, the comparison is accomplished by checking the intersection of two histogram
  // this comparison method can be extend in future
  return a->similarity(b);
}

volm_desc_sptr boxm2_volm_desc_ex_matcher::create_query_desc()
{
  volm_desc_sptr query_ex = new volm_desc_ex(dms_, radius_, norients_, nlands_, initial_mag_);
  return query_ex;
}

bool boxm2_volm_desc_ex_matcher::check_threshold(volm_desc_sptr const& query, float& thres_value)
{
  unsigned num_valid_bin = query->get_area();
  if (thres_value < 1.0f/num_valid_bin)
    thres_value = 1.0f/num_valid_bin;
  return true;
}
