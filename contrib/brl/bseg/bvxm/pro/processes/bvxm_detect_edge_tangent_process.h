// This is brl/bseg/bvxm/pro/processes/bvxm_detect_edge_tangent_process.h
#ifndef bvxm_detect_edge_tangent_process_h_
#define bvxm_detect_edge_tangent_process_h_
//:
// \file
// \brief A process that detects sub-pixel edges and edge tangent directions
//
// \author J.L. Mundy
// \date September 19, 2009
// \verbatim
//  Modifications
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <vcl_string.h>

//: global variables
namespace bvxm_detect_edge_tangent_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_=1;

  // parameter strings
  const vcl_string param_noise_multiplier_ =  "noise_multiplier";
  const vcl_string param_smooth_ =  "smooth";
  const vcl_string param_automatic_threshold_ =  "automatic_threshold";
  const vcl_string param_junctionp_ =  "junctionp";
  const vcl_string param_aggressive_junction_closure_ =  "aggressive_junction_closure";
}

//: initialize input and output types
bool bvxm_detect_edge_tangent_process_cons(bprb_func_process& pro);

//: generates the edge map
bool bvxm_detect_edge_tangent_process(bprb_func_process& pro);

#endif // bvxm_detect_edge_tangent_process_h_
