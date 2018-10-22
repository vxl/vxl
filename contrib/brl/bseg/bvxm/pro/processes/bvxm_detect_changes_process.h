// This is brl/bseg/bvxm/pro/processes/bvxm_detect_changes_process.h
#ifndef bvxm_detect_changes_process_h_
#define bvxm_detect_changes_process_h_
//:
// \file
// \brief A class for detecting changes using a voxel world .
//
// \author Daniel Crispell
// \date February 10, 2008
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: global variables
namespace bvxm_detect_changes_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 3;
}

//: initialize input and output types
bool bvxm_detect_changes_process_cons(bprb_func_process& pro);

//: detec changes
bool bvxm_detect_changes_process(bprb_func_process& pro);

#endif // bvxm_detect_changes_process_h_
