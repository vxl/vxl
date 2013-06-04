#include "volm_desc_ex_indexer.h"
//:
// \file
#include <vcl_algorithm.h>
#include <vul/vul_file.h>

volm_desc_ex_indexer::volm_desc_ex_indexer(vcl_string const& index_folder,
                                           vcl_string const& out_index_folder,
                                           vcl_vector<double> const& radius,
                                           unsigned const& norients,
                                           unsigned const& nlands,
                                           unsigned char const& initial_mag)
 : volm_desc_indexer(out_index_folder)
{
  index_folder_ = index_folder;
  norients_ = norients;
  nlands_ = nlands;
  out_index_folder_ = out_index_folder;
  if (radius.empty())
    radius_.push_back(1.0);
  else
    radius_ = radius;
  // sort the radius to ensure the bin order
  vcl_sort(radius_.begin(), radius_.end());
  unsigned ndists = (unsigned)radius.size() + 1;
  layer_size_ = ndists * norients_ * nlands_;
}

bool volm_desc_ex_indexer::load_tile_hypos(vcl_string const& geo_hypo_folder, int tile_id)
{
  
}

bool volm_desc_ex_indexer::extract(double lat, double lon, double elev, vcl_vector<unsigned char>& values)
{
  // obtain the index from current leaf
  return true;
}
