// This is brl/bseg/bvxm/pro/processes/bvxm_locate_region_process.h
#ifndef bvxm_locate_region_process_h_
#define bvxm_locate_region_process_h_
//:
// \file
// \brief A class for update process of a voxel world.
//
// \author Isabel Restrepo
// \date January 30, 2008
// \verbatim
//  Modifications
//   Brandon Mayer  - Jan 28, 2009 - converted process-class to function to conform with new bvxm_process architecture.
//   Peter Vanroose - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: global variables/functions
namespace bvxm_locate_region_process_globals
{
  constexpr unsigned int n_inputs_ = 6;
  constexpr unsigned int n_outputs_ = 0;
}

bool bvxm_locate_region_process_cons(bprb_func_process& pro);

bool bvxm_locate_region_process(bprb_func_process& pro);

#endif // bvxm_locate_region_process_h_
