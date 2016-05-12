#include <bvxm/grid/bvxm_voxel_storage_slab_mem.hxx>
#include <bsta/bsta_von_mises.h>
#include <bsta/bsta_attributes.h>
#include <bsta/io/bsta_io_attributes.h>
#include <bsta/io/bsta_io_von_mises.h>

typedef bsta_vsum_num_obs<bsta_von_mises<float, 3> > dir_dist;
BVXM_VOXEL_STORAGE_SLAB_MEM_INSTANTIATE(dir_dist);
