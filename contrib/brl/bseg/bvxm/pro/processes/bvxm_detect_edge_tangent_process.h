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

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: global variables
namespace bvxm_detect_edge_tangent_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;

  // parameter strings
  const std::string param_noise_multiplier_ =  "noise_multiplier";
  const std::string param_smooth_ =  "smooth";
  const std::string param_automatic_threshold_ =  "automatic_threshold";
  const std::string param_junctionp_ =  "junctionp";
  const std::string param_aggressive_junction_closure_ =  "aggressive_junction_closure";
}

//: initialize input and output types
bool bvxm_detect_edge_tangent_process_cons(bprb_func_process& pro);

//: generates the edge map
bool bvxm_detect_edge_tangent_process(bprb_func_process& pro);

#endif // bvxm_detect_edge_tangent_process_h_
