// This is brl/bseg/bvxm/pro/processes/bvxm_illum_index_process.h
#ifndef bvxm_illum_index_process_h_
#define bvxm_illum_index_process_h_
//:
// \file
// \brief A class for illum_index process of a voxel world.
//
// \author Isabel Restrepo
// \date Feb 20, 2008
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: global variables
namespace bvxm_illum_index_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 1;

  unsigned bin_index(const std::string& map_type, double sun_el, double sun_az,
                     unsigned num_lat, unsigned num_long);
}

//: set input and output types
bool bvxm_illum_index_process_cons(bprb_func_process& pro);

bool bvxm_illum_index_process(bprb_func_process& pro);

#endif // bvxm_illum_index_process_h_
