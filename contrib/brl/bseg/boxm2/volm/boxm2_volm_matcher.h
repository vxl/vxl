//This is brl/bseg/boxm2/volm/boxm2_volm_matcher.h
#ifndef boxm2_volm_matcher_h_
#define boxm2_volm_matcher_h_
//:
// \file
// \brief  A class to match a voxelized query volume to an indexed reference volume
//
// \author Yi Dong
// \date October 07, 2012
// \verbatim
//   Modifications
//       created by Ozge C. Ozcanli - Oct 07, 2012
// \endverbatim
// 

#include <volm/volm_query_sptr.h>
#include "boxm2_volm_wr3db_index.h"

class boxm2_volm_matcher
{
public:
  double matching_cost(volm_query_sptr q, boxm2_volm_wr3db_index ind);
};

#endif  // boxm2_volm_matcher_h_