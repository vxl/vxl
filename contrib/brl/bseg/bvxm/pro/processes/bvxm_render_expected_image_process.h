// This is brl/bseg/bvxm/pro/processes/bvxm_render_expected_image_process.h
#ifndef bvxm_render_expected_image_process_h_
#define bvxm_render_expected_image_process_h_
//:
// \file
// \brief A process that renders a video frame from a new viewpoint (used for 3-D registration)
//
// \author Daniel Crispell
// \date Feb 10, 2008
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: globals
namespace bvxm_render_expected_image_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 2;
}

//: set input and output types
bool bvxm_render_expected_image_process_cons(bprb_func_process& pro);

//: renders the expected image
bool bvxm_render_expected_image_process(bprb_func_process& pro);

#endif // bvxm_render_expected_image_process_h_
