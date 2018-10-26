// This is brl/bseg/bvxm/pro/processes/bvxm_ocp_hist_process.h
#ifndef bvxm_ocp_hist_process_h_
#define bvxm_ocp_hist_process_h_
//:
// \file
// \brief A class for generating a histogram out of occupancy probability grid
//        -  Inputs:
//             * bvxm_voxel_world_sptr
//             * std::string             --path for the text file of the histogram
//        -  No outputs
//
// \author  Gamze D. Tunali
// \date    May 22, 2008
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#include <vcl_compiler.h>
#include <bvxm/bvxm_voxel_world.h>

//: global variables for this process
namespace bvxm_ocp_hist_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;

  // functions
  bool compute(bvxm_voxel_world_sptr w, unsigned scale, std::string path);
}

//: set input and output types
bool bvxm_ocp_hist_process_cons(bprb_func_process& pro);

//: generates a histogram out of occupancy probability grid
bool bvxm_ocp_hist_process(bprb_func_process& pro);

#endif // bvxm_ocp_hist_process_h_
