//These instantiations are all together because, if one class is created, the other ones are needed
//Please leave them together.
#include <bvxm/grid/bvxm_voxel_grid.txx>
#include <bvxm/grid/bvxm_voxel_slab.txx>
#include <bvxm/grid/bvxm_voxel_slab_iterator.txx>
#include <bvxm/grid/bvxm_voxel_storage_disk.txx>
#include <bvxm/grid/bvxm_voxel_storage_disk_cached.txx>
#include <bvxm/grid/bvxm_voxel_storage_mem.txx>
#include <bvxm/grid/bvxm_voxel_storage_slab_mem.txx>
#include <bvpl/util/bvpl_corner_pair_finder.h>
#include <bvpl/bvpl_subgrid_iterator.txx>
#include <bvpl/bvpl_voxel_subgrid.txx>

BVXM_VOXEL_GRID_INSTANTIATE(bvpl_pair);
BVXM_VOXEL_SLAB_INSTANTIATE(bvpl_pair);
BVXM_VOXEL_SLAB_ITERATOR_INSTANTIATE(bvpl_pair);
BVXM_VOXEL_STORAGE_DISK_INSTANTIATE(bvpl_pair);
BVXM_VOXEL_STORAGE_DISK_CACHED_INSTANTIATE(bvpl_pair);
BVXM_VOXEL_STORAGE_MEM_INSTANTIATE(bvpl_pair);
BVXM_VOXEL_STORAGE_SLAB_MEM_INSTANTIATE(bvpl_pair);
BVPL_SUBGRID_ITERATOR_INSTANTIATE(bvpl_pair);
BVPL_VOXEL_SUBGRID_INSTANTIATE(bvpl_pair);
