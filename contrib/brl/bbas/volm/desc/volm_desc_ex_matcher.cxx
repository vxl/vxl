#include "volm_desc_ex_matcher.h"
//:
// \file
#include <vul/vul_file.h>

float volm_desc_ex_matcher::score(volm_desc_sptr const& a, volm_desc_sptr const& b)
{
  return a->similarity(b);
}

volm_desc_sptr volm_desc_ex_matcher::create_query_desc()
{
  volm_desc_sptr query_ex = new volm_desc_ex(dms_, radius_);
  return query_ex;
}


bool volm_desc_ex_matcher::write_out(vcl_string const& out_folder, unsigned const& tile_id)
{
  vcl_stringstream file_name;
  file_name << out_folder << "/" << name_ << "RUN/visibility_score_tile_" << tile_id << ".bin";
  volm_score::write_scores(score_all_, file_name.str());
  return true;
}
