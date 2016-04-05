#include "boxm2_volm_desc_ex_land_only_matcher.h"
//
// \file

float boxm2_volm_desc_ex_land_only_matcher::score(volm_desc_sptr const& a, volm_desc_sptr const& b)
{
  return a->similarity(b);
}

volm_desc_sptr boxm2_volm_desc_ex_land_only_matcher::create_query_desc()
{
  volm_desc_sptr query_ex = new volm_desc_ex_land_only(dms_, radius_, nlands_, initial_mag_);
  return query_ex;
}

bool boxm2_volm_desc_ex_land_only_matcher::check_threshold(volm_desc_sptr const& query, float& thres_value)
{
  unsigned num_valid_bin = query->get_area();
  if (thres_value < 1.0f/num_valid_bin)
    thres_value = 1.0f/num_valid_bin;
  return true;
}
