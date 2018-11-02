// This is brl/bseg/bvxm/pro/processes/bvxm_update_edges_process.h
#ifndef bvxm_update_edges_process_h_
#define bvxm_update_edges_process_h_
//:
// \file
// \brief A process that updates voxel world edge probabilities
//
// \author Ibrahim Eden
// \date February 11, 2009
// \verbatim
//  Modifications
//   Peter Vanroose - Jul 10, 2009 - split into .h and .cxx
//   Yi Dong        - May 06, 2014 - avoid using xml file as input to avoid IO confliction when the process is running in parallel
// \endverbatim

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: globals
namespace bvxm_update_edges_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 0;
#if 0
  // parameter strings
  const std::string param_edge_prob_mask_size_ =  "edge_prob_mask_size";
  const std::string param_edge_prob_mask_sigma_ =  "edge_prob_mask_sigma";
#endif
}

//: set input and output types
bool bvxm_update_edges_process_cons(bprb_func_process& pro);

//:  optimizes rpc camera parameters based on edges
bool bvxm_update_edges_process(bprb_func_process& pro);

#endif // bvxm_update_edges_process_h_
