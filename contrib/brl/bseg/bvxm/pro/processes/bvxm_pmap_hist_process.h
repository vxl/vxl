// This is brl/bseg/bvxm/pro/processes/bvxm_pmap_hist_process.h
#ifndef bvxm_pmap_hist_process_h_
#define bvxm_pmap_hist_process_h_
//:
// \file
// \brief A class for generating a histogram out of occupancy probability grid
//        -  Inputs:
//             * bvxm_voxel_world_sptr
//             * vcl_string             --path for the text file of the histogram
//        -  No outputs
//
// \author  Gamze D. Tunali
// \date    May 22, 2008
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <vcl_string.h>

//: globals
namespace bvxm_pmap_hist_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;

  // functions
  bool compute(vcl_string pmap, vcl_string path);
}

//: set input and output types
bool bvxm_pmap_hist_process_cons(bprb_func_process& pro);

bool bvxm_pmap_hist_process(bprb_func_process& pro);

#endif // bvxm_pmap_hist_process_h_
