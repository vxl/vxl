// This is brl/bseg/bvxm/pro/processes/bvxm_display_edge_tangent_world_vrml_process.h
#ifndef bvxm_display_edge_tangent_world_vrml_process_h_
#define bvxm_display_edge_tangent_world_vrml_process_h_
//:
// \file
// \brief A process that displays the edge tangent world as a vrml file
//
// \author J.L. Mundy
// \date Sept. 21, 2009
// \verbatim
//  Modifications
// \endverbatim

#include <bprb/bprb_func_process.h>

//: globals
namespace bvxm_display_edge_tangent_world_vrml_process_globals
{
  constexpr unsigned int n_inputs_ = 3;
  constexpr unsigned int n_outputs_ = 0;
}

//: set input and output types
bool bvxm_display_edge_tangent_world_vrml_process_cons(bprb_func_process& pro);

//: renders the expected image
bool bvxm_display_edge_tangent_world_vrml_process(bprb_func_process& pro);

#endif // bvxm_display_edge_tangent_world_vrml_process_h_
