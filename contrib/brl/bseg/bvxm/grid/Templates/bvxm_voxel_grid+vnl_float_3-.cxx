#include <bvxm/grid/bvxm_voxel_grid.hxx>
#include <bvxm/grid/bvxm_voxel_storage_disk.hxx>
#include <bvxm/grid/bvxm_voxel_storage_mem.hxx>
#include <bvxm/grid/bvxm_voxel_storage_slab_mem.hxx>
#include <bvxm/grid/bvxm_voxel_storage_disk_cached.hxx>
#include <vnl/vnl_float_3.h>

BVXM_VOXEL_GRID_INSTANTIATE(vnl_float_3);
BVXM_VOXEL_STORAGE_DISK_INSTANTIATE(vnl_float_3);
BVXM_VOXEL_STORAGE_MEM_INSTANTIATE(vnl_float_3);
BVXM_VOXEL_STORAGE_SLAB_MEM_INSTANTIATE(vnl_float_3);
BVXM_VOXEL_STORAGE_DISK_CACHED_INSTANTIATE(vnl_float_3);
