// This is brl/bseg/bvxm/pro/processes/ bvxm_detect_scale_process.h
#ifndef bvxm_detect_scale_process_h_
#define bvxm_detect_scale_process_h_
//:
// \file
//
//  CAUTION: Input image is assumed to have type vxl_byte
//
// \author Vishal Jain
// \date April 15, 2008
//
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: globals
namespace bvxm_detect_scale_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}

//: set input and output types
bool bvxm_detect_scale_process_cons(bprb_func_process& pro);

//: detects the scale of a voxel world
bool bvxm_detect_scale_process(bprb_func_process& pro);

#endif // bvxm_detect_scale_process_h_
