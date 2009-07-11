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
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <vcl_string.h>

//: globals
namespace bvxm_update_edges_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 1;

  // parameter strings
  const vcl_string param_edt_gaussian_sigma_ =  "edt_gaussian_sigma";
  const vcl_string param_edt_image_mean_scale_ =  "edt_image_mean_scale";
}

//: set input and output types
bool bvxm_update_edges_process_cons(bprb_func_process& pro);

//:  optimizes rpc camera parameters based on edges
bool bvxm_update_edges_process(bprb_func_process& pro);

#endif // bvxm_update_edges_process_h_
