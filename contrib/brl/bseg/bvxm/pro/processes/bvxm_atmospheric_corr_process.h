// This is brl/bseg/bvxm/pro/processes/bvxm_atmospheric_corr_process.h
#ifndef bvxm_atmospheric_corr_process_h_
#define bvxm_atmospheric_corr_process_h_
//:
// \file
// \brief A process for correcting the effects of the atmosphere on image data
// \author J.L. Mundy
// \date January 2, 2010
// \verbatim
//  Modifications
// \endverbatim

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/grid/bvxm_voxel_slab_base.h>

// global variables/functions for bvxm_atmospheric_corr_process_globals
namespace bvxm_atmospheric_corr_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;

  // other global variables
  unsigned ni_= 0;
  unsigned nj_= 0;
  unsigned nplanes_=0;
}

bool bvxm_atmospheric_corr_process_cons(bprb_func_process& pro);

bool bvxm_atmospheric_corr_process(bprb_func_process& pro);

#endif // bvxm_atmospheric_corr_process_h_
