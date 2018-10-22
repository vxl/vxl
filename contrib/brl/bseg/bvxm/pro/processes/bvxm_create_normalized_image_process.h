// This is brl/bseg/bvxm/pro/processes/bvxm_create_normalized_image_process.h
#ifndef bvxm_create_normalized_image_process_h_
#define bvxm_create_normalized_image_process_h_
//:
// \file
// \brief A class to create a contrast normalized image using the input gain and offset values
//
// \author Ozge Can Ozcanli
// \date February 17, 2008
//
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: global variables
namespace bvxm_create_normalized_image_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}

//: sets input and output types for bvxm_create_normalized_image_process
bool bvxm_create_normalized_image_process_cons(bprb_func_process& pro);

//: create a normalize image
bool bvxm_create_normalized_image_process(bprb_func_process& pro);

#endif // bvxm_create_normalized_image_process_h_
