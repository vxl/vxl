// This is brl/bseg/bvxm/grid/bvxm_voxel_grid_opinion_basic_ops.h
#ifndef bvxm_voxel_grid_opinion_basic_ops_h
#define bvxm_voxel_grid_opinion_basic_ops_h
//:
// \file
// \brief set of basic grid operations
//
// \author Vishal Jain vj@lems.brown.edu
//
// \date  July 9, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include "bvxm_voxel_grid_base.h"
#include "bvxm_voxel_grid.h"
#include <bvxm/grid/bvxm_opinion.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void bvxm_expectation_opinion_voxel_grid(bvxm_voxel_grid<bvxm_opinion> * grid_in_base,
                                         bvxm_voxel_grid<float> * grid_out_base);
void bvxm_belief_opinion_voxel_grid(bvxm_voxel_grid<bvxm_opinion> * grid_in_base,
                                    bvxm_voxel_grid<float> * grid_out_base);
#endif
