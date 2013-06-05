#include "volm_desc_land_matcher.h"
//:
// \file

float volm_desc_land_matcher::score(volm_desc_sptr const& query, volm_desc_sptr const& index)
{
  return query->similarity(index);
}

volm_desc_sptr volm_desc_land_matcher::create_query_desc()
{
  volm_desc_sptr query_land = new volm_desc_land(query_category_file_);
  return query_land;
}
