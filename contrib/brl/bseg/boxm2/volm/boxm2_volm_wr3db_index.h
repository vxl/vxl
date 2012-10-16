//This is brl/bseg/boxm2/volm/boxm2_volm_wr3db_index.h
#ifndef boxm2_volm_wr3db_index_h_
#define boxm2_volm_wr3db_index_h_
//:
// \file
// \brief  A class to index a given voxel world for fast matching to volumetric queries
//
// \author Ozge C. Ozcanli 
// \date October 07, 2012
// \verbatim
//   Modifications
//
// \endverbatim
// 

#include <vbl/vbl_array_1d.h>

struct index_value {
  char vis_;   // the highest bit is the visibility, visible or not, other bits will be used to code orientation if needed
  char prob_;
};

class boxm2_volm_wr3db_index
{
  public:
   vbl_array_1d<index_value> values_;
};

#endif  // boxm2_volm_wr3db_index_h_
