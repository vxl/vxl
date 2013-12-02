#include <bvxm/grid/bvxm_voxel_grid.txx>
#include <bvxm/grid/bvxm_voxel_storage_disk.txx>
#include <bvxm/grid/bvxm_voxel_storage_mem.txx>
#include <bvxm/grid/bvxm_voxel_storage_slab_mem.txx>
#include <bvxm/grid/bvxm_voxel_storage_disk_cached.txx>
#include <vnl/vnl_vector_fixed.h>

typedef vnl_vector_fixed<float,4> vnl_vector_fixed_f4;

BVXM_VOXEL_GRID_INSTANTIATE(vnl_vector_fixed_f4);
BVXM_VOXEL_STORAGE_DISK_INSTANTIATE(vnl_vector_fixed_f4);
BVXM_VOXEL_STORAGE_MEM_INSTANTIATE(vnl_vector_fixed_f4);
BVXM_VOXEL_STORAGE_SLAB_MEM_INSTANTIATE(vnl_vector_fixed_f4);
BVXM_VOXEL_STORAGE_DISK_CACHED_INSTANTIATE(vnl_vector_fixed_f4);

