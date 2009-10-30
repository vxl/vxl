#include "bvxm_voxel_grid_opinion_basic_ops.h"

void bvxm_expectation_opinion_voxel_grid(bvxm_voxel_grid<bvxm_opinion> * grid_in_base,
                                         bvxm_voxel_grid<float> * grid_out_base)
{
  bvxm_voxel_grid<bvxm_opinion>::iterator grid_in_it = grid_in_base->begin();
  bvxm_voxel_grid<float>::iterator grid_out_it = grid_out_base->begin();

  for (;grid_in_it!= grid_in_base->end(); ++grid_in_it, ++grid_out_it)
  {
    //iterate through slab and compute the expectation . At this point the grids get updated on disk
    bvxm_voxel_slab<bvxm_opinion>::iterator in_it = (*grid_in_it).begin();
    bvxm_voxel_slab<float>::iterator out_it = (*grid_out_it).begin();
    for (; in_it!= (*grid_in_it).end(); ++in_it, ++out_it)
       *out_it=(*in_it).expectation();
    }
}

void bvxm_belief_opinion_voxel_grid(bvxm_voxel_grid<bvxm_opinion> * grid_in_base,
                                         bvxm_voxel_grid<float> * grid_out_base)
{
  bvxm_voxel_grid<bvxm_opinion>::iterator grid_in_it = grid_in_base->begin();
  bvxm_voxel_grid<float>::iterator grid_out_it = grid_out_base->begin();

  for (;grid_in_it!= grid_in_base->end(); ++grid_in_it, ++grid_out_it)
  {
    //iterate through slab and compute the expectation . At this point the grids get updated on disk
    bvxm_voxel_slab<bvxm_opinion>::iterator in_it = (*grid_in_it).begin();
    bvxm_voxel_slab<float>::iterator out_it = (*grid_out_it).begin();
    for (; in_it!= (*grid_in_it).end(); ++in_it, ++out_it)
       *out_it=(*in_it).b();
    }
}
