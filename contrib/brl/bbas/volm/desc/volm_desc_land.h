#pragma once
// This is brl/bbas/volm/desc/volm_desc_land.h
#ifndef volm_desc_land_h_
#define volm_desc_land_h_
// \file
// \brief  A class to represent land type of a location, the descriptor uses classes 1-14 of volm_label_table::land_id
//
// \author Ozge C. Ozcanli
// \date May 29, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include "volm_desc.h"

class volm_desc_land : public volm_desc
{
public:
  // Constructor
  volm_desc_land(int land_type_id, std::string const& id_type = "NLCD");

  // Constructor for the query
  volm_desc_land(std::string& filename);

  //: genera similarity metric for two land descriptor
  float similarity(volm_desc_sptr other) override;

  //static unsigned n_bins;

};

#endif  //_VOLM_DESC_LAND_H
