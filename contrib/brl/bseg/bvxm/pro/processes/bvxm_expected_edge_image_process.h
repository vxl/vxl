// This is brl/bseg/bvxm/pro/processes/bvxm_expected_edge_image_process.h
#ifndef bvxm_expected_edge_image_process_h_
#define bvxm_expected_edge_image_process_h_
//:
// \file
// \brief A process that generates an expected edge image from an edge voxel world
//
// \author J. L. Mundy
// \date October 11, 2009
// \verbatim
//  Modifications
// \endverbatim

#include <bprb/bprb_func_process.h>

//: globals
namespace bvxm_expected_edge_image_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 2;
}

//: set input and output types
bool bvxm_expected_edge_image_process_cons(bprb_func_process& pro);

//:  optimizes rpc camera parameters based on edges
bool bvxm_expected_edge_image_process(bprb_func_process& pro);

#endif // bvxm_expected_edge_image_process_h_
