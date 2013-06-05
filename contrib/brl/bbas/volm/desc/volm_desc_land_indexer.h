//:
// \file
// \brief  A class to create a descriptor index to store land type of each location
//
// \author
// \date May 29, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#if !defined(_VOLM_DESC_LAND_INDEXER_H)
#define _VOLM_DESC_LAND_INDEXER_H

#include "volm_desc_indexer.h"
#include "volm_desc_land.h"

#include <volm/volm_io_tools.h>

class volm_desc_land_indexer : public volm_desc_indexer
{
public:
  static vcl_string name_; 

  volm_desc_land_indexer(vcl_string const& NLCD_folder, vcl_string const& out_index_folder);

  virtual bool extract(double lat, double lon, double elev, vcl_vector<unsigned char>& values);

  //: each driving indexer should overwrite with the size of the descriptor
  virtual unsigned layer_size() { return volm_desc_land::n_bins; }

  virtual vcl_string get_index_type_str() { return volm_desc_land_indexer::name_; }

public:
  vcl_vector<volm_img_info> NLCD_imgs_;

};

#endif  //_VOLM_DESC_LAND_INDEXER_H
