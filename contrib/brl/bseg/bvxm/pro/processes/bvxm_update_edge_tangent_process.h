// This is brl/bseg/bvxm/pro/processes/bvxm_update_edge_tangent_process.h
#ifndef bvxm_update_edge_tangent_process_h_
#define bvxm_update_edge_tangent_process_h_
//:
// \file
// \brief A process that updates 3-d edge tangent and sub-voxel position
//
// \author J.L. Mundy
// \date September 19, 2009
// \verbatim
//  Modifications
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <vcl_string.h>

//: globals
namespace bvxm_update_edge_tangent_process_globals
{
  const unsigned int n_inputs_ = 5;
  const unsigned int n_outputs_ = 0;
}

//: set input and output types
bool bvxm_update_edge_tangent_process_cons(bprb_func_process& pro);

//:  Initializes edge tangent world with two edge tangent images
bool bvxm_update_edge_tangent_process_init(bprb_func_process& pro);

//:  Updates edge tangent world from single edge tangent images
bool bvxm_update_edge_tangent_process(bprb_func_process& pro);

#endif // bvxm_update_edge_tangent_process_h_
