// This is brl/bseg/bapl/bapl_keypoint_set_sptr.h
#ifndef bapl_keypoint_set_sptr_h_
#define bapl_keypoint_set_sptr_h_
//:
// \file
// \brief Smart pointer to a keypoint_set
// \author Ozge C. Ozcanli, (ozge@lems.brown.edu)
// \date Sep 16, 2010
//

class bapl_keypoint_set;
class bapl_keypoint_match_set;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<bapl_keypoint_set> bapl_keypoint_set_sptr;
typedef vbl_smart_ptr<bapl_keypoint_match_set> bapl_keypoint_match_set_sptr;

#endif // bapl_keypoint_set_sptr_h_
