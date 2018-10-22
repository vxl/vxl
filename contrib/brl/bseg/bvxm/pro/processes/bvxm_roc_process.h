// This is brl/bseg/bvxm/pro/processes/bvxm_roc_process.h
#ifndef bvxm_roc_process_h_
#define bvxm_roc_process_h_
//:
// \file
// \brief A class for obtaining roc curve from change detection results.
//
// \author Isabel Restrepo
// \date April 19, 2008
// \verbatim
//  Modifications
//   Brandon Mayer - Jan 28, 2009 - converted process-class to function to conform with new bvxm_process architecture.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: global variables
namespace bvxm_roc_process_globals
{
  constexpr unsigned int n_inputs_ = 3;
  constexpr unsigned int n_outputs_ = 0;
}

bool bvxm_roc_process_cons(bprb_func_process& pro);

bool bvxm_roc_process(bprb_func_process& pro);

#endif // bvxm_roc_process_h_
