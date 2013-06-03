//:
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

#if !defined(_VOLM_DESC_LAND_H)
#define _VOLM_DESC_LAND_H

#include "volm_desc.h"

class volm_desc_land : public volm_desc
{
public:
  // Constructor
  volm_desc_land(int land_type_id);

  // Constructor for the query
  volm_desc_land(vcl_string& filename);

  static unsigned n_bins;

};

#endif  //_VOLM_DESC_LAND_H
