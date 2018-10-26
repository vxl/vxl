// This is brl/bseg/bvxm/bvxm_float_processor.h
#ifndef bvxm_float_processor_h_
#define bvxm_float_processor_h_
//:
// \file
// \brief A class for a float processor.
//
// \author Isabel Restrepo
// \date March 11, 2009
// \verbatim
//  Modifications
//   03/10/2009  Isabel Restrepo. Creation. This processor is intended for appearance model
//               grids that contain only one floating point value. The only method implemented
//               at creation time is expected_value() which returns itself
//               The intention here is to be able to get expected images of grids that contain
//               floating point information
// \endverbatim

#include "grid/bvxm_voxel_slab.h"

#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>


// The mix_gauss_type contains the same data as mix_gauss plus an
// extra attribute that indicate the number of observations

class  bvxm_float_processor
{
 public:

  typedef float apm_datatype;
  typedef float obs_datatype;
  typedef float obs_mathtype;

  bvxm_float_processor() = default;
  virtual ~bvxm_float_processor() = default;

  bvxm_voxel_slab<obs_datatype> expected_color( bvxm_voxel_slab<obs_mathtype> const& appear)
  {return appear;}
};


#endif // bvxm_float_processor_h_
