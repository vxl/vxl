//These instantiations are all together because, if one class is created, the other ones are needed
//Please leave them together.
#include <bvxm/grid/bvxm_voxel_grid.hxx>
#include <bvxm/grid/bvxm_voxel_slab.hxx>
#include <bvxm/grid/bvxm_voxel_slab_iterator.hxx>
#include <bvxm/grid/bvxm_voxel_storage_disk.hxx>
#include <bvxm/grid/bvxm_voxel_storage_disk_cached.hxx>
#include <bvxm/grid/bvxm_voxel_storage_mem.hxx>
#include <bvxm/grid/bvxm_voxel_storage_slab_mem.hxx>
#include <bvpl/util/bvpl_corner_pair_finder.h>
#include <bvpl/bvpl_subgrid_iterator.hxx>
#include <bvpl/bvpl_voxel_subgrid.hxx>

BVXM_VOXEL_GRID_INSTANTIATE(bvpl_pair);
BVXM_VOXEL_SLAB_INSTANTIATE(bvpl_pair);
BVXM_VOXEL_SLAB_ITERATOR_INSTANTIATE(bvpl_pair);
BVXM_VOXEL_STORAGE_DISK_INSTANTIATE(bvpl_pair);
BVXM_VOXEL_STORAGE_DISK_CACHED_INSTANTIATE(bvpl_pair);
BVXM_VOXEL_STORAGE_MEM_INSTANTIATE(bvpl_pair);
BVXM_VOXEL_STORAGE_SLAB_MEM_INSTANTIATE(bvpl_pair);
BVPL_SUBGRID_ITERATOR_INSTANTIATE(bvpl_pair);
BVPL_VOXEL_SUBGRID_INSTANTIATE(bvpl_pair);
