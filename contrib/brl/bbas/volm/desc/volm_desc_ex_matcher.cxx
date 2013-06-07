#include "volm_desc_ex_matcher.h"
//:
// \file

float volm_desc_ex_matcher::score(volm_desc_sptr const& a, volm_desc_sptr const& b)
{
  // currently, the comparision is accomplished by checking the intersection of two histgrom
  // this comparison method can be extend in future
  return a->similarity(b);
}

volm_desc_sptr volm_desc_ex_matcher::create_query_desc()
{
  volm_desc_sptr query_ex = new volm_desc_ex(dms_, radius_, norients_, nlands_, initial_mag_);
  return query_ex;
}

bool volm_desc_ex_matcher::check_threshold(volm_desc_sptr const& query, float& thres_value)
{
  unsigned num_valid_bin = query->get_area();
  if (thres_value < 1.0f/num_valid_bin)
    thres_value = 1.0f/num_valid_bin;
  return true;
}
#if 0
bool volm_desc_ex_matcher::write_out(vcl_string const& out_folder, unsigned const& tile_id)
{
#if 0
  vcl_stringstream file_name;
  file_name << out_folder << "/" << name_ << "RUN/visibility_score_tile_" << tile_id << ".bin";
  volm_score::write_scores(score_all_, file_name.str());
#endif
  return true;
}
#endif
