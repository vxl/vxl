// This is brl/bseg/bvxm/pro/processes/bvxm_mog_l2_process.h
#ifndef bvxm_mog_l2_process_h_
#define bvxm_mog_l2_process_h_
//:
// \file
// \brief A process for computing the l2 distance between mixtures of gaussians in a grid.
//        At creation time the reference mixture is the first mixture encountered in the grid.
// \author Isabel Restrepo
// \date March 11, 2009
//
// \verbatim
//  Modifications
//   Peter Vanroose - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: set input and output types
bool bvxm_mog_l2_process_cons(bprb_func_process& pro);

//: Execute the process
bool bvxm_mog_l2_process(bprb_func_process& pro);

#endif // bvxm_mog_l2_process_h_
