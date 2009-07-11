// This is brl/bseg/bvxm/pro/processes/bvxm_compare_rpc_process.h
#ifndef bvxm_compare_rpc_process_h_
#define bvxm_compare_rpc_process_h_
//:
// \file
// \brief A process for comparing rational cameras.
//        -  Inputs:
//             * Rational Camera 1
//             * Rational Camera 2
//             * Path
//
//        -  Outputs:
//             * modified rational camera "vpgl_camera_double_sptr"
//             * clipped image area (NITF) "vil_image_view_base_sptr"
//
//        -  Params:
//             *geographic uncertainty (error) in meters
//
// \author  Ibrahim? -Please remove hard coded paths and complete input/output description
// \date    Feb 19, 2008
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: global variables
namespace bvxm_compare_rpc_process_globals
{
  // process takes 3 inputs and 0 outputs:
  unsigned n_inputs_ = 3;
  unsigned n_outputs_ = 0;
}

//: sets input and output types for  bvxm_compare_rpc_process
bool bvxm_compare_rpc_process_cons(bprb_func_process& pro);

//: Execute the process
bool bvxm_compare_rpc_process(bprb_func_process& pro);

#endif // bvxm_compare_rpc_process_h_
