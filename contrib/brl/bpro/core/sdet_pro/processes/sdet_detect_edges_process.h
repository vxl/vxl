// This is brl/bpro/core/sdet_pro/processes/sdet_detect_edges_process.h
#ifndef sdet_detect_edges_process_h_
#define sdet_detect_edges_process_h_
//:
// \file
// \brief A process that takes a grayscale image and returns the corresponding edge map
//
// \author Ibrahim Eden
// \date March 05, 2008
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
//   Gamze Tunali    - Jan 13, 2010 - moved from bvxm/pro following the move of sdet/sdet_edge.h
//
// \endverbatim

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: global variables
namespace sdet_detect_edges_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;

  // parameter strings
  const std::string param_noise_multiplier_ =  "noise_multiplier";
  const std::string param_smooth_ =  "smooth";
  const std::string param_automatic_threshold_ =  "automatic_threshold";
  const std::string param_junctionp_ =  "junctionp";
  const std::string param_aggressive_junction_closure_ =  "aggressive_junction_closure";
}

//: initialize input and output types
bool sdet_detect_edges_process_cons(bprb_func_process& pro);

//: generates the edge map
bool sdet_detect_edges_process(bprb_func_process& pro);

#endif // sdet_detect_edges_process_h_
