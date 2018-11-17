// This is brl/bseg/bvxm/pro/processes/bvxm_ocp_compare_process.h
#ifndef bvxm_ocp_compare_process_h_
#define bvxm_ocp_compare_process_h_
//:
// \file
// \brief A class for comparing to occupancy grid for the probability values.
//
// It makes a search in the kxkxk neighborhood and returns the higher value.
// If the value is higher, the similarity is bigger.
//   -  Inputs:
//      * bvxm_voxel_world_sptr
//      * bvxm_voxel_world_sptr
//   -  Output:
//      * double    the similarity measureboolbool
//
// \author  Gamze D. Tunali
// \date    May 15, 2008
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

#include <bvxm/bvxm_voxel_world.h>

//: globals
namespace bvxm_ocp_compare_process_gloabals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 1;

  // functions
  bool save_raw(char *ocp_array, int x, int y, int z, const std::string& filename);
  double compare(const bvxm_voxel_world_sptr& w1, const bvxm_voxel_world_sptr& w2, unsigned n, unsigned scale);
}

//: set input and output types
bool bvxm_ocp_compare_process_cons(bprb_func_process& pro);

bool bvxm_ocp_compare_process(bprb_func_process& pro);


#endif // bvxm_ocp_compare_process_h_
