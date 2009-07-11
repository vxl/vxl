// This is brl/bseg/bvxm/pro/processes/bvxm_merge_mog_process.h
#ifndef bvxm_merge_mog_process_h_
#define bvxm_merge_mog_process_h_
//:
// \file
// \brief A process for merging gaussian mixtures in a grid into a single gaussian.
//        This process works with bsta_mixtures of 3 modes
// \author Isabel Restrepo
// \date May 5, 2009
//
// \verbatim
//  Modifications
//   Peter Vanroose - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: set input and output types
bool bvxm_merge_mog_process_cons(bprb_func_process& pro);

//: Execute the process
bool bvxm_merge_mog_process(bprb_func_process& pro);

#endif // bvxm_merge_mog_process_h_
