// This is brl/bseg/brec/brec_part_base_sptr.h
#ifndef brec_part_base_sptr_h_
#define brec_part_base_sptr_h_
//:
// \file
// \brief Smart pointer to a brec_part_base
// \author Ozge Can Ozcanli, (ozge@lems.brown.edu)
// \date October 16, 2008

#include <vbl/vbl_smart_ptr.h>

class brec_part_base;
class brec_part_instance;

typedef vbl_smart_ptr<brec_part_base> brec_part_base_sptr;
typedef vbl_smart_ptr<brec_part_instance> brec_part_instance_sptr;

#endif // brec_part_base_sptr_h_
