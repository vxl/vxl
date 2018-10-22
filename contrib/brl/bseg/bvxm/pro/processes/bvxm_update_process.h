// This is brl/bseg/bvxm/pro/processes/bvxm_update_process.h
#ifndef bvxm_update_process_h_
#define bvxm_update_process_h_
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
//   Yi Dong - add option to use cache storage to speed up IO process during multi-thread execution
// \endverbatim

#include <bprb/bprb_func_process.h>

//: global variables
namespace bvxm_update_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 2;
}

bool bvxm_update_process_cons(bprb_func_process& pro);

bool bvxm_update_process(bprb_func_process& pro);

#endif // bvxm_update_process_h_
