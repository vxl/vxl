// This is brl/bseg/bvxm/pro/processes/bvxm_pmap_ratio_process.h
#ifndef bvxm_pmap_ratio_process_h_
#define bvxm_pmap_ratio_process_h_
//:
// \file
// \brief A class for generating a ratio histogram out of occupancy probability grid
//        -  Inputs:
//             * 0 path to the prob. map image of LIDAR
//             * 1 path to the prob. map image of NON_LIDAR
//             * 2 output path
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
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: globals variables/functions
namespace bvxm_pmap_ratio_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;

  // functions
  bool compute(const std::string& pmap1,const std::string& pmap2, const std::string& path);
}

//: set input and output types
bool bvxm_pmap_ratio_process_cons(bprb_func_process& pro);

//: generates a ratio histogram out of occupancy probability grid
bool bvxm_pmap_ratio_process(bprb_func_process& pro);

#endif // bvxm_pmap_ratio_process_h_
