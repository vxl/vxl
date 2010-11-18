// This is brl/bseg/bapl/bapl_lowe_keypoint_sptr.h
#ifndef bapl_lowe_keypoint_sptr_h_
#define bapl_lowe_keypoint_sptr_h_
//:
// \file
// \brief Smart pointer to a Lowe keypoint
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date Sat Nov 16 2003
//

class bapl_lowe_keypoint;

#include <bapl/bapl_keypoint_sptr.h>

typedef bapl_keypoint_sptr_t<bapl_lowe_keypoint> bapl_lowe_keypoint_sptr;

#endif // bapl_lowe_keypoint_sptr_h_
