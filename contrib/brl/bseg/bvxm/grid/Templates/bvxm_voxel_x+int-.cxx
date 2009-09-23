//These instantiations are all together because, if one class is created, the other ones are needed
#include <bvxm/grid/bvxm_voxel_grid.txx>
#include <bvxm/grid/bvxm_voxel_slab.txx>
#include <bvxm/grid/bvxm_voxel_slab_iterator.txx>
#include <bvxm/grid/bvxm_voxel_storage_disk.txx>
#include <bvxm/grid/bvxm_voxel_storage_disk_cached.txx>
#include <bvxm/grid/bvxm_voxel_storage_mem.txx>

BVXM_VOXEL_GRID_INSTANTIATE(int);
BVXM_VOXEL_SLAB_INSTANTIATE(int);         
BVXM_VOXEL_SLAB_ITERATOR_INSTANTIATE(int);
BVXM_VOXEL_STORAGE_DISK_INSTANTIATE(int);
BVXM_VOXEL_STORAGE_DISK_CACHED_INSTANTIATE(int);
BVXM_VOXEL_STORAGE_MEM_INSTANTIATE(int);