// This is brl/bseg/bvxm/rec/bvxm_part_base_sptr.h
#ifndef bvxm_part_base_sptr_h_
#define bvxm_part_base_sptr_h_
//:
// \file
// \brief Smart pointer to a bvxm_part_base
// \author Ozge Can Ozcanli, (ozge@lems.brown.edu)
// \date Oct 16, 2008
//

#include <vbl/vbl_smart_ptr.h>

class bvxm_part_base;
class bvxm_part_instance;

typedef vbl_smart_ptr<bvxm_part_base> bvxm_part_base_sptr;
typedef vbl_smart_ptr<bvxm_part_instance> bvxm_part_instance_sptr;

#endif // bvxm_part_base_sptr_h_
