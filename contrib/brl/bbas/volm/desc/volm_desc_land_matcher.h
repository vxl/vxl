//:
// \file
// \brief  A class to represent 
//
// \author
// \date May 29, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#if !defined(_VOLM_DESC_LAND_MATCHER_H)
#define _VOLM_DESC_LAND_MATCHER_H

#include "volm_desc_matcher.h"
#include "volm_desc_land_indexer.h"

class volm_desc_land_matcher : public volm_desc_matcher
{

  virtual vcl_string get_index_type_str() { return volm_desc_land_indexer::name_; }
};

#endif  //_VOLM_DESC_LAND_MATCHER_H
